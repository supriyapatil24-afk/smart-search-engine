#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <iomanip>
#include "trie.h"
#include "graph.h"
#include "hashmap.h"
#include "heap.h"
#include "utils.h"
#include "datapersistence.h"

class SearchEngine {
private:
    Trie trie;
    Graph topicGraph;
    HashMap keywordIndex;
    std::vector<std::string> uploadedFiles;
    DataPersistence dataPersistence;

    void processKeywords(const std::vector<std::string>& keywords, const std::string& filename);
    void buildGraphFromSentences(const std::string& content);

public:
    SearchEngine() : dataPersistence("search_data.dat") {}

    void uploadNote(const std::string& filename);
    void uploadFile(const std::string& filename, const std::string& content);
    std::vector<FileInfo> search(const std::string& keyword);
    std::vector<std::pair<std::string, int>> getRelatedTopics(const std::string& topic);
    std::vector<std::string> getLearningPath(const std::string& topic);
    std::string getSnippet(const std::string& filename, const std::string& keyword);
    std::vector<std::string> getUploadedFiles();
    void searchAndDisplay(const std::string& keyword);
    void displayLearningPath(const std::string& topic);
    void displayMindMap(const std::string& topic);
    void displayMenu();
    void run();
    void saveData();
    void loadData();
};

void SearchEngine::buildGraphFromSentences(const std::string& content) {
    std::vector<std::string> sentences = Utils::splitIntoSentences(content);
    
    for (const auto& sentence : sentences) {
        std::vector<std::string> keywords = Utils::tokenize(sentence);
        
        for (size_t i = 0; i < keywords.size(); ++i) {
            topicGraph.addTopic(keywords[i]);
            for (size_t j = i + 1; j < keywords.size(); ++j) {
                topicGraph.incrementEdgeWeight(keywords[i], keywords[j]);
            }
        }
    }
}

void SearchEngine::processKeywords(const std::vector<std::string>& keywords, const std::string& filename) {
    for (const auto& keyword : keywords) {
        if (keyword.length() > 2) {
            trie.insert(keyword);
            keywordIndex.addKeyword(keyword, filename);
        }
    }
}

void SearchEngine::uploadNote(const std::string& filename) {
    try {
        std::string content = Utils::readFile(filename);
        std::vector<std::string> keywords = Utils::tokenize(content);

        keywordIndex.storeFileContent(filename, content);
        processKeywords(keywords, filename);
        buildGraphFromSentences(content);

        uploadedFiles.push_back(filename);
        std::cout << "\n[OK] Uploaded: " << filename << std::endl;
        std::cout << "    Indexed " << keywords.size() << " keywords\n";

    } catch (const std::exception& e) {
        std::cout << "\n[ERROR] " << e.what() << std::endl;
    }
}

void SearchEngine::uploadFile(const std::string& filename, const std::string& content) {
    try {
        std::vector<std::string> keywords = Utils::tokenize(content);

        keywordIndex.storeFileContent(filename, content);
        processKeywords(keywords, filename);
        buildGraphFromSentences(content);

        uploadedFiles.push_back(filename);
        std::cout << "\n[OK] Uploaded: " << filename << std::endl;
        std::cout << "    Indexed " << keywords.size() << " keywords\n";

    } catch (const std::exception& e) {
        std::cout << "\n[ERROR] " << e.what() << std::endl;
    }
}

std::vector<FileInfo> SearchEngine::search(const std::string& keyword) {
    std::vector<FileInfo> files = keywordIndex.getFiles(keyword);

    // Sort by frequency
    std::sort(files.begin(), files.end(),
              [](const FileInfo& a, const FileInfo& b) {
                  return a.frequency > b.frequency;
              });

    return files;
}

std::vector<std::pair<std::string, int>> SearchEngine::getRelatedTopics(const std::string& topic) {
    return topicGraph.getRelatedTopics(topic);
}

std::vector<std::string> SearchEngine::getLearningPath(const std::string& topic) {
    if (!topicGraph.containsTopic(topic)) {
        return {};
    }

    return topicGraph.getLearningPath(topic, 8);
}

std::string SearchEngine::getSnippet(const std::string& filename, const std::string& keyword) {
    if (!keywordIndex.hasFileContent(filename)) {
        return "File content not available";
    }

    std::string content = keywordIndex.getFileContent(filename);
    return Utils::extractSnippet(content, keyword, 8);
}

std::vector<std::string> SearchEngine::getUploadedFiles() {
    return uploadedFiles;
}

void SearchEngine::searchAndDisplay(const std::string& keyword) {
    // Get search results
    std::vector<FileInfo> files = keywordIndex.getFiles(keyword);
    
    if (!files.empty()) {
        // Sort by frequency
        std::sort(files.begin(), files.end(), 
                  [](const FileInfo& a, const FileInfo& b) { 
                      return a.frequency > b.frequency; 
                  });
        
        std::cout << "\n=== Search Results: " << keyword << " ===\n";
        
        // Show top 5 results
        int limit = std::min(5, (int)files.size());
        for (int i = 0; i < limit; ++i) {
            std::cout << i + 1 << ". " << files[i].filename 
                      << " (" << files[i].frequency << " mentions)\n";
        }
        
        // Show snippet from top result
        if (!files.empty() && keywordIndex.hasFileContent(files[0].filename)) {
            std::string content = keywordIndex.getFileContent(files[0].filename);
            std::string snippet = Utils::extractSnippet(content, keyword, 8);
            
            std::cout << "\n--- Snippet from " << files[0].filename << " ---\n" 
                      << snippet << "\n";
        }
        
        // Show related topics
        auto related = topicGraph.getRelatedTopics(keyword);
        if (!related.empty()) {
            std::cout << "\n--- Related topics ---\n";
            for (size_t i = 0; i < related.size(); ++i) {
                std::cout << "- " << related[i].first 
                          << " (strength: " << related[i].second << ")\n";
            }
        }
        
    } else {
        std::cout << "\n[INFO] No results found for: " << keyword << std::endl;
    }
}

void SearchEngine::displayLearningPath(const std::string& topic) {
    if (!topicGraph.containsTopic(topic)) {
        std::cout << "\n[INFO] Topic not found. Upload notes first.\n";
        return;
    }
    
    auto path = topicGraph.getLearningPath(topic, 8);
    
    if (path.empty() || path.size() < 3) {
        std::cout << "\n[INFO] Insufficient connections to build learning path.\n";
        return;
    }
    
    std::cout << "\n=== Learning Path: " << topic << " ===\n";
    
    for (size_t i = 0; i < path.size(); ++i) {
        std::cout << " " << i + 1 << ". " << path[i] << "\n";
    }
    
    std::cout << "\n[INFO] Suggested study order based on topic relationships\n";
}

void SearchEngine::displayMindMap(const std::string& topic) {
    if (!topicGraph.containsTopic(topic)) {
        std::cout << "\n[INFO] Topic not found. Upload notes first.\n";
        return;
    }
    
    std::cout << "\n=== Mind Map: " << topic << " ===\n";
    
    // Simple indented display
    auto related = topicGraph.getRelatedTopics(topic, 1);
    
    if (related.empty()) {
        std::cout << topic << "\n";
        std::cout << "  (no strong connections found)\n";
        return;
    }
    
    std::cout << topic << "\n";
    for (const auto& rel : related) {
        std::cout << "  |- " << rel.first << " [weight: " << rel.second << "]\n";
    }
}

void SearchEngine::displayMenu() {
    std::cout << "\n=====================================\n";
    std::cout << "    SMART SEARCH ENGINE v2.0\n";
    std::cout << "=====================================\n";
    std::cout << "1. Upload note\n";
    std::cout << "2. Search topic\n";
    std::cout << "3. Generate learning path\n";
    std::cout << "4. View mind map\n";
    std::cout << "5. Save & Exit\n";
    std::cout << "=====================================\n";
    std::cout << "Choice: ";
}

void SearchEngine::saveData() {
    dataPersistence.saveData(trie, topicGraph, keywordIndex);
}

void SearchEngine::loadData() {
    if (!dataPersistence.loadData(trie, topicGraph, keywordIndex)) {
        std::cout << "No saved data found.\n";
    }
}

void SearchEngine::run() {
    loadData();
    
    int choice;
    std::string input;
    
    std::cout << "\nSMART SEARCH ENGINE FOR COLLEGE NOTES\n";
    std::cout << "=======================================\n";
    
    while (true) {
        displayMenu();
        std::cin >> choice;
        std::cin.ignore();
        
        switch (choice) {
            case 1: {
                std::cout << "\nFile path: ";
                std::getline(std::cin, input);
                uploadNote(input);
                break;
            }
            case 2: {
                std::cout << "\nSearch: ";
                std::getline(std::cin, input);
                if (!input.empty()) {
                    searchAndDisplay(input);
                }
                break;
            }
            case 3: {
                std::cout << "\nStart topic: ";
                std::getline(std::cin, input);
                displayLearningPath(input);
                break;
            }
            case 4: {
                std::cout << "\nCenter topic: ";
                std::getline(std::cin, input);
                displayMindMap(input);
                break;
            }
            case 5:
                std::cout << "\nSaving data...\n";
                saveData();
                std::cout << "Goodbye!\n";
                return;
            default:
                std::cout << "\n[ERROR] Invalid choice. Please enter 1-5.\n";
        }
        std::cout << std::endl;
    }
}

int main() {
    SearchEngine engine;
    engine.run();
    return 0;
}
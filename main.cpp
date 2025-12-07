#include <iostream>
#include <string>
#include <vector>
#include <memory>
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
    
    void buildTopicRelationships(const std::vector<std::string>& keywords);
    void processKeywords(const std::vector<std::string>& keywords, const std::string& filename);
    void buildGraphFromSentences(const std::string& content);
    
public:
    SearchEngine() : dataPersistence("search_data.dat") {}
    
    void uploadNote(const std::string& filename);
    std::vector<std::string> searchWithAutocomplete(const std::string& prefix);
    std::vector<SearchResult> searchKeyword(const std::string& keyword, int topK = 5);
    std::vector<std::pair<std::string, int>> getRelatedTopics(const std::string& topic, int maxDepth = 2);
    void displayEnhancedSearchResults(const std::string& keyword);
    void displayLearningPath(const std::string& topic);
    void displayMindMap(const std::string& topic);
    void displayMenu();
    void run();
    void saveData();
    void loadData();
};

void SearchEngine::buildTopicRelationships(const std::vector<std::string>& keywords) {
    for (size_t i = 0; i < keywords.size(); ++i) {
        topicGraph.addTopic(keywords[i]);
        
        for (size_t j = i + 1; j < std::min(i + 4, keywords.size()); ++j) {
            if (keywords[j].length() > 2) {
                topicGraph.addEdge(keywords[i], keywords[j]);
            }
        }
    }
}

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
        
        // Store file content for snippet extraction
        keywordIndex.storeFileContent(filename, content);
        
        processKeywords(keywords, filename);
        buildGraphFromSentences(content);
        
        uploadedFiles.push_back(filename);
        std::cout << "\n✅ File processed successfully! Found " << keywords.size() 
                  << " keywords and built topic relationships." << std::endl;
                  
    } catch (const std::exception& e) {
        std::cout << "\n❌ Error: " << e.what() << std::endl;
    }
}

std::vector<std::string> SearchEngine::searchWithAutocomplete(const std::string& prefix) {
    std::vector<std::string> suggestions = trie.autocomplete(prefix);
    if (suggestions.size() > 5) {
        suggestions.resize(5);
    }
    return suggestions;
}

std::vector<SearchResult> SearchEngine::searchKeyword(const std::string& keyword, int topK) {
    std::vector<FileInfo> files = keywordIndex.getFiles(keyword);
    Heap resultHeap;
    
    for (const auto& fileInfo : files) {
        resultHeap.push(SearchResult(fileInfo.filename, fileInfo.frequency));
    }
    
    return resultHeap.getTopK(topK);
}

std::vector<std::pair<std::string, int>> SearchEngine::getRelatedTopics(const std::string& topic, int maxDepth) {
    return topicGraph.getRelatedTopics(topic, maxDepth);
}

void SearchEngine::displayEnhancedSearchResults(const std::string& keyword) {
    std::vector<SearchResult> results = searchKeyword(keyword);
    
    if (!results.empty()) {
        std::cout << "\n📊 Top Notes:" << std::endl;
        std::cout << std::string(40, '=') << std::endl;
        for (size_t i = 0; i < results.size(); ++i) {
            std::cout << i + 1 << ". " << results[i].filename 
                      << " (" << results[i].frequency << " mentions)" << std::endl;
        }
        
        // Show snippet from the top result
        if (!results.empty() && keywordIndex.hasFileContent(results[0].filename)) {
            std::string content = keywordIndex.getFileContent(results[0].filename);
            std::string snippet = Utils::extractSnippet(content, keyword, 15);
            
            std::cout << "\n💡 Quick Summary from " << results[0].filename << ":" << std::endl;
            std::cout << std::string(50, '-') << std::endl;
            std::cout << snippet << std::endl;
        }
        
        // Show related topics
        std::vector<std::pair<std::string, int>> related = getRelatedTopics(keyword);
        if (!related.empty()) {
            std::cout << "\n🌐 Related Topics (from graph):" << std::endl;
            std::cout << std::string(40, '-') << std::endl;
            for (const auto& topic_pair : related) {
                std::cout << "- " << topic_pair.first << " (weight: " << topic_pair.second << ")" << std::endl;
            }
        }
        
        // Show topic clusters
        std::vector<std::vector<std::string>> clusters = topicGraph.findTopicClusters(2);
        if (!clusters.empty()) {
            std::cout << "\n🔗 Main Topic Cluster:" << std::endl;
            std::cout << std::string(35, '-') << std::endl;
            for (const auto& cluster : clusters) {
                if (std::find(cluster.begin(), cluster.end(), keyword) != cluster.end() || 
                    cluster.size() >= 3) {
                    std::cout << "• ";
                    for (size_t i = 0; i < std::min(cluster.size(), size_t(5)); ++i) {
                        std::cout << cluster[i];
                        if (i < std::min(cluster.size(), size_t(5)) - 1) std::cout << " → ";
                    }
                    if (cluster.size() > 5) std::cout << " ...";
                    std::cout << std::endl;
                    break;
                }
            }
        }
    } else {
        std::cout << "\n❌ No results found for: " << keyword << std::endl;
    }
}

void SearchEngine::displayLearningPath(const std::string& topic) {
    if (!topicGraph.containsTopic(topic)) {
        std::cout << "\n❌ Topic '" << topic << "' not found in the knowledge graph." << std::endl;
        std::cout << "   Try uploading notes containing this topic first." << std::endl;
        return;
    }
    
    std::vector<std::string> learningPath = topicGraph.getLearningPath(topic, 12);
    
    if (learningPath.empty()) {
        std::cout << "\n❌ Could not generate a learning path for '" << topic << "'." << std::endl;
        return;
    }
    
    std::cout << "\n📚 Suggested Learning Path starting from: " << topic << std::endl;
    std::cout << std::string(60, '=') << std::endl;
    
    for (size_t i = 0; i < learningPath.size(); ++i) {
        std::cout << std::setw(3) << (i + 1) << ". " << learningPath[i] << std::endl;
    }
    
    std::cout << std::string(60, '-') << std::endl;
    std::cout << "💡 This path is based on topic co-occurrence in your notes." << std::endl;
    std::cout << "   Topics are ordered by their semantic connections (strongest first)." << std::endl;
}

void SearchEngine::displayMindMap(const std::string& topic) {
    if (!topicGraph.containsTopic(topic)) {
        std::cout << "\n❌ Topic '" << topic << "' not found in the knowledge graph." << std::endl;
        std::cout << "   Try uploading notes containing this topic first." << std::endl;
        return;
    }
    
    // Display ASCII mind map in console
    topicGraph.printMindMap(topic, 3);
    
    // Ask user if they want to export as DOT file
    std::cout << "\n💾 Would you like to export this mind map as a DOT file for visualization? (y/n): ";
    char choice;
    std::cin >> choice;
    std::cin.ignore();
    
    if (choice == 'y' || choice == 'Y') {
        std::string filename = "mindmap_" + topic + ".dot";
        if (topicGraph.exportMindMapAsDOT(topic, filename, 3)) {
            std::cout << "\n✅ Mind map exported to: " << filename << std::endl;
            std::cout << "   To generate an image, run: dot -Tpng " << filename << " -o " << topic << "_mindmap.png" << std::endl;
        }
    }
}

void SearchEngine::displayMenu() {
    std::cout << "\n" << std::string(60, '=') << std::endl;
    std::cout << "           SMART SEARCH ENGINE FOR COLLEGE NOTES" << std::endl;
    std::cout << std::string(60, '=') << std::endl;
    std::cout << "1. 📁 Upload new note" << std::endl;
    std::cout << "2. 🔍 Search a topic" << std::endl;
    std::cout << "3. 🌐 View related topics" << std::endl;
    std::cout << "4. 📚 Generate learning path for a topic" << std::endl;
    std::cout << "5. 🧠 View mind map for a topic" << std::endl;
    std::cout << "6. 💾 Save and Exit" << std::endl;
    std::cout << std::string(60, '-') << std::endl;
    std::cout << "Enter your choice (1-6): ";
}

void SearchEngine::saveData() {
    dataPersistence.saveData(trie, topicGraph, keywordIndex);
}

void SearchEngine::loadData() {
    if (!dataPersistence.loadData(trie, topicGraph, keywordIndex)) {
        std::cout << "No previous data found. Starting fresh..." << std::endl;
    }
}

void SearchEngine::run() {
    loadData();
    
    int choice;
    std::string input;
    
    std::cout << "\n🎯 Welcome to Smart Search Engine for College Notes!" << std::endl;
    std::cout << "   Enhanced features: Text snippets, Topic clustering, Smart ranking\n" << std::endl;
    
    while (true) {
        displayMenu();
        std::cin >> choice;
        std::cin.ignore();
        
        switch (choice) {
            case 1: {
                std::cout << "\n📁 Enter file path: ";
                std::getline(std::cin, input);
                uploadNote(input);
                break;
            }
            case 2: {
                std::cout << "\n🔍 Enter search keyword: ";
                std::getline(std::cin, input);
                
                if (input.empty()) {
                    std::cout << "❌ Please enter a valid keyword." << std::endl;
                    break;
                }
                
                // Autocomplete suggestions
                std::vector<std::string> suggestions = searchWithAutocomplete(input);
                if (!suggestions.empty()) {
                    std::cout << "\n💡 Autocomplete suggestions: ";
                    for (size_t i = 0; i < suggestions.size(); ++i) {
                        std::cout << suggestions[i];
                        if (i < suggestions.size() - 1) std::cout << ", ";
                    }
                    std::cout << std::endl;
                }
                
                std::cout << "🎯 You selected: " << input << std::endl;
                
                // Enhanced search results with snippets and clustering
                displayEnhancedSearchResults(input);
                break;
            }
            case 3: {
                std::cout << "\n🌐 Enter topic to explore: ";
                std::getline(std::cin, input);
                
                std::vector<std::pair<std::string, int>> related = getRelatedTopics(input, 3);
                if (!related.empty()) {
                    std::cout << "\n🔗 Related Topics for '" << input << "' (BFS depth: 3):" << std::endl;
                    std::cout << std::string(50, '-') << std::endl;
                    for (const auto& topic_pair : related) {
                        std::cout << "  ↳ " << topic_pair.first << " [connection strength: " << topic_pair.second << "]" << std::endl;
                    }
                } else {
                    std::cout << "\n❌ No related topics found for: " << input << std::endl;
                    std::cout << "   Try uploading notes containing this topic first." << std::endl;
                }
                break;
            }
            case 4: {
                std::cout << "\n📚 Enter topic for learning path: ";
                std::getline(std::cin, input);
                displayLearningPath(input);
                break;
            }
            case 5: {
                std::cout << "\n🧠 Enter topic for mind map: ";
                std::getline(std::cin, input);
                displayMindMap(input);
                break;
            }
            case 6:
                std::cout << "\n💾 Saving data...";
                saveData();
                std::cout << "\n🎉 Thank you for using Smart Search Engine!" << std::endl;
                std::cout << "   Your data has been saved and will be available next time." << std::endl;
                return;
            default:
                std::cout << "\n❌ Invalid choice. Please enter 1-6." << std::endl;
        }
    }
}

int main() {
    SearchEngine engine;
    engine.run();
    return 0;
}
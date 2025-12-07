#ifndef SEARCHENGINE_H
#define SEARCHENGINE_H

#include <string>
#include <vector>
#include <algorithm>
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

#endif // SEARCHENGINE_H

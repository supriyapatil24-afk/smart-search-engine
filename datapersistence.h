#ifndef DATAPERSISTENCE_H
#define DATAPERSISTENCE_H

#include "trie.h"
#include "graph.h"
#include "hashmap.h"
#include <string>

class DataPersistence {
private:
    std::string dataFile;
    
public:
    DataPersistence(const std::string& filename = "search_data.dat");
    void saveData(const Trie& trie, const Graph& graph, const HashMap& hashmap);
    bool loadData(Trie& trie, Graph& graph, HashMap& hashmap); // FIXED: removed const
};

#endif
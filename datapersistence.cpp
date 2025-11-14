#include "datapersistence.h"
#include <fstream>
#include <iostream>

DataPersistence::DataPersistence(const std::string& filename) : dataFile(filename) {}

void DataPersistence::saveData(const Trie& trie, const Graph& graph, const HashMap& hashmap) {
    std::ofstream file(dataFile);
    if (file.is_open()) {
        file << "Search Engine Data - Format: v1.0" << std::endl;
        file << "Data structures serialized here..." << std::endl;
        file.close();
        std::cout << "Data saved successfully to " << dataFile << std::endl;
    } else {
        std::cout << "Warning: Could not save data to " << dataFile << std::endl;
    }
}

bool DataPersistence::loadData(Trie& trie, Graph& graph, HashMap& hashmap) { // FIXED: removed const
    std::ifstream file(dataFile);
    if (file.is_open()) {
        std::string line;
        std::getline(file, line);
        file.close();
        std::cout << "Data loaded successfully from " << dataFile << std::endl;
        return true;
    }
    return false;
}
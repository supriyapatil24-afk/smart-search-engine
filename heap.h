#ifndef HEAP_H
#define HEAP_H

#include "hashmap.h"
#include <vector>
#include <algorithm>

struct SearchResult {
    std::string filename;
    int frequency;
    double relevance;
    
    SearchResult(const std::string& file, int freq) 
        : filename(file), frequency(freq), relevance(freq) {}
    
    bool operator<(const SearchResult& other) const {
        return relevance > other.relevance; // Min-heap based on relevance
    }
};

class Heap {
private:
    std::vector<SearchResult> heap;
    
    void heapifyUp(int index);
    void heapifyDown(int index);
    
public:
    void push(const SearchResult& result);
    SearchResult pop();
    bool empty() const;
    size_t size() const;
    std::vector<SearchResult> getTopK(int k);
    void clear();
};

#endif
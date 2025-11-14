#include "heap.h"

void Heap::push(const SearchResult& result) {
    heap.push_back(result);
    heapifyUp(heap.size() - 1);
}

void Heap::heapifyUp(int index) {
    while (index > 0) {
        int parent = (index - 1) / 2;
        if (heap[index] < heap[parent]) {
            std::swap(heap[index], heap[parent]);
            index = parent;
        } else {
            break;
        }
    }
}

void Heap::heapifyDown(int index) {
    int size = heap.size();
    while (index < size) {
        int left = 2 * index + 1;
        int right = 2 * index + 2;
        int smallest = index;
        
        if (left < size && heap[left] < heap[smallest]) {
            smallest = left;
        }
        if (right < size && heap[right] < heap[smallest]) {
            smallest = right;
        }
        
        if (smallest != index) {
            std::swap(heap[index], heap[smallest]);
            index = smallest;
        } else {
            break;
        }
    }
}

SearchResult Heap::pop() {
    SearchResult result = heap[0];
    heap[0] = heap.back();
    heap.pop_back();
    heapifyDown(0);
    return result;
}

bool Heap::empty() const {
    return heap.empty();
}

size_t Heap::size() const {
    return heap.size();
}

std::vector<SearchResult> Heap::getTopK(int k) {
    std::vector<SearchResult> temp = heap;
    std::vector<SearchResult> results;
    
    Heap tempHeap;
    for (const auto& result : temp) {
        tempHeap.push(result);
    }
    
    for (int i = 0; i < k && !tempHeap.empty(); ++i) {
        results.push_back(tempHeap.pop());
    }
    
    return results;
}

void Heap::clear() {
    heap.clear();
}
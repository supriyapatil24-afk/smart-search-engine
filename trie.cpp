#include "trie.h"
#include <iostream>

TrieNode::TrieNode() : isEndOfWord(false) {}

TrieNode::~TrieNode() {
    for (auto& pair : children) {
        delete pair.second;
    }
}

Trie::Trie() {
    root = new TrieNode();
}

Trie::~Trie() {
    delete root;
}

void Trie::insert(const std::string& word) {
    TrieNode* current = root;
    for (char c : word) {
        if (current->children.find(c) == current->children.end()) {
            current->children[c] = new TrieNode();
        }
        current = current->children[c];
    }
    current->isEndOfWord = true;
    current->word = word;
}

void Trie::findAllWords(TrieNode* node, std::vector<std::string>& suggestions) {
    if (node->isEndOfWord) {
        suggestions.push_back(node->word);
    }
    
    for (auto& pair : node->children) {
        findAllWords(pair.second, suggestions);
    }
}

std::vector<std::string> Trie::autocomplete(const std::string& prefix) {
    std::vector<std::string> suggestions;
    TrieNode* current = root;
    
    for (char c : prefix) {
        if (current->children.find(c) == current->children.end()) {
            return suggestions;
        }
        current = current->children[c];
    }
    
    findAllWords(current, suggestions);
    return suggestions;
}

bool Trie::search(const std::string& word) {
    TrieNode* current = root;
    for (char c : word) {
        if (current->children.find(c) == current->children.end()) {
            return false;
        }
        current = current->children[c];
    }
    return current->isEndOfWord;
}

void Trie::clear() {
    delete root;
    root = new TrieNode();
}
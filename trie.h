#ifndef TRIE_H
#define TRIE_H

#include <unordered_map>
#include <string>
#include <vector>

class TrieNode {
public:
    std::unordered_map<char, TrieNode*> children;
    bool isEndOfWord;
    std::string word;
    
    TrieNode();
    ~TrieNode();
};

class Trie {
private:
    TrieNode* root;
    
    void findAllWords(TrieNode* node, std::vector<std::string>& suggestions);
    
public:
    Trie();
    ~Trie();
    
    void insert(const std::string& word);
    std::vector<std::string> autocomplete(const std::string& prefix);
    bool search(const std::string& word);
    void clear();
};

#endif
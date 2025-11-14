#include "utils.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <unordered_set>
#include <regex>

std::vector<std::string> Utils::tokenize(const std::string& text) {
    std::vector<std::string> tokens;
    std::stringstream ss(text);
    std::string token;
    
    while (ss >> token) {
        token.erase(std::remove_if(token.begin(), token.end(), 
                   [](unsigned char c) { 
                       return std::ispunct(c) && c != '_' && c != '-'; 
                   }), 
                   token.end());
        token = toLowerCase(token);
        
        if (!token.empty() && !isStopWord(token) && isImportantWord(token)) {
            tokens.push_back(token);
        }
    }
    
    return tokens;
}

std::string Utils::toLowerCase(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

bool Utils::isStopWord(const std::string& word) {
    static const std::unordered_set<std::string> stopWords = {
        "the", "a", "an", "and", "or", "but", "in", "on", "at", "to", "for", 
        "of", "with", "by", "as", "is", "was", "are", "were", "be", "been", 
        "have", "has", "had", "do", "does", "did", "will", "would", "could", 
        "should", "may", "might", "must", "this", "that", "these", "those",
        "it", "its", "they", "them", "their", "what", "which", "who", "whom",
        "when", "where", "why", "how", "all", "any", "both", "each", "few",
        "more", "most", "other", "some", "such", "no", "nor", "not", "only",
        "own", "same", "so", "than", "too", "very", "can", "just", "should", "now"
    };
    
    return stopWords.find(word) != stopWords.end();
}

std::string Utils::readFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: " + filename);
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

std::vector<std::string> Utils::splitIntoSentences(const std::string& text) {
    std::vector<std::string> sentences;
    std::string current_sentence;
    
    for (char c : text) {
        current_sentence += c;
        if (c == '.' || c == '!' || c == '?') {
            current_sentence.erase(0, current_sentence.find_first_not_of(" \t\n\r"));
            current_sentence.erase(current_sentence.find_last_not_of(" \t\n\r") + 1);
            
            if (!current_sentence.empty()) {
                sentences.push_back(current_sentence);
            }
            current_sentence.clear();
        }
    }
    
    if (!current_sentence.empty()) {
        current_sentence.erase(0, current_sentence.find_first_not_of(" \t\n\r"));
        current_sentence.erase(current_sentence.find_last_not_of(" \t\n\r") + 1);
        if (!current_sentence.empty()) {
            sentences.push_back(current_sentence);
        }
    }
    
    return sentences;
}

bool Utils::isImportantWord(const std::string& word) {
    return word.length() > 2 && 
           !std::all_of(word.begin(), word.end(), ::isdigit);
}

std::string Utils::extractSnippet(const std::string& content, const std::string& keyword, int contextWords) {
    std::vector<std::string> words;
    std::stringstream ss(content);
    std::string word;
    
    while (ss >> word) {
        words.push_back(word);
    }
    
    // Find the keyword position (case insensitive)
    std::string lowerKeyword = toLowerCase(keyword);
    int keywordPos = -1;
    for (size_t i = 0; i < words.size(); ++i) {
        std::string lowerWord = toLowerCase(words[i]);
        // Remove punctuation for comparison
        lowerWord.erase(std::remove_if(lowerWord.begin(), lowerWord.end(), 
                      [](unsigned char c) { return std::ispunct(c); }), 
                      lowerWord.end());
        
        if (lowerWord.find(lowerKeyword) != std::string::npos) {
            keywordPos = i;
            break;
        }
    }
    
    if (keywordPos == -1) {
        return "Keyword not found in context.";
    }
    
    // Extract context around the keyword
    int start = std::max(0, keywordPos - contextWords);
    int end = std::min(static_cast<int>(words.size()), keywordPos + contextWords + 1);
    
    std::string snippet;
    for (int i = start; i < end; ++i) {
        snippet += words[i] + " ";
    }
    
    // Clean up the snippet
    if (snippet.length() > 200) {
        snippet = snippet.substr(0, 200) + "...";
    }
    
    return "\"" + snippet + "\"";
}

std::vector<std::string> Utils::extractParagraphs(const std::string& content) {
    std::vector<std::string> paragraphs;
    std::stringstream ss(content);
    std::string paragraph;
    
    while (std::getline(ss, paragraph, '\n')) {
        paragraph.erase(0, paragraph.find_first_not_of(" \t\n\r"));
        paragraph.erase(paragraph.find_last_not_of(" \t\n\r") + 1);
        
        if (!paragraph.empty() && paragraph.length() > 10) {
            paragraphs.push_back(paragraph);
        }
    }
    
    return paragraphs;
}
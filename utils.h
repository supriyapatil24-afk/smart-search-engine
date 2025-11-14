#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>

class Utils {
public:
    static std::vector<std::string> tokenize(const std::string& text);
    static std::string toLowerCase(const std::string& str);
    static bool isStopWord(const std::string& word);
    static std::string readFile(const std::string& filename);
    static std::vector<std::string> splitIntoSentences(const std::string& text);
    static bool isImportantWord(const std::string& word);
    static std::string extractSnippet(const std::string& content, const std::string& keyword, int contextWords = 10);
    static std::vector<std::string> extractParagraphs(const std::string& content);
};

#endif
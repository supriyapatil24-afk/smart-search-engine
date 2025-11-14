#ifndef HASHMAP_H
#define HASHMAP_H

#include <string>
#include <unordered_map>
#include <vector>

struct FileInfo {
    std::string filename;
    int frequency;
    
    FileInfo(const std::string& file, int freq) : filename(file), frequency(freq) {}
};

class HashMap {
private:
    std::unordered_map<std::string, std::vector<FileInfo>> keywordIndex;
    std::unordered_map<std::string, std::string> fileContents; // Store file contents
    
public:
    void addKeyword(const std::string& keyword, const std::string& filename);
    std::vector<FileInfo> getFiles(const std::string& keyword);
    bool containsKeyword(const std::string& keyword);
    void incrementFrequency(const std::string& keyword, const std::string& filename);
    const std::unordered_map<std::string, std::vector<FileInfo>>& getIndex() const;
    void setIndex(const std::unordered_map<std::string, std::vector<FileInfo>>& newIndex);
    
    // New methods for file content storage
    void storeFileContent(const std::string& filename, const std::string& content);
    std::string getFileContent(const std::string& filename);
    bool hasFileContent(const std::string& filename);
};

#endif
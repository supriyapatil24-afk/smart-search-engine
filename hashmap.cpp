#include "hashmap.h"

void HashMap::addKeyword(const std::string& keyword, const std::string& filename) {
    for (auto& fileInfo : keywordIndex[keyword]) {
        if (fileInfo.filename == filename) {
            fileInfo.frequency++;
            return;
        }
    }
    keywordIndex[keyword].push_back(FileInfo(filename, 1));
}

std::vector<FileInfo> HashMap::getFiles(const std::string& keyword) {
    if (keywordIndex.find(keyword) != keywordIndex.end()) {
        return keywordIndex[keyword];
    }
    return std::vector<FileInfo>();
}

bool HashMap::containsKeyword(const std::string& keyword) {
    return keywordIndex.find(keyword) != keywordIndex.end();
}

void HashMap::incrementFrequency(const std::string& keyword, const std::string& filename) {
    for (auto& fileInfo : keywordIndex[keyword]) {
        if (fileInfo.filename == filename) {
            fileInfo.frequency++;
            return;
        }
    }
    keywordIndex[keyword].push_back(FileInfo(filename, 1));
}

const std::unordered_map<std::string, std::vector<FileInfo>>& HashMap::getIndex() const {
    return keywordIndex;
}

void HashMap::setIndex(const std::unordered_map<std::string, std::vector<FileInfo>>& newIndex) {
    keywordIndex = newIndex;
}

void HashMap::storeFileContent(const std::string& filename, const std::string& content) {
    fileContents[filename] = content;
}

std::string HashMap::getFileContent(const std::string& filename) {
    if (fileContents.find(filename) != fileContents.end()) {
        return fileContents[filename];
    }
    return "";
}

bool HashMap::hasFileContent(const std::string& filename) {
    return fileContents.find(filename) != fileContents.end();
}
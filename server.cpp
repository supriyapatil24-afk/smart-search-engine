struct FileInfo {
    string filename;
    int frequency;
};

class SimulatedSearchEngine {
private:
    unordered_map<string, vector<FileInfo>> keywordIndex;
    unordered_map<string, string> fileContents;
    unordered_map<string, vector<pair<string, int>>> topicGraph;
    vector<string> uploadedFiles;

public:
    // Upload and process file
    void uploadFile(const string& filename, const string& content) {
        fileContents[filename] = content;
        uploadedFiles.push_back(filename);
        
        // Simple tokenization (replace with your Utils::tokenize)
        vector<string> words;
        stringstream ss(content);
        string word;
        while (ss >> word) {
            // Remove punctuation
            word.erase(remove_if(word.begin(), word.end(), 
                [](char c) { return ispunct(c); }), word.end());
            
            if (word.length() > 2) {
                words.push_back(word);
                
                // Add to keyword index
                bool found = false;
                for (auto& fi : keywordIndex[word]) {
                    if (fi.filename == filename) {
                        fi.frequency++;
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    keywordIndex[word].push_back({filename, 1});
                }
            }
        }
        
        // Build topic graph (simple co-occurrence)
        for (size_t i = 0; i < words.size(); i++) {
            for (size_t j = i + 1; j < min(i + 5, words.size()); j++) {
                string w1 = words[i], w2 = words[j];
                
                // Add edge both ways
                bool edgeExists = false;
                for (auto& edge : topicGraph[w1]) {
                    if (edge.first == w2) {
                        edge.second++;
                        edgeExists = true;
                        break;
                    }
                }
                if (!edgeExists) {
                    topicGraph[w1].push_back({w2, 1});
                }
                
                edgeExists = false;
                for (auto& edge : topicGraph[w2]) {
                    if (edge.first == w1) {
                        edge.second++;
                        edgeExists = true;
                        break;
                    }
                }
                if (!edgeExists) {
                    topicGraph[w2].push_back({w1, 1});
                }
            }
        }
        
        cout << "Uploaded " << filename << " with " << words.size() << " keywords" << endl;
    }

    // Search for keyword
    vector<pair<string, int>> search(const string& keyword) {
        vector<pair<string, int>> results;
        
        if (keywordIndex.find(keyword) != keywordIndex.end()) {
            for (const auto& fi : keywordIndex[keyword]) {
                results.push_back({fi.filename, fi.frequency});
            }
            
            // Sort by frequency (highest first)
            sort(results.begin(), results.end(),
                [](const pair<string, int>& a, const pair<string, int>& b) {
                    return b.second < a.second;
                });
        }
        
        return results;
    }

    // Get related topics
    vector<pair<string, int>> getRelatedTopics(const string& topic, int maxDepth = 2) {
        vector<pair<string, int>> related;
        
        if (topicGraph.find(topic) != topicGraph.end()) {
            for (const auto& edge : topicGraph[topic]) {
                related.push_back(edge);
            }
            
            // Sort by weight (highest first)
            sort(related.begin(), related.end(),
                [](const pair<string, int>& a, const pair<string, int>& b) {
                    return b.second < a.second;
                });
            
            // Limit results
            if (related.size() > 6) related.resize(6);
        }
        
        return related;
    }

    // Generate learning path
    vector<string> getLearningPath(const string& startTopic, int maxTopics = 8) {
        vector<string> path;
        
        if (topicGraph.find(startTopic) == topicGraph.end()) {
            return path;
        }
        
        path.push_back(startTopic);
        
        unordered_set<string> visited;
        visited.insert(startTopic);
        
        // Simple BFS for learning path
        vector<pair<string, int>> current = {{startTopic, 0}};
        
        while (path.size() < maxTopics && !current.empty()) {
            // Get strongest connection from current topics
            string nextTopic;
            int maxWeight = -1;
            
            for (const auto& topicPair : current) {
                const string& topic = topicPair.first;
                if (topicGraph.find(topic) != topicGraph.end()) {
                    for (const auto& edge : topicGraph[topic]) {
                        if (visited.find(edge.first) == visited.end() && edge.second > maxWeight) {
                            maxWeight = edge.second;
                            nextTopic = edge.first;
                        }
                    }
                }
            }
            
            if (nextTopic.empty()) break;
            
            path.push_back(nextTopic);
            visited.insert(nextTopic);
            current.push_back({nextTopic, maxWeight});
        }
        
        return path;
    }

    // Get all uploaded files
    vector<string> getUploadedFiles() {
        return uploadedFiles;
    }

    // Get snippet from file
    string getSnippet(const string& filename, const string& keyword, int contextWords = 8) {
        if (fileContents.find(filename) == fileContents.end()) {
            return "File content not available";
        }
        
        string content = fileContents[filename];
        vector<string> words;
        stringstream ss(content);
        string word;
        
        while (ss >> word) {
            words.push_back(word);
        }
        
        // Find keyword position
        int keywordPos = -1;
        for (size_t i = 0; i < words.size(); i++) {
            string lowerWord = words[i];
            transform(lowerWord.begin(), lowerWord.end(), lowerWord.begin(), ::tolower);
            
            string lowerKeyword = keyword;
            transform(lowerKeyword.begin(), lowerKeyword.end(), lowerKeyword.begin(), ::tolower);
            
            if (lowerWord.find(lowerKeyword) != string::npos) {
                keywordPos = i;
                break;
            }
        }
        
        if (keywordPos == -1) {
            return "Keyword not found in context";
        }
        
        // Extract snippet
        int start = max(0, keywordPos - contextWords);
        int end = min((int)words.size(), keywordPos + contextWords + 1);
        
        string snippet;
        for (int i = start; i < end; i++) {
            snippet += words[i] + " ";
        }
        
        return "\"" + snippet + "...\"";
    }
};
=======

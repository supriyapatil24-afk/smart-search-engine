#ifndef GRAPH_H
#define GRAPH_H

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <queue>
#include <utility>

struct Edge {
    std::string destination;
    int weight;
    
    Edge(const std::string& dest, int w) : destination(dest), weight(w) {}
};

class Graph {
private:
    std::unordered_map<std::string, std::vector<Edge>> adjacencyList;
    void dfsCluster(const std::string& node, std::unordered_set<std::string>& visited, 
                   std::vector<std::string>& cluster, int minWeight);
    
public:
    void addEdge(const std::string& topic1, const std::string& topic2);
    void addTopic(const std::string& topic);
    std::vector<std::pair<std::string, int>> getRelatedTopics(const std::string& topic, int maxDepth = 2);
    bool containsTopic(const std::string& topic);
    void incrementEdgeWeight(const std::string& topic1, const std::string& topic2);
    const std::unordered_map<std::string, std::vector<Edge>>& getAdjacencyList() const;
    void setAdjacencyList(const std::unordered_map<std::string, std::vector<Edge>>& newList);
    std::vector<std::string> getAllTopics() const;
    std::vector<std::vector<std::string>> findTopicClusters(int minWeight = 2);
    
    // New methods for learning path and mind map
    std::vector<std::string> getLearningPath(const std::string& startTopic, int maxTopics = 10);
    void printMindMap(const std::string& startTopic, int maxDepth = 2) const;
    bool exportMindMapAsDOT(const std::string& startTopic, 
                           const std::string& filename, 
                           int maxDepth = 2) const;
};

#endif
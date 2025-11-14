#include "graph.h"
#include <unordered_set>
#include <queue>
#include <algorithm>
#include <iostream>

void Graph::addEdge(const std::string& topic1, const std::string& topic2) {
    if (topic1 == topic2) return;
    
    for (auto& edge : adjacencyList[topic1]) {
        if (edge.destination == topic2) {
            edge.weight++;
            return;
        }
    }
    
    adjacencyList[topic1].push_back(Edge(topic2, 1));
    adjacencyList[topic2].push_back(Edge(topic1, 1));
}

void Graph::addTopic(const std::string& topic) {
    if (adjacencyList.find(topic) == adjacencyList.end()) {
        adjacencyList[topic] = std::vector<Edge>();
    }
}

std::vector<std::pair<std::string, int>> Graph::getRelatedTopics(const std::string& topic, int maxDepth) {
    std::vector<std::pair<std::string, int>> related;
    if (adjacencyList.find(topic) == adjacencyList.end()) {
        return related;
    }
    
    std::queue<std::pair<std::string, int>> q;
    std::unordered_set<std::string> visited;
    
    q.push({topic, 0});
    visited.insert(topic);
    
    while (!q.empty()) {
        auto current = q.front().first;
        auto depth = q.front().second;
        q.pop();
        
        if (depth > 0 && depth <= maxDepth) {
            for (const auto& edge : adjacencyList[current]) {
                if (visited.find(edge.destination) == visited.end()) {
                    related.push_back({edge.destination, edge.weight});
                }
            }
        }
        
        if (depth < maxDepth) {
            for (const auto& edge : adjacencyList[current]) {
                if (visited.find(edge.destination) == visited.end()) {
                    visited.insert(edge.destination);
                    q.push({edge.destination, depth + 1});
                }
            }
        }
    }
    
    std::sort(related.begin(), related.end(), 
              [](const std::pair<std::string, int>& a, const std::pair<std::string, int>& b) { 
                  return a.second > b.second; 
              });
    
    return related;
}

bool Graph::containsTopic(const std::string& topic) {
    return adjacencyList.find(topic) != adjacencyList.end();
}

void Graph::incrementEdgeWeight(const std::string& topic1, const std::string& topic2) {
    addEdge(topic1, topic2);
}

const std::unordered_map<std::string, std::vector<Edge>>& Graph::getAdjacencyList() const {
    return adjacencyList;
}

void Graph::setAdjacencyList(const std::unordered_map<std::string, std::vector<Edge>>& newList) {
    adjacencyList = newList;
}

std::vector<std::string> Graph::getAllTopics() const {
    std::vector<std::string> topics;
    for (const auto& pair : adjacencyList) {
        topics.push_back(pair.first);
    }
    return topics;
}

void Graph::dfsCluster(const std::string& node, std::unordered_set<std::string>& visited, 
                      std::vector<std::string>& cluster, int minWeight) {
    visited.insert(node);
    cluster.push_back(node);
    
    for (const auto& edge : adjacencyList[node]) {
        if (edge.weight >= minWeight && visited.find(edge.destination) == visited.end()) {
            dfsCluster(edge.destination, visited, cluster, minWeight);
        }
    }
}

std::vector<std::vector<std::string>> Graph::findTopicClusters(int minWeight) {
    std::vector<std::vector<std::string>> clusters;
    std::unordered_set<std::string> visited;
    
    for (const auto& pair : adjacencyList) {
        const std::string& topic = pair.first;
        if (visited.find(topic) == visited.end()) {
            std::vector<std::string> cluster;
            dfsCluster(topic, visited, cluster, minWeight);
            if (cluster.size() > 1) { // Only include clusters with multiple topics
                clusters.push_back(cluster);
            }
        }
    }
    
    // Sort clusters by size (largest first)
    std::sort(clusters.begin(), clusters.end(),
              [](const std::vector<std::string>& a, const std::vector<std::string>& b) {
                  return a.size() > b.size();
              });
    
    return clusters;
}
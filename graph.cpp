#include "graph.h"
#include <unordered_set>
#include <queue>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <functional>

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
    
    // Sort by weight and keep top 6
    std::sort(related.begin(), related.end(), 
              [](const std::pair<std::string, int>& a, const std::pair<std::string, int>& b) { 
                  return a.second > b.second; 
              });
    
    if (related.size() > 6) {
        related.resize(6);
    }
    
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
            if (cluster.size() > 1) {
                clusters.push_back(cluster);
            }
        }
    }
    
    std::sort(clusters.begin(), clusters.end(),
              [](const std::vector<std::string>& a, const std::vector<std::string>& b) {
                  return a.size() > b.size();
              });
    
    return clusters;
}

// =========== NEW IMPROVED METHODS ===========

std::vector<std::string> Graph::getLearningPath(const std::string& startTopic, int maxTopics) {
    std::vector<std::string> learningPath;
    
    if (adjacencyList.find(startTopic) == adjacencyList.end()) {
        return learningPath;
    }

    // Use priority queue: prioritize by connection strength and depth
    struct NodeInfo {
        std::string topic;
        int weight;
        int depth;
        
        bool operator<(const NodeInfo& other) const {
            if (weight != other.weight) return weight < other.weight; // higher weight first
            return depth > other.depth; // shallower depth first
        }
    };

    std::priority_queue<NodeInfo> pq;
    std::unordered_set<std::string> visited;
    
    pq.push({startTopic, 0, 0});
    visited.insert(startTopic);
    
    while (!pq.empty() && learningPath.size() < maxTopics) {
        NodeInfo current = pq.top();
        pq.pop();
        learningPath.push_back(current.topic);
        
        if (learningPath.size() >= maxTopics) break;
        
        // Get and sort neighbors by weight
        std::vector<Edge> neighbors = adjacencyList[current.topic];
        std::sort(neighbors.begin(), neighbors.end(),
                  [](const Edge& a, const Edge& b) {
                      return a.weight > b.weight;
                  });
        
        // Add top 3 strongest connections to queue
        int added = 0;
        for (const auto& edge : neighbors) {
            if (visited.find(edge.destination) == visited.end() && added < 3) {
                visited.insert(edge.destination);
                pq.push({edge.destination, edge.weight, current.depth + 1});
                added++;
            }
        }
    }
    
    return learningPath;
}

void Graph::displayMindMap(const std::string& startTopic, int maxDepth) const {
    if (adjacencyList.find(startTopic) == adjacencyList.end()) {
        std::cout << "Topic not found in knowledge base." << std::endl;
        return;
    }
    
    std::cout << "\n🧠 Mind Map: " << startTopic << std::endl;
    std::cout << "═══════════════════════════════════\n";
    
    std::function<void(const std::string&, int, std::vector<bool>)> printTree;
    printTree = [&](const std::string& node, int depth, std::vector<bool> last) {
        // Print current node with indentation
        for (int i = 0; i < depth; i++) {
            if (i == depth - 1) {
                std::cout << (last[i] ? "└── " : "├── ");
            } else {
                std::cout << (last[i] ? "    " : "│   ");
            }
        }
        
        if (depth > 0) {
            std::cout << node;
            // Show connection strength for immediate children
            if (depth == 1 && adjacencyList.find(startTopic) != adjacencyList.end()) {
                for (const auto& edge : adjacencyList.at(startTopic)) {
                    if (edge.destination == node) {
                        std::cout << " [" << edge.weight << "]";
                        break;
                    }
                }
            }
        } else {
            std::cout << "● " << node;
        }
        std::cout << std::endl;
        
        if (depth >= maxDepth) return;
        
        // Get and sort children by weight
        if (adjacencyList.find(node) != adjacencyList.end()) {
            auto children = adjacencyList.at(node);
            std::sort(children.begin(), children.end(),
                      [](const Edge& a, const Edge& b) { return a.weight > b.weight; });
            
            // Limit to top 4 children for readability - FIXED
            size_t limit = std::min(children.size(), size_t(4));
            for (size_t i = 0; i < limit; ++i) {
                last.push_back(i == limit - 1);
                printTree(children[i].destination, depth + 1, last);
                last.pop_back();
            }
        }
    };
    
    printTree(startTopic, 0, {});
    std::cout << "\n● = Main topic, [n] = Connection strength\n";
}

bool Graph::exportMindMap(const std::string& startTopic, const std::string& filename, int maxDepth) const {
    if (adjacencyList.find(startTopic) == adjacencyList.end()) {
        return false;
    }
    
    std::ofstream dotFile(filename);
    if (!dotFile.is_open()) return false;
    
    dotFile << "digraph MindMap {\n";
    dotFile << "  rankdir=TB;\n";
    dotFile << "  node [shape=box, style=filled, fillcolor=lightblue];\n";
    dotFile << "  edge [penwidth=2];\n\n";
    
    std::queue<std::pair<std::string, int>> q;
    std::unordered_set<std::string> visited;
    
    q.push(std::make_pair(startTopic, 0));
    visited.insert(startTopic);
    
    while (!q.empty()) {
        auto currentPair = q.front();
        std::string current = currentPair.first;
        int depth = currentPair.second;
        q.pop();
        
        dotFile << "  \"" << current << "\" [label=\"" << current << "\"];\n";
        
        if (depth < maxDepth && adjacencyList.find(current) != adjacencyList.end()) {
            auto neighbors = adjacencyList.at(current);
            std::sort(neighbors.begin(), neighbors.end(),
                      [](const Edge& a, const Edge& b) { return a.weight > b.weight; });
            
            for (const auto& edge : neighbors) {
                dotFile << "  \"" << current << "\" -> \"" << edge.destination 
                       << "\" [label=\"" << edge.weight << "\", weight=" << edge.weight << "];\n";
                
                if (visited.find(edge.destination) == visited.end()) {
                    visited.insert(edge.destination);
                    q.push(std::make_pair(edge.destination, depth + 1));
                }
            }
        }
    }
    
    dotFile << "}\n";
    dotFile.close();
    return true;
}
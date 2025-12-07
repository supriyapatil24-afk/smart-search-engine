#include "graph.h"
#include <unordered_set>
#include <queue>
#include <algorithm>
#include <iostream>
#include <fstream>

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

// =========== NEW METHODS FOR LEARNING PATH AND MIND MAP ===========

std::vector<std::string> Graph::getLearningPath(const std::string& startTopic, int maxTopics) {
    std::vector<std::string> learningPath;
    std::unordered_set<std::string> visited;
    
    if (adjacencyList.find(startTopic) == adjacencyList.end()) {
        return learningPath;
    }
    
    std::queue<std::string> q;
    q.push(startTopic);
    visited.insert(startTopic);
    learningPath.push_back(startTopic);
    
    while (!q.empty() && learningPath.size() < maxTopics) {
        std::string current = q.front();
        q.pop();
        
        // Get neighbors sorted by weight (highest first)
        std::vector<Edge> neighbors = adjacencyList[current];
        std::sort(neighbors.begin(), neighbors.end(),
                  [](const Edge& a, const Edge& b) {
                      return a.weight > b.weight;
                  });
        
        for (const auto& edge : neighbors) {
            if (visited.find(edge.destination) == visited.end()) {
                visited.insert(edge.destination);
                learningPath.push_back(edge.destination);
                q.push(edge.destination);
                
                if (learningPath.size() >= maxTopics) {
                    break;
                }
            }
        }
    }
    
    return learningPath;
}

void Graph::printMindMap(const std::string& startTopic, int maxDepth) const {
    if (adjacencyList.find(startTopic) == adjacencyList.end()) {
        std::cout << "Topic '" << startTopic << "' not found in the graph." << std::endl;
        return;
    }
    
    std::cout << "\n🧠 Mind Map for: " << startTopic << " (max depth: " << maxDepth << ")" << std::endl;
    std::cout << std::string(60, '=') << std::endl;
    
    std::queue<std::pair<std::string, int>> q;
    std::unordered_set<std::string> visited;
    std::unordered_map<std::string, int> depthMap;
    
    q.push({startTopic, 0});
    visited.insert(startTopic);
    depthMap[startTopic] = 0;
    
    while (!q.empty()) {
        auto [current, depth] = q.front();
        q.pop();
        
        // Print current node with indentation
        if (depth > 0) {
            for (int i = 0; i < depth; ++i) {
                std::cout << "  ";
            }
            
            // Determine if this is the last child
            if (depth == 1) {
                std::cout << "├─ ";
            } else {
                std::cout << "│  ";
                for (int i = 1; i < depth; ++i) {
                    if (i == depth - 1) {
                        std::cout << "└─ ";
                    } else {
                        std::cout << "   ";
                    }
                }
            }
        }
        
        std::cout << current;
        
        // Sort neighbors by weight
        if (adjacencyList.find(current) != adjacencyList.end()) {
            auto neighbors = adjacencyList.at(current);
            std::sort(neighbors.begin(), neighbors.end(),
                      [](const Edge& a, const Edge& b) {
                          return a.weight > b.weight;
                      });
            
            // Add children to queue if within depth limit
            if (depth < maxDepth) {
                int childCount = 0;
                for (const auto& edge : neighbors) {
                    if (visited.find(edge.destination) == visited.end()) {
                        visited.insert(edge.destination);
                        depthMap[edge.destination] = depth + 1;
                        q.push({edge.destination, depth + 1});
                        childCount++;
                    }
                }
                
                if (childCount > 0 && depth < maxDepth - 1) {
                    std::cout << " [" << childCount << " subtopics]";
                }
            }
        }
        
        std::cout << std::endl;
    }
    
    std::cout << std::string(60, '-') << std::endl;
}

bool Graph::exportMindMapAsDOT(const std::string& startTopic, 
                              const std::string& filename, 
                              int maxDepth) const {
    if (adjacencyList.find(startTopic) == adjacencyList.end()) {
        std::cerr << "Topic '" << startTopic << "' not found in the graph." << std::endl;
        return false;
    }
    
    std::ofstream dotFile(filename);
    if (!dotFile.is_open()) {
        std::cerr << "Error: Could not open file " << filename << " for writing." << std::endl;
        return false;
    }
    
    // DOT file header
    dotFile << "digraph MindMap {" << std::endl;
    dotFile << "  rankdir=TB;" << std::endl;
    dotFile << "  node [shape=rectangle, style=filled, fillcolor=lightblue];" << std::endl;
    dotFile << "  edge [fontsize=10];" << std::endl;
    dotFile << std::endl;
    
    // Perform BFS to collect nodes and edges
    std::queue<std::pair<std::string, int>> q;
    std::unordered_set<std::string> visited;
    
    q.push({startTopic, 0});
    visited.insert(startTopic);
    
    while (!q.empty()) {
        auto [current, depth] = q.front();
        q.pop();
        
        // Export node
        dotFile << "  \"" << current << "\" [label=\"" << current;
        
        // Add neighbor information if within depth
        if (adjacencyList.find(current) != adjacencyList.end() && depth < maxDepth) {
            auto neighbors = adjacencyList.at(current);
            std::sort(neighbors.begin(), neighbors.end(),
                      [](const Edge& a, const Edge& b) {
                          return a.weight > b.weight;
                      });
            
            // Export edges
            for (const auto& edge : neighbors) {
                dotFile << "\"];" << std::endl;
                dotFile << "  \"" << current << "\" -> \"" << edge.destination 
                       << "\" [label=\"weight: " << edge.weight << "\"];" << std::endl;
                
                if (visited.find(edge.destination) == visited.end() && depth < maxDepth - 1) {
                    visited.insert(edge.destination);
                    q.push({edge.destination, depth + 1});
                }
            }
        } else {
            dotFile << "\"];" << std::endl;
        }
    }
    
    dotFile << "}" << std::endl;
    dotFile.close();
    
    std::cout << "✓ DOT file exported to: " << filename << std::endl;
    std::cout << "  To generate image: dot -Tpng " << filename << " -o mindmap.png" << std::endl;
    
    return true;
}
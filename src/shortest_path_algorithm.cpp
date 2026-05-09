#include "shortest_path_algorithm.hpp"

ShortestPathAlgorithm::ShortestPathAlgorithm(const Graph &graph) 
    : m_graph(&graph) {
    // 将图转换为邻接表
    for (const Edge *edge : graph.second) {
        m_adjacencyList[edge->from].push_back(edge);
    }
}

ShortestPathAlgorithm::~ShortestPathAlgorithm() {}

double ShortestPathAlgorithm::euclideanDistance(const Node *a, const Node *b) const {
    if (a->address.size() != b->address.size()) {
        return 0.0;
    }
    double sum = 0.0;
    for (size_t i = 0; i < a->address.size(); ++i) {
        double diff = static_cast<double>(a->address[i] - b->address[i]);
        sum += diff * diff;
    }
    return std::sqrt(sum);
}

int ShortestPathAlgorithm::getCurrentFlow(const Edge *edge) const {
    if (m_flowQueryFunc) {
        return m_flowQueryFunc(edge);
    }
    return 0; // 默认返回0
}

double ShortestPathAlgorithm::getMinP1() const {
    if (m_minP1Getter) {
        return m_minP1Getter();
    }
    return 1.0; // 默认返回1.0
}

double ShortestPathAlgorithm::getTimeHeuristic(const Node *current, const Node *end) const {
    // 时间启发函数：欧几里得距离 * 全局最小p1参数
    // 这是从当前节点到目标节点的最小可能时间
    double dist = euclideanDistance(current, end);
    double minP1 = getMinP1();
    return dist * minP1;
}

bool ShortestPathAlgorithm::aStarDistance(const Node *start, const Node *end,
                                          std::unordered_map<const Node*, const Edge*> &cameFrom) {
    if (start == end) return true;

    // 优先队列：f值最小的节点优先
    using PQElement = std::pair<double, const Node*>;
    std::priority_queue<PQElement, std::vector<PQElement>, std::greater<PQElement>> openSet;

    std::unordered_map<const Node*, double> gScore; // 起点到当前节点的实际距离
    std::unordered_map<const Node*, double> fScore; // g + h

    // 初始化
    gScore[start] = 0.0;
    fScore[start] = euclideanDistance(start, end);
    openSet.push({fScore[start], start});

    while (!openSet.empty()) {
        const Node *current = openSet.top().second;
        openSet.pop();

        if (current == end) {
            return true;
        }

        // 遍历邻接边
        auto it = m_adjacencyList.find(current);
        if (it == m_adjacencyList.end()) continue;

        for (const Edge *edge : it->second) {
            const Node *neighbor = edge->to;
            double tentativeG = gScore[current] + edge->length;

            if (gScore.find(neighbor) == gScore.end() || tentativeG < gScore[neighbor]) {
                cameFrom[neighbor] = edge;
                gScore[neighbor] = tentativeG;
                fScore[neighbor] = tentativeG + euclideanDistance(neighbor, end);
                openSet.push({fScore[neighbor], neighbor});
            }
        }
    }

    return false; // 未找到路径
}

bool ShortestPathAlgorithm::aStarTime(const Node *start, const Node *end,
                                      std::unordered_map<const Node*, const Edge*> &cameFrom) {
    if (start == end) return true;

    // 优先队列：f值最小的节点优先
    using PQElement = std::pair<double, const Node*>;
    std::priority_queue<PQElement, std::vector<PQElement>, std::greater<PQElement>> openSet;

    std::unordered_map<const Node*, double> gScore; // 起点到当前节点的实际时间
    std::unordered_map<const Node*, double> fScore; // g + h

    // 初始化
    gScore[start] = 0.0;
    // 启发函数：欧几里得距离 * 全局最小p1参数（最小可能时间）
    fScore[start] = getTimeHeuristic(start, end);
    openSet.push({fScore[start], start});

    while (!openSet.empty()) {
        const Node *current = openSet.top().second;
        openSet.pop();

        if (current == end) {
            return true;
        }

        // 遍历邻接边
        auto it = m_adjacencyList.find(current);
        if (it == m_adjacencyList.end()) continue;

        for (const Edge *edge : it->second) {
            const Node *neighbor = edge->to;
            // 获取当前边的车流量，计算实际用时
            int flow = getCurrentFlow(edge);
            double tentativeG = gScore[current] + edge->getTime(flow);

            if (gScore.find(neighbor) == gScore.end() || tentativeG < gScore[neighbor]) {
                cameFrom[neighbor] = edge;
                gScore[neighbor] = tentativeG;
                // 启发函数：欧几里得距离 * 全局最小p1参数
                fScore[neighbor] = tentativeG + getTimeHeuristic(neighbor, end);
                openSet.push({fScore[neighbor], neighbor});
            }
        }
    }

    return false; // 未找到路径
}

Graph ShortestPathAlgorithm::reconstructPath(const Node *start, const Node *end,
                                             const std::unordered_map<const Node*, const Edge*> &cameFrom) {
    Graph path;
    std::set<const Node*> nodes;
    std::set<const Edge*> edges;

    if (start == end) {
        nodes.insert(start);
        return std::make_pair(nodes, edges);
    }

    const Node *current = end;
    while (current != nullptr && current != start) {
        nodes.insert(current);
        auto it = cameFrom.find(current);
        if (it == cameFrom.end()) {
            return {}; // 路径不完整
        }
        edges.insert(it->second);
        current = it->second->from;
    }
    
    if (current == start) {
        nodes.insert(start);
    } else {
        return {}; // 路径不完整
    }

    return std::make_pair(nodes, edges);
}

Graph ShortestPathAlgorithm::queryShortestPath(const Node *start, const Node *end) {
    std::unordered_map<const Node*, const Edge*> cameFrom;
    if (aStarDistance(start, end, cameFrom)) {
        return reconstructPath(start, end, cameFrom);
    }
    return {};
}

Graph ShortestPathAlgorithm::queryShortestTimePath(const Node *start, const Node *end) {
    std::unordered_map<const Node*, const Edge*> cameFrom;
    if (aStarTime(start, end, cameFrom)) {
        return reconstructPath(start, end, cameFrom);
    }
    return {};
}
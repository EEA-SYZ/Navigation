#include "data_maker.hpp"

#include <algorithm>
#include <limits>
#include <numeric>
#include <unordered_map>

namespace {
/**
 * @brief 计算两个节点的欧氏距离平方
 */
double distanceSquared(const Node *a, const Node *b) {
    double dx = a->x - b->x;
    double dy = a->y - b->y;
    return dx * dx + dy * dy;
}

double distanceToBoundary(
    const Node *node,
    double left, double right, double bottom, double top
) {
    return std::min({
        node->x - left,
        right - node->x,
        node->y - bottom,
        top - node->y
    });
}

bool isInsideCoreMargin(
    const Node *node,
    double left, double right, double bottom, double top,
    double margin
) {
    return node->x >= left + margin && node->x <= right - margin &&
           node->y >= bottom + margin && node->y <= top - margin;
}

int countNearbyNodes(
    const Node *node,
    const std::vector<Node*> &nodes,
    const NodeSpatialGrid &grid,
    double radius
) {
    int count = 0;
    double radiusSquared = radius * radius;
    grid.visitNearby(node->x, node->y, radius, [&](int nodeIndex) {
        if (distanceSquared(node, nodes[nodeIndex]) <= radiusSquared) {
            ++count;
        }
        return true;
    });
    return count;
}

/**
 * @brief 在核心点网格中查找距离节点最近的核心点
 */
int findNearestCore(
    const Node *node,
    const std::vector<Node*> &corePoints,
    const std::unordered_map<const Node*, int> &nodeIds,
    const NodeSpatialGrid &coreGrid,
    double initialRadius
) {
    int nearestCore = nodeIds.at(corePoints.front());
    double nearestDistance = std::numeric_limits<double>::infinity();
    double radius = std::max(initialRadius, 1.0);

    // 逐步扩大查询半径，优先在局部网格中查找最近核心点
    for (int attempt = 0; attempt < 32; ++attempt) {
        bool found = false;
        coreGrid.visitNearby(node->x, node->y, radius, [&](int coreIndex) {
            double dist = distanceSquared(node, corePoints[coreIndex]);
            if (dist < nearestDistance) {
                nearestDistance = dist;
                nearestCore = nodeIds.at(corePoints[coreIndex]);
            }
            found = true;
            return true;
        });

        if (found && nearestDistance <= radius * radius) {
            return nearestCore;
        }

        radius *= 2.0;
    }

    // 极端情况下回退到全量扫描，保证一定能找到归属核心点
    for (const Node *core : corePoints) {
        double dist = distanceSquared(node, core);
        if (dist < nearestDistance) {
            nearestDistance = dist;
            nearestCore = nodeIds.at(core);
        }
    }

    return nearestCore;
}

} // namespace

/**
 * @brief 选取多层核心点，并为每个节点写入层级地址
 */
void selectCorePoint(
    std::vector<Node*> &nodes, int level_num, double base_radius,
    double left, double right, double bottom, double top
) {
    if (nodes.empty() || level_num <= 0) {
        return;
    }

    // 初始化每个节点的层级地址
    for (Node *node : nodes) {
        if (node == nullptr) {
            continue;
        }
        node->address.assign(level_num, -1);
    }

    std::vector<Node*> currentLevelNodes;
    currentLevelNodes.reserve(nodes.size());
    std::unordered_map<const Node*, int> nodeIds;
    // 第0层为普通节点，每个节点归属于自身
    for (std::size_t i = 0; i < nodes.size(); ++i) {
        Node *node = nodes[i];
        if (node == nullptr) {
            continue;
        }
        node->address[0] = static_cast<int>(i);
        nodeIds[node] = static_cast<int>(i);
        currentLevelNodes.emplace_back(node);
    }

    if (currentLevelNodes.empty()) {
        return;
    }

    base_radius = std::max(base_radius, 1.0);
    double normalizedLeft = std::min(left, right);
    double normalizedRight = std::max(left, right);
    double normalizedBottom = std::min(bottom, top);
    double normalizedTop = std::max(bottom, top);
    left = normalizedLeft;
    right = normalizedRight;
    bottom = normalizedBottom;
    top = normalizedTop;
    double width = std::max(right - left, 1.0);
    double height = std::max(top - bottom, 1.0);

    // 计算层级半径倍数
    auto pow = [&](int a, int x) {
        int ret = 1;
        while (x) {
            if (x & 1) {
                ret *= a;
            }
            a *= a;
            x >>= 1;
        }
        return ret;
    };

    // 从低层核心点中继续筛选更高层核心点
    for (int level = 1; level < level_num; ++level) {
        double levelRadius = base_radius * pow(2, level);
        double levelRadiusSquared = levelRadius * levelRadius;
        double coreMargin = 0.0;
        if (level >= 2) {
            coreMargin = std::min(levelRadius * 0.6, std::min(width, height) * 0.25);
        }

        NodeSpatialGrid densityGrid(left, right, bottom, top, levelRadius);
        for (std::size_t i = 0; i < currentLevelNodes.size(); ++i) {
            densityGrid.add(currentLevelNodes[i], static_cast<int>(i));
        }

        std::vector<double> candidateScores(currentLevelNodes.size(), 0.0);
        std::vector<double> boundaryDistances(currentLevelNodes.size(), 0.0);
        for (std::size_t i = 0; i < currentLevelNodes.size(); ++i) {
            Node *candidate = currentLevelNodes[i];
            double boundaryDistance = distanceToBoundary(candidate, left, right, bottom, top);
            int nearbyCount = countNearbyNodes(
                candidate, currentLevelNodes, densityGrid, levelRadius * 2.0
            );
            boundaryDistances[i] = boundaryDistance;
            candidateScores[i] = nearbyCount + boundaryDistance / std::max(levelRadius, 1.0) * 3.0;
        }

        std::vector<int> order(currentLevelNodes.size());
        std::iota(order.begin(), order.end(), 0);
        std::sort(order.begin(), order.end(), [&](int lhs, int rhs) {
            if (candidateScores[lhs] != candidateScores[rhs]) {
                return candidateScores[lhs] > candidateScores[rhs];
            }
            if (boundaryDistances[lhs] != boundaryDistances[rhs]) {
                return boundaryDistances[lhs] > boundaryDistances[rhs];
            }
            return lhs < rhs;
        });

        // 使用空间网格存储本层已选核心点
        std::vector<Node*> corePoints;
        NodeSpatialGrid coreSelectGrid(left, right, bottom, top, levelRadius);
        for (int idx : order) {
            Node *candidate = currentLevelNodes[idx];
            if (coreMargin > 0.0 && !isInsideCoreMargin(
                candidate, left, right, bottom, top, coreMargin
            )) {
                continue;
            }

            bool valid = true;

            // 只检查候选点附近网格中的已选核心点
            coreSelectGrid.visitNearby(candidate->x, candidate->y, levelRadius, [&](int coreIndex) {
                if (distanceSquared(candidate, corePoints[coreIndex]) < levelRadiusSquared) {
                    valid = false;
                    return false;
                }
                return true;
            });

            if (valid) {
                coreSelectGrid.add(candidate, static_cast<int>(corePoints.size()));
                corePoints.emplace_back(candidate);
            }
        }

        // 理论上不会为空，保留兜底保证层级存在
        if (corePoints.empty()) {
            corePoints.emplace_back(currentLevelNodes[order.front()]);
        }

        // 构建本层核心点归属查询网格
        NodeSpatialGrid coreAssignGrid(left, right, bottom, top, levelRadius);
        for (std::size_t i = 0; i < corePoints.size(); ++i) {
            coreAssignGrid.add(corePoints[i], static_cast<int>(i));
        }

        // 只为上一层核心点分配最近的本层核心点。
        // 普通节点的本层父节点可以由上一层父节点继续向上继承。
        std::unordered_map<int, int> parentCoreByPreviousCore;
        parentCoreByPreviousCore.reserve(currentLevelNodes.size());
        for (Node *node : currentLevelNodes) {
            if (node == nullptr) {
                continue;
            }

            int previousCore = nodeIds.at(node);
            parentCoreByPreviousCore[previousCore] = findNearestCore(
                node, corePoints, nodeIds, coreAssignGrid, levelRadius
            );
        }

        // 为每个普通节点继承上一层核心点的本层归属
        for (Node *node : nodes) {
            if (node == nullptr) {
                continue;
            }

            int previousCore = node->address[level - 1];
            auto iter = parentCoreByPreviousCore.find(previousCore);
            if (iter != parentCoreByPreviousCore.end()) {
                node->address[level] = iter->second;
            }
        }

        // 下一层只从当前层核心点中继续筛选
        currentLevelNodes = corePoints;
    }
}

DataMaker::DataMaker(
    double left, double right, double top, double bottom,
    int node_num, int edge_num, 
    int level_num, int level_volume
) {
    std::vector<Node*> nodes;
    double r = 0;

    // 生成基础泊松点集
    PoissonSampling sampling;
    sampling.generateNotes(left, right, top, bottom, nodes, node_num, r);

    // 在基础点集中选取多层核心点
    selectCorePoint(nodes, level_num, r, left, right, bottom, top);

    // 将生成节点加入图中
    for (const Node *node : nodes) {
        this->graph.first.insert(node);
    }
}

const Graph &DataMaker::getGraph() {
    return this->graph;
}

DataMaker::~DataMaker() {
    for (const Edge *edge : this->graph.second) {
        delete edge;
    }
    for (const Node *node : this->graph.first) {
        delete node;
    }
}

int DataMaker::queryCurrentFlowInEdge(Edge *edge) {
    return 0;
}

#include "data_maker.hpp"

#include <algorithm>
#include <limits>
#include <numeric>
#include <random>
#include <unordered_map>
#include <unordered_set>
#include <cmath>

namespace {
/**
 * @brief 计算两个节点的欧氏距离平方
 */
double distanceSquared(const Node *a, const Node *b) {
    double dx = a->x - b->x;
    double dy = a->y - b->y;
    return dx * dx + dy * dy;
}

double distanceSquared(double ax, double ay, double bx, double by) {
    double dx = ax - bx;
    double dy = ay - by;
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

int representativeLevel(const Node *node) {
    if (node == nullptr || node->address.empty()) {
        return 0;
    }

    int level = 0;
    int self = node->address.front();
    for (std::size_t i = 1; i < node->address.size(); ++i) {
        if (node->address[i] == self) {
            level = static_cast<int>(i);
        }
    }
    return level;
}

int commonAddressLevel(const Node *a, const Node *b) {
    if (a == nullptr || b == nullptr) {
        return 0;
    }

    int limit = static_cast<int>(std::min(a->address.size(), b->address.size()));
    int level = 0;
    for (int i = 1; i < limit; ++i) {
        if (a->address[i] >= 0 && a->address[i] == b->address[i]) {
            level = i;
        }
    }
    return level;
}

double distance(const Node *a, const Node *b) {
    return std::sqrt(distanceSquared(a, b));
}

unsigned long long edgeKey(int a, int b) {
    if (a > b) {
        std::swap(a, b);
    }
    return (static_cast<unsigned long long>(static_cast<unsigned int>(a)) << 32) |
           static_cast<unsigned int>(b);
}

struct CandidateEdge {
    Edge edge;
    int fromIndex;
    int toIndex;
    int level;
    int priority;
};

class DisjointSet {
public:
    explicit DisjointSet(int size) : parent(size), rank(size, 0), componentCount(size) {
        std::iota(parent.begin(), parent.end(), 0);
    }

    int find(int value) {
        if (parent[value] != value) {
            parent[value] = find(parent[value]);
        }
        return parent[value];
    }

    bool unite(int lhs, int rhs) {
        int lhsRoot = find(lhs);
        int rhsRoot = find(rhs);
        if (lhsRoot == rhsRoot) {
            return false;
        }

        if (rank[lhsRoot] < rank[rhsRoot]) {
            std::swap(lhsRoot, rhsRoot);
        }
        parent[rhsRoot] = lhsRoot;
        if (rank[lhsRoot] == rank[rhsRoot]) {
            ++rank[lhsRoot];
        }
        --componentCount;
        return true;
    }

    int components() const {
        return componentCount;
    }

private:
    std::vector<int> parent;
    std::vector<int> rank;
    int componentCount;
};

constexpr double GeometryEpsilon = 1e-9;

double cross(const Node *a, const Node *b, const Node *c) {
    return (b->x - a->x) * (c->y - a->y) - (b->y - a->y) * (c->x - a->x);
}

bool pointOnSegment(const Node *point, const Node *a, const Node *b) {
    if (std::abs(cross(a, b, point)) > GeometryEpsilon) {
        return false;
    }

    return point->x >= std::min(a->x, b->x) - GeometryEpsilon &&
           point->x <= std::max(a->x, b->x) + GeometryEpsilon &&
           point->y >= std::min(a->y, b->y) - GeometryEpsilon &&
           point->y <= std::max(a->y, b->y) + GeometryEpsilon;
}

bool segmentsIntersect(const Node *a, const Node *b, const Node *c, const Node *d) {
    double abC = cross(a, b, c);
    double abD = cross(a, b, d);
    double cdA = cross(c, d, a);
    double cdB = cross(c, d, b);

    if (((abC > GeometryEpsilon && abD < -GeometryEpsilon) ||
         (abC < -GeometryEpsilon && abD > GeometryEpsilon)) &&
        ((cdA > GeometryEpsilon && cdB < -GeometryEpsilon) ||
         (cdA < -GeometryEpsilon && cdB > GeometryEpsilon))) {
        return true;
    }

    return pointOnSegment(c, a, b) ||
           pointOnSegment(d, a, b) ||
           pointOnSegment(a, c, d) ||
           pointOnSegment(b, c, d);
}

bool edgeWouldCrossGraph(const Graph &graph, const Node *u, const Node *v) {
    for (const Edge *edge : graph.second) {
        if (edge == nullptr || edge->from == nullptr || edge->to == nullptr) {
            continue;
        }

        // Sharing an endpoint is a normal planar adjacency, not a crossing.
        if (edge->from == u || edge->from == v || edge->to == u || edge->to == v) {
            continue;
        }

        if (segmentsIntersect(u, v, edge->from, edge->to)) {
            return true;
        }
    }

    return false;
}

class EdgeSpatialIndex {
public:
    EdgeSpatialIndex(
        const std::vector<Node*> &nodes,
        double left,
        double right,
        double bottom,
        double top,
        double cellSize
    ) : nodes(nodes) {
        leftBound = std::min(left, right);
        rightBound = std::max(left, right);
        bottomBound = std::min(bottom, top);
        topBound = std::max(bottom, top);
        this->cellSize = std::max(cellSize, 1.0);
        double width = std::max(rightBound - leftBound, this->cellSize);
        double height = std::max(topBound - bottomBound, this->cellSize);
        cols = std::max(1, static_cast<int>(std::ceil(width / this->cellSize)));
        rows = std::max(1, static_cast<int>(std::ceil(height / this->cellSize)));
        buckets.assign(cols * rows, {});
    }

    bool wouldCross(int from, int to) const {
        std::unordered_set<int> visited;
        // 只遍历候选边包围盒覆盖到的网格桶，避免每次补边都扫描全部已有边。
        // 边的覆盖网格为，以两端点为对角线的矩形
        forEachCell(from, to, [&](int cellIndex) {
            for (int edgeIndex : buckets[cellIndex]) {
                if (!visited.insert(edgeIndex).second) {
                    continue;
                }
                const auto &edge = edges[edgeIndex];
                if (edge.first == from || edge.first == to ||
                    edge.second == from || edge.second == to) {
                    continue;
                }
                if (segmentsIntersect(
                    nodes[from],
                    nodes[to],
                    nodes[edge.first],
                    nodes[edge.second]
                )) {
                    return false;
                }
            }
            return true;
        });

        return crossingFound;
    }

    void add(int from, int to) {
        int edgeIndex = static_cast<int>(edges.size());
        edges.emplace_back(from, to);
        // 按边的包围盒登记到网格中，后续相交检测只需检查局部桶。
        forEachCell(from, to, [&](int cellIndex) {
            buckets[cellIndex].emplace_back(edgeIndex);
            return true;
        });
    }

private:
    const std::vector<Node*> &nodes;
    std::vector<std::pair<int, int>> edges;
    std::vector<std::vector<int>> buckets;
    double leftBound = 0.0;
    double rightBound = 1.0;
    double bottomBound = 0.0;
    double topBound = 1.0;
    double cellSize = 1.0;
    int cols = 1;
    int rows = 1;
    mutable bool crossingFound = false;

    int cellX(double x) const {
        int ret = static_cast<int>((x - leftBound) / cellSize);
        return std::clamp(ret, 0, cols - 1);
    }

    int cellY(double y) const {
        int ret = static_cast<int>((y - bottomBound) / cellSize);
        return std::clamp(ret, 0, rows - 1);
    }

    template <class Visitor>
    void forEachCell(int from, int to, Visitor visitor) const {
        const Node *u = nodes[from];
        const Node *v = nodes[to];
        int minX = cellX(std::min(u->x, v->x));
        int maxX = cellX(std::max(u->x, v->x));
        int minY = cellY(std::min(u->y, v->y));
        int maxY = cellY(std::max(u->y, v->y));

        for (int x = minX; x <= maxX; ++x) {
            for (int y = minY; y <= maxY; ++y) {
                if (!visitor(y * cols + x)) {
                    crossingFound = true;
                    return;
                }
            }
        }
        crossingFound = false;
    }
};

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

std::mt19937 edgeRng(std::chrono::steady_clock::now().time_since_epoch().count());

double edgeParamJitter(double base, double range) {
    std::uniform_real_distribution<double> dist(-range, range);
    return base + dist(edgeRng);
}

bool addGraphEdge(
    Graph &graph,
    std::vector<Node*> &nodes,
    int from,
    int to,
    int level,
    int baseVolume,
    int edgeIndex
) {
    if (from == to || from < 0 || to < 0 ||
        from >= static_cast<int>(nodes.size()) ||
        to >= static_cast<int>(nodes.size())) {
        return false;
    }

    Node *u = nodes[from];
    Node *v = nodes[to];
    if (u == nullptr || v == nullptr) {
        return false;
    }

    int uRep = representativeLevel(u);
    int vRep = representativeLevel(v);
    int maxRep = std::max(uRep, vRep);

    int inferredType = 0;
    if (maxRep >= 2 && level >= 2) {
        inferredType = 2;
    } else if (level >= 1) {
        inferredType = 1;
    } else if (maxRep >= 1) {
        inferredType = 3;
    }

    Edge *edge = new Edge();
    edge->name = "E" + std::to_string(edgeIndex);
    edge->from = u;
    edge->to = v;
    edge->length = distance(u, v);

    switch (inferredType) {
    case 0:
        edge->volume = std::max(1, static_cast<int>(baseVolume * (1.0 + 0.5 * level)));
        edge->p1 = edgeParamJitter(2.0, 0.3);
        edge->p2 = edgeParamJitter(0.625, 0.05);
        break;
    case 1:
        edge->volume = std::max(1, static_cast<int>(baseVolume * (2.0 + 1.0 * level)));
        edge->p1 = edgeParamJitter(1.05, 0.15);
        edge->p2 = edgeParamJitter(0.775, 0.05);
        break;
    case 2:
        edge->volume = std::max(1, static_cast<int>(baseVolume * (4.0 + 1.5 * level)));
        edge->p1 = edgeParamJitter(0.55, 0.08);
        edge->p2 = edgeParamJitter(0.85, 0.03);
        break;
    case 3:
        edge->volume = std::max(1, static_cast<int>(baseVolume * (2.0 + 0.8 * level)));
        edge->p1 = edgeParamJitter(1.2, 0.18);
        edge->p2 = edgeParamJitter(0.725, 0.05);
        break;
    }

    Edge *revEdge = new Edge();

    revEdge->from = v;
    revEdge->to = u;
    revEdge->volume = edge->volume;
    revEdge->p1 = edge->p1;
    revEdge->p2 = edge->p2;
    revEdge->length = edge->length;
    revEdge->name = "E" + std::to_string(edgeIndex + 1);

    
    u->edges.emplace_back(edge);
    v->edges.emplace_back(revEdge);
    graph.second.insert(edge);
    graph.second.insert(revEdge);
    return true;
}

void addCandidate(
    std::vector<CandidateEdge> &candidates,
    std::unordered_set<unsigned long long> &candidateKeys,
    const std::vector<Node*> &nodes,
    int from,
    int to,
    int level,
    int priority
) {
    if (from == to || from < 0 || to < 0 ||
        from >= static_cast<int>(nodes.size()) ||
        to >= static_cast<int>(nodes.size())) {
        return;
    }

    unsigned long long key = edgeKey(from, to);
    if (candidateKeys.find(key) != candidateKeys.end()) {
        return;
    }

    candidateKeys.insert(key);
    CandidateEdge candidate;
    candidate.edge.from = nodes[from];
    candidate.edge.to = nodes[to];
    candidate.edge.length = distance(nodes[from], nodes[to]);
    candidate.fromIndex = from;
    candidate.toIndex = to;
    candidate.level = level;
    candidate.priority = priority;
    candidates.push_back(candidate);
}

bool isGabrielEdge(
    const std::vector<Node*> &nodes,
    const NodeSpatialGrid &grid,
    int from,
    int to
) {
    const Node *u = nodes[from];
    const Node *v = nodes[to];
    double midX = (u->x + v->x) * 0.5;
    double midY = (u->y + v->y) * 0.5;
    double radiusSquared = distanceSquared(u, v) * 0.25;
    double radius = std::sqrt(radiusSquared);

    // Gabriel 空圆条件：以 uv 为直径的圆内不能存在第三个点。
    // 查询仍通过点空间网格限定在局部半径内，避免全量节点扫描。
    return grid.visitNearby(midX, midY, radius + 1e-7, [&](int nodeIndex) {
        if (nodeIndex == from || nodeIndex == to) {
            return true;
        }
        const Node *other = nodes[nodeIndex];
        return distanceSquared(other->x, other->y, midX, midY) >= radiusSquared - 1e-7;
    });
}

std::vector<CandidateEdge> buildGabrielCandidates(
    const std::vector<Node*> &nodes,
    const NodeSpatialGrid &grid,
    double searchRadius
) {
    std::vector<CandidateEdge> candidates;
    std::unordered_set<unsigned long long> candidateKeys;
    int nodeCount = static_cast<int>(nodes.size());
    candidates.reserve(nodeCount * 4);
    candidateKeys.reserve(nodeCount * 8);

    // 先用局部近邻生成候选，再用 Gabriel 空圆条件保留平面图候选边。
    for (int i = 0; i < nodeCount; ++i) {
        const Node *node = nodes[i];
        grid.visitNearby(node->x, node->y, searchRadius, [&](int otherIndex) {
            if (otherIndex <= i) {
                return true;
            }
            if (distanceSquared(nodes[i], nodes[otherIndex]) > searchRadius * searchRadius) {
                return true;
            }
            if (!isGabrielEdge(nodes, grid, i, otherIndex)) {
                return true;
            }

            int level = commonAddressLevel(nodes[i], nodes[otherIndex]);
            int priority = level > 0 ? 1 : 4;
            if (representativeLevel(nodes[i]) > 0 && representativeLevel(nodes[otherIndex]) > 0) {
                priority = std::max(0, 3 - level);
            }
            addCandidate(candidates, candidateKeys, nodes, i, otherIndex, level, priority);
            return true;
        });
    }

    return candidates;
}

std::vector<CandidateEdge> buildLocalCandidates(
    const std::vector<Node*> &nodes,
    const NodeSpatialGrid &grid,
    double searchRadius,
    const std::unordered_set<unsigned long long> &usedEdges
) {
    std::vector<CandidateEdge> candidates;
    std::unordered_set<unsigned long long> candidateKeys;
    int nodeCount = static_cast<int>(nodes.size());
    candidates.reserve(nodeCount * 6);
    candidateKeys.reserve(nodeCount * 12);

    // Gabriel 图较稀疏时，用局部短边补足目标边数，仍然不枚举全量点对。
    for (int i = 0; i < nodeCount; ++i) {
        const Node *node = nodes[i];
        grid.visitNearby(node->x, node->y, searchRadius, [&](int otherIndex) {
            if (otherIndex <= i) {
                return true;
            }
            unsigned long long key = edgeKey(i, otherIndex);
            if (usedEdges.find(key) != usedEdges.end()) {
                return true;
            }
            if (distanceSquared(nodes[i], nodes[otherIndex]) > searchRadius * searchRadius) {
                return true;
            }

            int level = commonAddressLevel(nodes[i], nodes[otherIndex]);
            int priority = level > 0 ? 2 : 5;
            if (representativeLevel(nodes[i]) > 0 && representativeLevel(nodes[otherIndex]) > 0) {
                priority = std::max(1, 4 - level);
            }
            addCandidate(candidates, candidateKeys, nodes, i, otherIndex, level, priority);
            return true;
        });
    }

    return candidates;
}

bool candidatesConnectAllNodes(std::vector<CandidateEdge> candidates, int nodeCount) {
    DisjointSet dsu(nodeCount);
    std::sort(candidates.begin(), candidates.end(), [](const CandidateEdge &lhs, const CandidateEdge &rhs) {
        return lhs.edge.length < rhs.edge.length;
    });
    for (const CandidateEdge &candidate : candidates) {
        dsu.unite(candidate.fromIndex, candidate.toIndex);
        if (dsu.components() == 1) {
            return true;
        }
    }
    return nodeCount <= 1 || dsu.components() == 1;
}

void addKruskalConnectivityEdges(
    Graph &graph,
    std::vector<Node*> &nodes,
    const std::vector<CandidateEdge> &candidates,
    std::unordered_set<unsigned long long> &usedEdges,
    EdgeSpatialIndex &edgeIndex,
    int edgeTarget,
    int baseVolume
) {
    std::vector<CandidateEdge> ordered = candidates;
    // 在 Gabriel 候选边上运行 Kruskal，替代全量 Prim，生成连通骨架。
    std::sort(ordered.begin(), ordered.end(), [](const CandidateEdge &lhs, const CandidateEdge &rhs) {
        return lhs.edge.length < rhs.edge.length;
    });

    DisjointSet dsu(static_cast<int>(nodes.size()));
    for (const CandidateEdge &candidate : ordered) {
        if (static_cast<int>(graph.second.size()) >= edgeTarget || dsu.components() == 1) {
            break;
        }
        if (!dsu.unite(candidate.fromIndex, candidate.toIndex)) {
            continue;
        }
        if (addGraphEdge(
            graph,
            nodes,
            candidate.fromIndex,
            candidate.toIndex,
            candidate.level,
            baseVolume,
            static_cast<int>(graph.second.size())
        )) {
            usedEdges.insert(edgeKey(candidate.fromIndex, candidate.toIndex));
            edgeIndex.add(candidate.fromIndex, candidate.toIndex);
        }
    }
}

void generateHierarchicalEdges(
    Graph &graph,
    std::vector<Node*> &nodes,
    int edgeTarget,
    int levelVolume,
    double baseRadius,
    double left,
    double right,
    double bottom,
    double top
) {
    if (nodes.size() <= 1 || edgeTarget <= 0) {
        return;
    }

    int nodeCount = static_cast<int>(nodes.size());
    int planarEdgeLimit = nodeCount < 3
        ? nodeCount * (nodeCount - 1) / 2
        : 3 * nodeCount - 6;
        
    planarEdgeLimit *= 2;

    edgeTarget = std::max(edgeTarget, static_cast<int>(nodes.size()) - 1);
    edgeTarget = std::min(edgeTarget, planarEdgeLimit);
    int baseVolume = std::max(1, levelVolume);
    std::unordered_set<unsigned long long> usedEdges;
    usedEdges.reserve(edgeTarget * 2);

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
    double diagonal = std::sqrt(width * width + height * height);
    double nominalRadius = std::max(baseRadius, std::sqrt(width * height / std::max(1, nodeCount)));
    NodeSpatialGrid pointGrid(left, right, bottom, top, std::max(nominalRadius, 1.0));
    // 标记边覆盖的网格
    EdgeSpatialIndex edgeIndex(
        nodes,
        left,
        right,
        bottom,
        top,
        std::max(nominalRadius * 3.0, 1.0)
    );

    for (int i = 0; i < nodeCount; ++i) {
        pointGrid.add(nodes[i], i);
    }

    std::vector<CandidateEdge> candidates;
    double searchRadius = nominalRadius * 3.0;
    // 若初始局部半径不足以连通，则逐步扩大半径并重建 Gabriel 候选集。
    for (int attempt = 0; attempt < 8; ++attempt) {
        candidates = buildGabrielCandidates(nodes, pointGrid, searchRadius);
        if (candidatesConnectAllNodes(candidates, nodeCount) ||
            searchRadius >= diagonal) {
            break;
        }
        searchRadius *= 1.8;
    }

    addKruskalConnectivityEdges(
        graph,
        nodes,
        candidates,
        usedEdges,
        edgeIndex,
        edgeTarget,
        baseVolume
    );
    if (static_cast<int>(graph.second.size()) >= edgeTarget) {
        return;
    }

    auto sortCandidates = [](std::vector<CandidateEdge> &items) {
        // 先按层级优先级补边，再偏向同层级更高、距离更短的候选边。
        std::sort(items.begin(), items.end(), [](const CandidateEdge &lhs, const CandidateEdge &rhs) {
            if (lhs.priority != rhs.priority) {
                return lhs.priority < rhs.priority;
            }
            if (lhs.level != rhs.level) {
                return lhs.level > rhs.level;
            }
            return lhs.edge.length < rhs.edge.length;
        });
    };

    sortCandidates(candidates);
    for (const CandidateEdge &candidate : candidates) {
        if (static_cast<int>(graph.second.size()) >= edgeTarget) {
            break;
        }
        unsigned long long key = edgeKey(candidate.fromIndex, candidate.toIndex);
        if (usedEdges.find(key) != usedEdges.end()) {
            continue;
        }
        if (addGraphEdge(
            graph,
            nodes,
            candidate.fromIndex,
            candidate.toIndex,
            candidate.level,
            baseVolume,
            static_cast<int>(graph.second.size())
        )) {
            usedEdges.insert(key);
            edgeIndex.add(candidate.fromIndex, candidate.toIndex);
        }
    }

    if (static_cast<int>(graph.second.size()) >= edgeTarget) {
        return;
    }

    std::vector<CandidateEdge> fillerCandidates = buildLocalCandidates(
        nodes,
        pointGrid,
        std::min(searchRadius * 1.4, diagonal),
        usedEdges
    );
    sortCandidates(fillerCandidates);

    for (const CandidateEdge &candidate : fillerCandidates) {
        if (static_cast<int>(graph.second.size()) >= edgeTarget) {
            break;
        }
        unsigned long long key = edgeKey(candidate.fromIndex, candidate.toIndex);
        if (usedEdges.find(key) != usedEdges.end()) {
            continue;
        }
        if (edgeIndex.wouldCross(candidate.fromIndex, candidate.toIndex)) {
            continue;
        }
        if (addGraphEdge(
            graph,
            nodes,
            candidate.fromIndex,
            candidate.toIndex,
            candidate.level,
            baseVolume,
            static_cast<int>(graph.second.size())
        )) {
            usedEdges.insert(key);
            edgeIndex.add(candidate.fromIndex, candidate.toIndex);
        }
    }
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
        for (Node *node : currentLevelNodes) {
            if (node == nullptr) {
                continue;
            }

            node->address[level] = findNearestCore(
                node, corePoints, nodeIds, coreAssignGrid, levelRadius
            );
        }

        // 为每个普通节点继承上一层核心点的本层归属
        for (Node *node : nodes) {
            if (node == nullptr) {
                continue;
            }

            int previousCore = node->address[level - 1];
            node->address[level] = nodes[previousCore]->address[level];
        }

        // 下一层只从当前层核心点中继续筛选
        currentLevelNodes = corePoints;
    }
}

DataMaker::DataMaker(
    double left, double right, double top, double bottom,
    int node_num, int edge_num, 
    int level_num, int level_volume
) : leftBound(left), rightBound(right), topBound(top), bottomBound(bottom)
{
    std::vector<Node*> nodes;
    double r = 0;

    // 生成基础泊松点集
    PoissonSampling sampling;
    sampling.generateNotes(left, right, top, bottom, nodes, node_num, r);

    // 在基础点集中选取多层核心点
    selectCorePoint(nodes, level_num, r, left, right, bottom, top);

    // 根据层级地址生成边：先保证连通，再补充局部/核心/跨区候选边
    generateHierarchicalEdges(
        this->graph,
        nodes,
        edge_num * 2,
        level_volume,
        r,
        left,
        right,
        bottom,
        top
    );

    // 将生成节点加入图中
    for (const Node *node : nodes) {
        this->graph.first.insert(node);
    }

    // initTrafficSimulator(edge_num * 10);

    initForFlow();
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

int DataMaker::queryCurrentFlowInEdge(const Edge *edge) {
    return 0;
}

void DataMaker::initForFlow()
{
    ;
}

PerlinNoise::PerlinNoise(int width, int height, double block_size, int seed)
{
    srand(seed);

    this->width = width;
    this->height = height;
    this->block_size = block_size;

    noiseMap.resize(width, std::vector<sf::Vector2f>(height, sf::Vector2f(0, 0)));

    for (int i = 0; i < width; ++i) {
        for (int j = 0; j < height; ++j) {
            noiseMap[i][j] = sf::Vector2f(2 * dis(gen) - 1, 2 * dis(gen) - 1);
        }
    }
}

double PerlinNoise::noise(double x, double y) const
{
    if (x < 0 || x >= width * block_size || y < 0 || y >= height * block_size) {
        return 0.0;
    }

    x /= block_size;
    y /= block_size;
    int xi = std::floor(x);
    int yi = std::floor(y);
    
    auto xrb = xi + 1;
    auto yrb = yi;

    auto xrt = xi + 1;
    auto yrt = yi + 1;

    auto xlt = xi;
    auto ylt = yi + 1;

    auto xlb = xi;
    auto ylb = yi;
    
    auto t = (x - xlb * block_size) / block_size;
    auto btm = lerp(
        sf::Vector2f(xlb * block_size, ylb * block_size), 
        sf::Vector2f(xrb * block_size, yrb * block_size), 
        noiseMap[xlb][ylb], noiseMap[xrb][ylb], lerp(t));    
    auto top = lerp(
        sf::Vector2f(xlt * block_size, ylt * block_size), 
        sf::Vector2f(xrt * block_size, yrt * block_size), 
        noiseMap[xlt][ylt], noiseMap[xrt][yrt], lerp(t));

    return lerp(btm, top, lerp((y - ylb * block_size) / block_size));
}

PerlinNoise::~PerlinNoise()
{
    ;
}

double PerlinNoise::lerp(sf::Vector2f a, sf::Vector2f b, sf::Vector2f ga, sf::Vector2f gb, double t) const
{
    auto v = b - a;
    auto m = sf::Vector2f(v.x * t, v.y * t);

    auto pro = [](sf::Vector2f a, sf::Vector2f b){ return a.x * b.x + a.y * b.y; };

    return pro(m - a, ga) * (1 - lerp(t)) + pro(m - b, gb) * lerp(t);
}

double PerlinNoise::lerp(double t) const
{
    return 3 * t * t + 2 * t * t * t;
}

double PerlinNoise::lerp(double a, double b, double t) const
{
    return a * (1 - lerp(t)) + b * lerp(t);
}

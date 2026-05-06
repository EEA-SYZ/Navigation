#ifndef __DATA_MAKER_HPP__
#define __DATA_MAKER_HPP__

/**
 * @brief 数据生成器
 */

#include <random>
#include <chrono>
#include <cmath>
#include <algorithm>
#include <vector>
#include "global.hpp"

class DataMaker {
public:
    /**
     * @brief 生成图
     * @param left 图的左边界
     * @param right 图的右边界
     * @param top 图的上边界
     * @param bottom 图的下边界
     * @param node_num 图的节点数量
     * @param edge_num 图的边数量
     * @param level_num 图的层数量
     * @param level_volume 图的层流量容量
     * @return Graph 图的节点和边
     */
    DataMaker(
        double left, double right, double top, double bottom,
        int node_num, int edge_num, 
        int level_num = 5, int level_volume = 50
        // 可以继续加其他参数
    );
    ~DataMaker();
    /**
     * @brief 获取图
     * @return Graph 图的节点和边
     */
    const Graph &getGraph();
    /**
     * @brief 查询边的当前流量
     * @param edge 边
     * @return 边的当前流量
     */
    int queryCurrentFlowInEdge(Edge *edge);

private:
    Graph graph;
};

/**
 * @brief 空间网格，用于按半径快速查询附近节点
 */
class NodeSpatialGrid {
public:
    /**
     * @param left, right, bottom, top 网格覆盖的边界范围
     * @param cell_size 网格单元边长
     */
    NodeSpatialGrid(
        double left, double right, double bottom, double top, double cell_size
    ) {
        // 统一边界方向，避免调用方传入反向边界
        leftBound = std::min(left, right);
        rightBound = std::max(left, right);
        bottomBound = std::min(bottom, top);
        topBound = std::max(bottom, top);
        cellSize = std::max(cell_size, 1e-9);

        // 初始化网格行列数
        double width = std::max(rightBound - leftBound, cellSize);
        double height = std::max(topBound - bottomBound, cellSize);
        cols = std::max(1, static_cast<int>(std::ceil(width / cellSize)));
        rows = std::max(1, static_cast<int>(std::ceil(height / cellSize)));
        buckets.assign(cols * rows, {});
    }

    /**
     * @brief 将节点编号加入对应网格
     */
    void add(const Node *node, int node_index) {
        buckets[cellIndex(node->x, node->y)].emplace_back(node_index);
    }

    /**
     * @brief 遍历给定半径附近网格中的节点
     */
    template <class Visitor>
    bool visitNearby(double x, double y, double radius, Visitor visitor) const {
        int cell_x = cellX(x);
        int cell_y = cellY(y);
        int range = std::max(1, static_cast<int>(std::ceil(radius / cellSize)));

        // 只访问半径覆盖到的邻近网格
        for (int i = std::max(0, cell_x - range); i <= std::min(cols - 1, cell_x + range); ++i) {
            for (int j = std::max(0, cell_y - range); j <= std::min(rows - 1, cell_y + range); ++j) {
                for (int nodeIndex : buckets[j * cols + i]) {
                    if (!visitor(nodeIndex)) {
                        return false;
                    }
                }
            }
        }

        return true;
    }

private:
    double leftBound;
    double rightBound;
    double bottomBound;
    double topBound;
    double cellSize;
    int cols;
    int rows;
    std::vector<std::vector<int>> buckets;

    /**
     * @brief 计算 x 坐标所在列
     */
    int cellX(double x) const {
        int ret = static_cast<int>((x - leftBound) / cellSize);
        return std::clamp(ret, 0, cols - 1);
    }

    /**
     * @brief 计算 y 坐标所在行
     */
    int cellY(double y) const {
        int ret = static_cast<int>((y - bottomBound) / cellSize);
        return std::clamp(ret, 0, rows - 1);
    }

    /**
     * @brief 计算坐标对应的一维网格下标
     */
    int cellIndex(double x, double y) const {
        return cellY(y) * cols + cellX(x);
    }
};
 
/**
 * @brief 泊松磁盘采样，生成均匀不规则点集
 */
class PoissonSampling {
public:
    /**
     * @param left, right, top, bottom 边界范围
     * @param r 点间最小距离(为保证生成点的数量，传入默认的0即可)
     * @param k 每个点探测次数
     * @param n 生成的点数量
     */
    void generateNotes(
        double left, double right, double top, double bottom,
        std::vector<Node*> &nodes,
        int n, double &r, int k = 30
    ) {
        if (left > right) {
            std::swap(left, right);
        }
        if (bottom > top) {
            std::swap(bottom, top);
        }

        double width = right - left;
        double lenth = top - bottom;
        if (n <= 0 || width <= 0 || lenth <= 0) {
            return;
        }

        double area = width * lenth;
        if (!r) {
            r = std::sqrt(area * .45 / n);
        }

        // 全域候选采样，避免单种子前沿扩展在达到目标数量时留下边缘空洞。
        double nominalRadius = std::max(r, 1e-9);
        double relax = 1.0;
        double minRadius = nominalRadius * 0.72;
        double maxRadius = nominalRadius * 1.08;
        double cellSize = minRadius / std::sqrt(2);
        NodeSpatialGrid grid(left, right, bottom, top, cellSize);
        std::vector<double> nodeRadii;
        nodeRadii.reserve(n);
    
        // 随机数生成器
        std::mt19937 rnd(std::chrono::steady_clock::now().time_since_epoch().count());
        std::uniform_real_distribution<double> disx(left, right);
        std::uniform_real_distribution<double> disy(bottom, top);
        std::uniform_real_distribution<double> radiusJitter(0.82, 1.12);

        int targetCount = n;
        int failedAttempts = 0;
        int maxFailedAttempts = std::max(2000, targetCount * std::max(80, k * 4));
        while (static_cast<int>(nodes.size()) < targetCount) {
            double x = disx(rnd);
            double y = disy(rnd);
            double candidateRadius = nominalRadius * relax * radiusJitter(rnd);
            double boundaryDistance = std::min({
                x - left,
                right - x,
                y - bottom,
                top - y
            });
            double boundaryBlend = std::clamp(boundaryDistance / nominalRadius, 0.0, 1.0);
            candidateRadius *= 0.80 + 0.20 * boundaryBlend;
            candidateRadius = std::clamp(candidateRadius, minRadius * relax, maxRadius * relax);

            if (isValid(
                x, y, left, right, top, bottom,
                maxRadius * relax, candidateRadius, grid, nodes, nodeRadii
            )) {
                addNode(
                    createNode("N" + std::to_string(nodes.size()), x, y),
                    candidateRadius, nodes, nodeRadii, grid
                );
                failedAttempts = 0;
                continue;
            }

            ++failedAttempts;
            if (failedAttempts >= maxFailedAttempts) {
                relax *= 0.94;
                minRadius *= 0.94;
                maxRadius *= 0.94;
                failedAttempts = 0;
            }
        }
    }

private:

    Node* createNode(std::string &&s, double x, double y) {
        Node* n = new Node();
        n->name = s;
        n->x = x;
        n->y = y;

        return n;
    }

    void addNode(
        Node* n, double radius, std::vector<Node*> &nodes,
        std::vector<double> &nodeRadii, NodeSpatialGrid &grid
    ) {
        int nodeIndex = static_cast<int>(nodes.size());
        grid.add(n, nodeIndex);
        nodeRadii.emplace_back(radius);
        nodes.emplace_back(n);
    }

    bool isValid(
        double x, double y,
        double left, double right, double top, double bottom,
        double queryRadius, double candidateRadius,
        NodeSpatialGrid &grid, std::vector<Node*> &nodes,
        const std::vector<double> &nodeRadii
    ){
        if (x > right || x < left || y > top || y < bottom) {
            return false;
        }

        // 检查候选点附近是否已经有距离过近的点
        return grid.visitNearby(x, y, queryRadius, [&](int nodeIndex) {
            Node *other = nodes[nodeIndex];
            double radius = std::min(candidateRadius, nodeRadii[nodeIndex]);
            double radiusSquared = radius * radius;
            double dis = (x - other->x) * (x - other->x) + (y - other->y) * (y - other->y);
            if (dis < radiusSquared) {
                return false;
            }
            return true;
        });
    }
};

#endif

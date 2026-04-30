#ifndef __DATA_MAKER_HPP__
#define __DATA_MAKER_HPP__

/**
 * @brief 数据生成器
 */

#include <random>
#include <chrono>
#include <cmath>
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
    std::vector<Node> &generateNotes(
        double left, double right, double top, double bottom,
        int n, double r = 0, int k = 30
    ) {
        double width = std::abs(right - left);
        double lenth = std::abs(top - bottom);
        long long area = width * lenth;
        if (!r) {
            r = std::sqrt(area * .7 / n); 
        }

        // 初始化网格
        double cellSize = r / std::sqrt(2);
        int cols = width / cellSize;
        int rows = lenth / cellSize;
        
        std::vector<int> grid(cols * rows, -1); // 网格，存储节点编号
        std::vector<Node*> nodes;
        std::vector<int> activeList;            // 待选点集
    
        // 随机数生成器
        std::mt19937 rnd(std::chrono::steady_clock::now().time_since_epoch().count());
        std::uniform_real_distribution<double> disx(top, bottom);
        std::uniform_real_distribution<double> disy(left, right);

        Node* first = createNode("N0", disx(rnd), disy(rnd));

        addNode(first, nodes, activeList, grid, top, left, cellSize, cols, n);    
    
        // 采样循环
        while (!activeList.empty() && n) {
            // 从active点集随机选取
            std::uniform_int_distribution<int> activeIdx(0, activeList.size() - 1);
            int idx = activeIdx(rnd);

            Node *parent = nodes[activeList[idx]];

            bool find = false;

            // 对选中的参考点作候选取样
            for (int i = 0; i < k; ++i) {
                Node *candidate = generateAround(parent, r, rnd);

                if (isValid(
                    candidate, cellSize, rows, cols,
                    left, right, top, bottom, r,
                    grid, nodes
                )) {
                    find = true;
                    candidate->name = "N" + std::to_string(nodes.size());
                    addNode(candidate, nodes, activeList, grid, left, bottom, cellSize, cols, n);
                    if (n <= 0) break;
                }
                else {
                    delete candidate;
                }
            }

            if (!find) {
                activeList.erase(activeList.begin() + idx);
            }
        }
    }

private:

    #define PI 3.14159265358979323846

    Node* createNode(std::string &&s, double x, double y) {
        Node* n = new Node();
        n->name = s;
        n->x = x;
        n->y = y;
    }

    void addNode(
        Node* n, std::vector<Node*> &nodes, std::vector<int> &active, std::vector<int> &grid, 
        double minx, double miny, double cellSize, int cols, int &_n
    ) {
        int x = static_cast<int>((n->x - minx) / cellSize);
        int y = static_cast<int>((n->y - miny) / cellSize);
    
        grid[cols * y + x] = nodes.size();
        active.emplace_back(nodes.size());
        nodes.emplace_back(n);
        _n--;
    }

    Node *generateAround(Node *p, double r, std::mt19937 &rnd) {
        // 随机选取候选点的半径与角度
        std::uniform_real_distribution<double> distRadius(r, 2 * r);
        std::uniform_real_distribution<double> distAngle(0, 2 * PI);

        double radius = distRadius(rnd);
        double angle = distAngle(rnd);

        return createNode("", p->x + radius * std::cos(angle), p->y + radius * std::sin(angle));
    }

    bool isValid(
        Node *p, double cellSize, int rows, int cols,
        double left, double right, double top, double bottom, double r,
        std::vector<int> &g, std::vector<Node*> &nodes
    ){
        if (p->x > right || p->x < left || p->y > top || p->y < bottom) {
            return false;
        }

        int y = static_cast<int>((p->y - bottom) / cellSize);
        int x = static_cast<int>((p->x - left) / cellSize);

        // 检查5*5的领域是否已经有点
        for (int i = std::max(0, x - 2); i <= std::min(cols - 1, x + 2); ++i) {
            for (int j = std::max(0, y - 2); j <= std::min(rows - 1, y + 2); ++y) {
                int gridIdx = j * cols + i;

                if (g[gridIdx] != -1) {
                    Node *other = nodes[g[gridIdx]];
                    double dis = (p->x - other->x) * (p->x - other->x) + (p->y - other->y) * (p->y - other->y);

                    if (dis < r * r) {
                        return false;
                    }
                }
            }
        }
        return true;
    }
};

#endif

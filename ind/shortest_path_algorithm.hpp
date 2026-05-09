#ifndef __SHORTEST_PATH_ALGORITHM_HPP__
#define __SHORTEST_PATH_ALGORITHM_HPP__

#include "global.hpp"
#include <unordered_map>
#include <queue>
#include <functional>
#include <algorithm>

class ShortestPathAlgorithm {
public:
    ShortestPathAlgorithm(const Graph &graph);
    ~ShortestPathAlgorithm();
    /**
     * @brief 查询节点路径（距离最短）
     * @param start 起始节点
     * @param end 结束节点
     * @return VPath 节点路径
     */
    VPath queryNodePath(const Node *start, const Node *end);
    /**
     * @brief 查询边路径（距离最短）
     * @param start 起始节点
     * @param end 结束节点
     * @return EPath 边路径
     */
    EPath queryEdgePath(const Node *start, const Node *end);
    /**
     * @brief 查询节点时间路径（时间最短）
     * @param start 起始节点
     * @param end 结束节点
     * @return VPath 节点时间路径
     */
    VPath queryNodeTimePath(const Node *start, const Node *end);
    /**
     * @brief 查询边时间路径（时间最短）
     * @param start 起始节点
     * @param end 结束节点
     * @return EPath 边时间路径
     */
    EPath queryEdgeTimePath(const Node *start, const Node *end);

    // 设置流量查询接口（用于获取边的当前流量）
    void setFlowQueryInterface(std::function<int(const Edge*)> flowQueryFunc) {
        m_flowQueryFunc = flowQueryFunc;
    }

    // 设置全局最小p1参数获取接口
    void setMinP1Getter(std::function<double()> minP1Getter) {
        m_minP1Getter = minP1Getter;
    }

private:
    const Graph *m_graph;
    // 邻接表：节点 -> 连接的边列表
    std::unordered_map<const Node*, std::vector<const Edge*>> m_adjacencyList;
    // 流量查询函数
    std::function<int(const Edge*)> m_flowQueryFunc;
    // 全局最小p1参数获取函数
    std::function<double()> m_minP1Getter;

    /**
     * @brief 获取时间启发值（从当前节点到目标节点的最小可能时间）
     * @param current 当前节点
     * @param end 目标节点
     * @return 启发值
     */
    double getTimeHeuristic(const Node *current, const Node *end) const;

    /**
     * @brief 计算两点之间的欧氏距离
     * @param a 节点a
     * @param b 节点b
     * @return 欧氏距离
     */
    double euclideanDistance(const Node *a, const Node *b) const;

    /**
     * @brief A*算法查找最短路径（距离模式）
     * @param start 起始节点
     * @param end 结束节点
     * @param cameFrom 输出参数，记录路径
     * @return 是否找到路径
     */
    bool aStarDistance(const Node *start, const Node *end,
                       std::unordered_map<const Node*, const Edge*> &cameFrom);

    /**
     * @brief A*算法查找最短路径（时间模式）
     * @param start 起始节点
     * @param end 结束节点
     * @param cameFrom 输出参数，记录路径
     * @return 是否找到路径
     */
    bool aStarTime(const Node *start, const Node *end,
                   std::unordered_map<const Node*, const Edge*> &cameFrom);

    /**
     * @brief 从cameFrom中提取节点路径
     * @param start 起始节点
     * @param end 结束节点
     * @param cameFrom 路径记录
     * @return 节点路径
     */
    VPath reconstructNodePath(const Node *start, const Node *end,
                              const std::unordered_map<const Node*, const Edge*> &cameFrom);

    /**
     * @brief 从cameFrom中提取边路径
     * @param start 起始节点
     * @param end 结束节点
     * @param cameFrom 路径记录
     * @return 边路径
     */
    EPath reconstructEdgePath(const Node *start, const Node *end,
                              const std::unordered_map<const Node*, const Edge*> &cameFrom);

    /**
     * @brief 获取边的当前流量
     * @param edge 边
     * @return 当前流量
     */
    int getCurrentFlow(const Edge *edge) const;

    /**
     * @brief 获取全局最小p1参数
     * @return 最小p1值
     */
    double getMinP1() const;
};

#endif
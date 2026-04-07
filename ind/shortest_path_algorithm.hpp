#ifndef __SHORTEST_PATH_ALGORITHM_HPP__
#define __SHORTEST_PATH_ALGORITHM_HPP__

#include "global.hpp"

class ShortestPathAlgorithm {
public:
    ShortestPathAlgorithm(const Graph &graph);
    ~ShortestPathAlgorithm();
    /**
     * @brief 查询节点路径
     * @param start 起始节点
     * @param end 结束节点
     * @return VPath 节点路径
     */
    VPath queryNodePath(const Node *start, const Node *end);
    /**
     * @brief 查询边路径
     * @param start 起始节点
     * @param end 结束节点
     * @return EPath 边路径
     */
    EPath queryEdgePath(const Node *start, const Node *end);
    /**
     * @brief 查询节点时间路径
     * @param start 起始节点
     * @param end 结束节点
     * @return VPath 节点时间路径
     */
    VPath queryNodeTimePath(const Node *start, const Node *end);
    /**
     * @brief 查询边时间路径
     * @param start 起始节点
     * @param end 结束节点
     * @return EPath 边时间路径
     */
    EPath queryEdgeTimePath(const Node *start, const Node *end);
};

#endif
 
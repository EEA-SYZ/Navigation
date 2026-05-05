#ifndef __GRAPH_VIEW_HPP__
#define __GRAPH_VIEW_HPP__

#include "global.hpp"

class GraphView {
public:
    GraphView(const Graph &graph);
    /**
     * @brief 根据层级处理原始图，返回该层级视图
     * @note  不使用原始图的原因是，地图缩放后，部分边会发生“聚合”，此时会出现原图不存在的边
     * @param level 筛选的层级
     */
    Graph getGraphViewByLevel(int level) const;

private:
    const Graph &basegraph;
};


#endif
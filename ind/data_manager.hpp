#ifndef __DATA_MANAGER_HPP__
#define __DATA_MANAGER_HPP__

#include "global.hpp"
#include <queue>
#include <unordered_set>
#include <climits>
#include <tuple>
#include <limits>
#include <algorithm>
#include <list>

using Distancecmp = std::pair<double,const Node*>;
using Cell=std::pair<int,int>;
//left,right,top,bottom
using Boundary=std::tuple<int,int,int,int>;
//using a=std::pair<Boundary,const Node*>;
struct NearestInfo
{
    Boundary bound;
    const Node* nearestNode;
    std::vector<const Node*> nearestNodes;
};


struct pairHash {
    std::size_t operator() (const std::pair<int, int> &pair) const {
        auto hash1 = std::hash<int>{}(pair.first);
        auto hash2 = std::hash<int>{}(pair.second);
        return hash1 ^ (hash2 << 1); // Combine the two hashes
    }
};
class DataManager {
public:
    DataManager(const Graph &graph);
    ~DataManager();
    /**
     * @brief 查询视口内的数据
     * @param left 视口的左边界
     * @param right 视口的右边界
     * @param top 视口的上边界
     * @param bottom 视口的下边界
     * @param level 查询的图的层级
     * @return Graph 视口内的节点和边
     * @note level=0
     */
    Graph queryDataInViewport(
        int left, int right, int top, int bottom, int level
    );

    
    /**
     * @brief 拿取指定坐标附近100个点的边界
     */
    Boundary getNearest100NodesBounds(double coorCol,double coorRow,int level);

    /**
     * @brief 查询指定坐标附近100个点及其相关联的边
     */
    Graph queryNearest100Subgraph(double coorCol,double coorRow,int level);

    #ifdef DATA_MANAGER_TEST
    //测试函数所需公有函数
    void printCellBucket() const
    {
        for(const auto& cell:cellBucket)
        {
            std::cout<<"Cell ("<<cell.first.first<<","<<cell.first.second<<"):"<<std::endl;
            for(const auto& node:cell.second)
            {
                std::cout<<"  Node "<<node->name<<" at ("<<node->x<<","<<node->y<<")"<<std::endl;
            }
        }
    }
    double getCellWidth() const { return cellWidth; }
    double getCellHeight() const { return cellHeight; }
    double getLeftBound() const { return leftBound; }
    double getRightBound() const { return rightBound; }
    double getTopBound() const { return topBound; }
    double getBottomBound() const { return bottomBound; }
    int getColNums() const { return colNums; }
    int getRowNums() const { return rowNums; }
    std::vector<const Node*> getNodesInViewPort(int left,int right,int top,int bottom,int level) const{
        return nodeInViewPort(left,right,top,bottom,level);
    }
    #endif

private:
    //网格大小
    double cellWidth;
    double cellHeight;
    //边界
    double leftBound;
    double rightBound;
    double topBound;
    double bottomBound;
    //网格行数列数
    int colNums;
    int rowNums;
    std::unordered_map<std::pair<int,int>, std::vector<const Node*>, pairHash> cellBucket;
    void initCellData(const Graph& graph);
    void initHash(const Graph& graph);
    /**
     * @brief 计算网格到中心点的距离
     */
    std::pair<double,Cell> cellCalculateDistance(int col,int row,double centerX,double centerY) const;
    /**
     * @brief 检查网格是否已访问
     */
    bool isCellVisited(int col,int row,const std::unordered_set<Cell, pairHash>& visitedCells) const;
    /**
     * @brief 统计节点数量
     */
    int countNode(const Graph& graph);
    /**
     * @brief 查询指定坐标附近100个点的综合信息
     */
    NearestInfo getNearestInfo(double coorCol,double coorRow,int level);
    /**
     * @brief 均匀网格哈希拿候选点
     */
    std::vector<const Node*> nodeInViewPort(int left,int right,int top,int bottom,int level) const;
};

#endif

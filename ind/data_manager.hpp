#ifndef __DATA_MANAGER_HPP__
#define __DATA_MANAGER_HPP__

#include "global.hpp"
#include <queue>
#include <iostream>
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
     * @brief 均匀网格哈希拿候选点
     * @param left 视口的左边界
     * @param right 视口的右边界
     * @param top 视口的上边界 
     * @param bottom 视口的下边界 
     * @param level 查询的图的层级 
     * @return std::list<const Node*> 
     */
    //todo 用上level
    std::vector<const Node*> nodeInViewPort(int left,int right,int top,int bottom,int level);

    /**
     * @brief 拿取指定坐标附近100个点的边界
     */
    Boundary getNearest100NodesBounds(int col,int row,int level);

    /**
     * @brief 
     * 
     */
    //todo 
    const Node* getNodeAt(int col,int row);

    /**
     * @brief 
     * 
     */
    NearestInfo getNearestInfo(int col,int row,int level);

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
    //行数列数
    int colNums;
    int rowNums;
    std::unordered_map<std::pair<int,int>, std::vector<const Node*>, pairHash> cellBucket;
    Graph graphManager;
    void initCellData(const Graph& graph);
    void initHash(const Graph& graph);
    std::pair<double,Cell> cellCalculateDistance(int col,int row,double centerX,double centerY) const;
    bool isCellVisited(int col,int row,const std::unordered_set<Cell, pairHash>& visitedCells) const;
    Node* nearestNode;
};

#endif

#include "data_manager.hpp"
#include<cmath>
#include<limits>
#include<algorithm>

DataManager::DataManager(const Graph& graph)
{
    graphManager=graph;
    initCellData(graph);
    initHash(graph);
}
void DataManager::initCellData(const Graph& graph)
{
    int nodeNum=graph.first.size();    
    //floor向下取整，ceil向上取整
    colNums=std::max(1,(int)std::floor(sqrt(nodeNum)));
    rowNums=std::max(1,(int)std::floor(sqrt(nodeNum)));
    //计算边界
    //处理空值
    if(graph.first.empty())
    {
        leftBound=0;
        rightBound=0;
        topBound=0;
        bottomBound=0;
        cellWidth=1;
        cellHeight=1;
        colNums=1;
        rowNums=1;
        return;
    }
    //初始化边界
    int flag=0;//防止graph.first全为nullptr
    leftBound=std::numeric_limits<double>::max();
    rightBound=std::numeric_limits<double>::lowest();
    topBound=std::numeric_limits<double>::lowest();
    bottomBound=std::numeric_limits<double>::max();
    for(const auto& node:graph.first)
    {
        if(node==nullptr) 
        {
            continue;
        }
        flag=1;
        leftBound=std::min(leftBound,node->x);
        rightBound=std::max(rightBound,node->x);
        topBound=std::max(topBound,node->y);
        bottomBound=std::min(bottomBound,node->y);
    }
    if(!flag)
    {
        leftBound=0;
        rightBound=0;
        topBound=0;
        bottomBound=0;
        colNums=1;
        rowNums=1;
        cellWidth=1;
        cellHeight=1;
        return;
    }
    //计算网格大小
    //处理错误情况
    if(leftBound == rightBound)
    {
        leftBound-=0.5;
        rightBound+=0.5;
        colNums=1;
    }
    cellWidth=(rightBound-leftBound)/colNums;
    if(bottomBound == topBound)
    {
        bottomBound-=0.5;
        topBound+=0.5;
        rowNums=1;
    }
    cellHeight=(topBound-bottomBound)/rowNums;
    
}
void DataManager::initHash(const Graph& graph)
{
    for(const auto& node:graph.first)
    {
        if(node==nullptr) 
        {
            continue;
        }
        int col=(int)((node->x-leftBound)/cellWidth);
        int row=(int)((node->y-bottomBound)/cellHeight);
        col=std::clamp(col,0,colNums-1);//防止越界
        row=std::clamp(row,0,rowNums-1);
        cellBucket[{col,row}].push_back(node);
    }
}

DataManager::~DataManager()
{
    cellBucket.clear();
}
#include "data_manager.hpp"
#include<cmath>
#include<limits>
#include<algorithm>

//todo code review
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
        int col=(int)std::floor((node->x-leftBound)/cellWidth);
        int row=(int)std::floor((node->y-bottomBound)/cellHeight);
        col=std::clamp(col,0,colNums-1);//防止越界
        row=std::clamp(row,0,rowNums-1);
        cellBucket[{col,row}].push_back(node);
    }
}

DataManager::~DataManager()
{
    cellBucket.clear();
}

//todo 降级成辅助测试函数
std::set<const Node*> DataManager::hashSearch(int left,int right,int top,int bottom,int level)
{
    level=0;
    std::set<const Node*> result;
    int xmin=std::min(left,right);
    int xmax=std::max(left,right);
    int ymin=std::min(bottom,top);
    int ymax=std::max(bottom,top);
    //可能有负数，不能int直接截断
    int leftCol=(int) std::floor((xmin-leftBound)/cellWidth);
    int rightCol=(int) std::floor((xmax-leftBound)/cellWidth);
    int topRow=(int) std::floor((ymax-bottomBound)/cellHeight);
    int bottomRow=(int) std::floor((ymin-bottomBound)/cellHeight);
    leftCol=std::clamp(leftCol,0,colNums-1);
    rightCol=std::clamp(rightCol,0,colNums-1);
    topRow=std::clamp(topRow,0,rowNums-1);
    bottomRow=std::clamp(bottomRow,0,rowNums-1);
    for(int col=leftCol;col<=rightCol;col++)
    {
        for(int row=bottomRow;row<=topRow;row++)
        {
            auto it=cellBucket.find({col,row});
            if(it!=cellBucket.end())
            {
                for(const auto& node:it->second)
                {
                    if(node->x>=xmin && node->x<=xmax && node->y>=ymin && node->y<=ymax)
                    {
                        result.insert(node);
                    }
                }
            }
        }
    }
    return result;
}

//todo 为什么没有传点进来
std::priority_queue<Distancecmp> DataManager::priorityQueueSearch(int left,int right,int top,int bottom,int level)
{
    level=0;
    std::priority_queue<Distancecmp> result;
    std::priority_queue<std::pair<double,Cell>,std::vector<std::pair<double,Cell>>,std::greater<std::pair<double,Cell>>> cellQueue;
    std::unordered_set<Cell, pairHash> visitedCells;
    std::unordered_set<const Node*> visitedNodes;
    double centerX=(left+right)/2.0;
    double centerY=(top+bottom)/2.0;
    int centerCol=(int) std::floor((centerX-leftBound)/cellWidth);
    int centerRow=(int) std::floor((centerY-bottomBound)/cellHeight);
    centerCol=std::clamp(centerCol,0,colNums-1);
    centerRow=std::clamp(centerRow,0,rowNums-1);
    cellQueue.push(cellCalculateDistance(centerCol,centerRow,centerX,centerY));
    visitedCells.insert({centerCol,centerRow});

    auto tryPushCell = [&](int nextCol, int nextRow)
    {
        if (nextCol < 0 || nextCol >= colNums || nextRow < 0 || nextRow >= rowNums)
        {
            return;
        }
        if (isCellVisited(nextCol, nextRow, visitedCells))
        {
            return;
        }
        cellQueue.push(cellCalculateDistance(nextCol, nextRow, centerX, centerY));
        visitedCells.insert({nextCol, nextRow});
    };

    while (!cellQueue.empty())
    {
        auto [distance, cell] = cellQueue.top();
        auto [col,row] = cell;
        cellQueue.pop();

        if (result.size() >= 100 && distance >= result.top().first)
        {
            break;
        }

        auto it = cellBucket.find(cell);
        if (it != cellBucket.end())
        {
            for (const auto& node : it->second)
            {
                if (node == nullptr || visitedNodes.find(node) != visitedNodes.end())
                {
                    continue;
                }

                visitedNodes.insert(node);
                double nodeDistance = std::hypot(node->x-centerX,node->y-centerY);
                result.push({nodeDistance,node});
                if (result.size() > 100)
                {
                    result.pop();
                }
            }
        }

        tryPushCell(col,row+1);
        tryPushCell(col,row-1);
        tryPushCell(col+1,row);
        tryPushCell(col-1,row);
    }
    
    return result;
}

Graph DataManager::queryDataInViewport(int left, int right, int top, int bottom, int level)
{
    level=0;
    std::set<const Node*> nodeSet;
    std::priority_queue<Distancecmp> topNodes=priorityQueueSearch(left,right,top,bottom,level);
    while (!topNodes.empty())
    {
        const Node* node=topNodes.top().second;
        topNodes.pop();
        nodeSet.insert(node);
    }
    std::set<const Edge*> edgeSet;
    for(const auto& node:nodeSet)
    {
        //跳过nullptr
        if (node == nullptr)
        {
            continue;
        }
        for(const auto& edge:node->edges)
        {
            if (edge == nullptr || edge->from == nullptr || edge->to == nullptr)
            {
                continue;
            }
            if(nodeSet.find(edge->from) != nodeSet.end() && nodeSet.find(edge->to) != nodeSet.end())
            {
                edgeSet.insert(edge);
            }
        }
    }
    return {nodeSet,edgeSet};
}

std::pair<double,Cell> DataManager::cellCalculateDistance(int col,int row,double centerX,double centerY) const
{
    double cellLeft = leftBound + col * cellWidth;
    double cellRight = (col == colNums - 1) ? rightBound : cellLeft + cellWidth;
    double cellBottom = bottomBound + row * cellHeight;
    double cellTop = (row == rowNums - 1) ? topBound : cellBottom + cellHeight;

    double dx = 0.0;
    if (centerX < cellLeft)
    {
        dx = cellLeft - centerX;
    }
    else if (centerX > cellRight)
    {
        dx = centerX - cellRight;
    }

    double dy = 0.0;
    if (centerY < cellBottom)
    {
        dy = cellBottom - centerY;
    }
    else if (centerY > cellTop)
    {
        dy = centerY - cellTop;
    }

    return {std::hypot(dx, dy), {col, row}};
}

bool DataManager::isCellVisited(int col,int row,const std::unordered_set<Cell, pairHash>& visitedCells) const
{
    return visitedCells.find({col,row})!=visitedCells.end();
}

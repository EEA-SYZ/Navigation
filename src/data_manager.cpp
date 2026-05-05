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

std::set<const Node*> DataManager::hashSearch(int left,int right,int top,int bottom,int level)
{
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
    std::priority_queue<Distancecmp> result;
    int centerX=(left+right)/2;
    int centerY=(top+bottom)/2;
    std::unordered_set<const Node*> uniqueSet;
    const int MAX_SIZE=100;
    while(result.size() != 100 && left >= leftBound && right <= rightBound && top <= topBound && bottom >= bottomBound)
    {
        std::set<const Node*> nodeSet=hashSearch(left,right,top,bottom,level);
        for(const auto& node:nodeSet)
        {
            if(uniqueSet.find(node) != uniqueSet.end())
            {
                continue;
            }
            uniqueSet.insert(node);
            double distance=std::sqrt(std::pow(node->x-centerX,2)+std::pow(node->y-centerY,2));
            result.push({distance,node});
            //100 后续可能不会写死
            if(result.size()>MAX_SIZE)
            {
                result.pop();
            }
        }
        left-=MAX_SIZE/2;
        right+=MAX_SIZE/2;
        top+=MAX_SIZE/2;
        bottom-=MAX_SIZE/2;
    }
    return result;
}

Graph DataManager::GraphqueryDataInViewport(int left, int right, int top, int bottom, int level)
{
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
        for(const auto& edge:node->edges)
        {
            if(nodeSet.find(edge->from) != nodeSet.end() && nodeSet.find(edge->to) != nodeSet.end())
            {
                edgeSet.insert(edge);
            }
        }
    }
    return {nodeSet,edgeSet};
}
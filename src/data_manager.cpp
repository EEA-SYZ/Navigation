#include "data_manager.hpp"

DataManager::DataManager(const Graph& graph)
{
    initCellData(graph);
    initHash(graph);
}

void DataManager::initCellData(const Graph& graph)
{
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
    }
    if(bottomBound == topBound)
    {
        bottomBound-=0.5;
        topBound+=0.5;
    }
    double width=rightBound-leftBound;
    double hight=topBound-bottomBound;
    double r=std::sqrt((width*hight*0.45)/this->countNode(graph));
    //理论网格大小
    double cellSize=r*1.5;
    rowNums=std::max(1,(int)std::ceil(hight/cellSize));
    colNums=std::max(1,(int)std::ceil(width/cellSize));
    cellWidth=width/colNums;
    cellHeight=hight/rowNums;
    return;
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

std::vector<const Node*> DataManager::nodeInViewPort(int left,int right,int top,int bottom,int level)
{
    level=0;
    std::vector<const Node*> nodeIn;
    int xmin=std::min(left,right);
    int xmax=std::max(left,right);
    int ymin=std::min(bottom,top);
    int ymax=std::max(bottom,top);
    //可能有负数，不能int直接截断,[i,i+1)
    //视口对应的网格行列
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
                    //判断是否在视口内，细过滤
                    if(node->x>=xmin && node->x<=xmax && node->y>=ymin && node->y<=ymax)
                    {
                        nodeIn.push_back(node);
                    }
                }
            }
        }
    }
    return nodeIn;
}

Graph DataManager::queryDataInViewport(int left, int right, int top, int bottom, int level)
{
    std::vector<const Node*> nodeList=nodeInViewPort(left,right,top,bottom,0);
    std::unordered_set<const Node*> copyNodeSet;
    //{address[level],node}
    std::unordered_map<int,const Node*> nodeByGroup;
    //{address[level],distance}
    std::unordered_map<int,double> distanceByGroup;

    double centerX=(left+right)/2.0;
    double centerY=(top+bottom)/2.0;

    //data_maker保证每个点的address层级一致
    if(!nodeList.empty() && level >= nodeList.front()->address.size())
    {
        level=0;
    }
    if(level <= 0)
    {
        copyNodeSet.insert(nodeList.begin(),nodeList.end());
    }
    else{
        
        for(auto node:nodeList)
        {
            //跳过空节点
            if(node == nullptr)
            {
                continue;
            }
            int currentAddress=node->address[level];
            //1.是否是代表点,是则加入
            bool isRepresent=currentAddress == node->address[0];
            if(isRepresent)
            {
                nodeByGroup[currentAddress]=node;
                distanceByGroup[currentAddress]=0;
                continue;
            }
            double distance=std::hypot(node->x-centerX,node->y-centerY);
            //2.如果不是代表点且该组没有点被选中，则加入
            if(nodeByGroup.find(currentAddress) == nodeByGroup.end())
            {
                nodeByGroup[currentAddress]=node;
                distanceByGroup[currentAddress]=distance;
                continue;
            }
            //3.如果该组已经有点,则取距离小的
            if(distance < distanceByGroup[currentAddress])
            {
                nodeByGroup[currentAddress]=node;
                distanceByGroup[currentAddress]=distance;
                continue;
            }
            //4.距离一样，统一语义：选address[0]小的
            if (std::abs(distance - distanceByGroup[currentAddress]) < 1e-9 && node->address[0] < nodeByGroup[currentAddress]->address[0])
            {
                nodeByGroup[currentAddress] = node;
                distanceByGroup[currentAddress] = distance;
            }
        }
        for(auto node:nodeByGroup)
        {
            copyNodeSet.insert(node.second);
        }
    }
    std::set<const Node*> nodeSet(copyNodeSet.begin(),copyNodeSet.end());
    std::set<const Edge*> edgeSet;
    for(const auto& node:copyNodeSet)
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
            if(copyNodeSet.find(edge->from) != copyNodeSet.end() && copyNodeSet.find(edge->to) != copyNodeSet.end())
            {
                edgeSet.insert(edge);
            }
        }
    }
    return {nodeSet,edgeSet};
}


NearestInfo DataManager::getNearestInfo(double col,double row,int level)
{
    level=0;
    std::priority_queue<Distancecmp> nearestNode;
    std::priority_queue<std::pair<double,Cell>,std::vector<std::pair<double,Cell>>,std::greater<std::pair<double,Cell>>> cellQueue;
    std::unordered_set<Cell, pairHash> visitedCells;
    std::unordered_set<const Node*> visitedNodes;
    int cellCol=(int) std::floor((col-leftBound)/cellWidth);
    int cellRow=(int) std::floor((row-bottomBound)/cellHeight);
    cellCol=std::clamp(cellCol,0,colNums-1);
    cellRow=std::clamp(cellRow,0,rowNums-1);
    cellQueue.push(cellCalculateDistance(cellCol,cellRow,col,row));
    visitedCells.insert({cellCol,cellRow});

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
        cellQueue.push(cellCalculateDistance(nextCol, nextRow, col, row));
        visitedCells.insert({nextCol, nextRow});
    };

    while (!cellQueue.empty())
    {
        auto [distance, cell] = cellQueue.top();
        auto [nodeCol,nodeRow] = cell;
        cellQueue.pop();

        if (nearestNode.size() >= 100 && distance >= nearestNode.top().first)
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
                double nodeDistance = std::hypot(node->x-col,node->y-row);
                nearestNode.push({nodeDistance,node});
                if (nearestNode.size() > 100)
                {
                    nearestNode.pop();
                }
            }
        }

        tryPushCell(nodeCol,nodeRow+1);
        tryPushCell(nodeCol,nodeRow-1);
        tryPushCell(nodeCol+1,nodeRow);
        tryPushCell(nodeCol-1,nodeRow);
    }
    NearestInfo nearestInfo;
    if (nearestNode.empty())
    {
        nearestInfo.bound={0,0,0,0};
        nearestInfo.nearestNode=nullptr;
        return nearestInfo;
    }

    double minX=std::numeric_limits<double>::max();
    double maxX=std::numeric_limits<double>::lowest();
    double minY=std::numeric_limits<double>::max();
    double maxY=std::numeric_limits<double>::lowest();

    while (!nearestNode.empty())
    {
        auto node=nearestNode.top();
        if(nearestNode.size()==1)
        {
            nearestInfo.nearestNode=node.second;
        }
        nearestNode.pop();
        minX=std::min(node.second->x,minX);
        maxX=std::max(node.second->x,maxX);
        minY=std::min(node.second->y,minY);
        maxY=std::max(node.second->y,maxY);
    }

    int left=(int)std::floor(minX);
    int right=(int)std::ceil(maxX);
    int top=(int)std::ceil(maxY);
    int bottom=(int)std::floor(minY);
    nearestInfo.bound={left,right,top,bottom};
    return nearestInfo;
}

Boundary DataManager::getNearest100NodesBounds(double col,double row,int level)
{
    return getNearestInfo(col,row,0).bound;
}


const Node* DataManager::getNodeAt(double col,double row)
{
    return getNearestInfo(col,row,0).nearestNode;
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


int DataManager::countNode(const Graph& graph)
{
    int count=0;
    for(auto node:graph.first)
    {
        if(node != nullptr)
        {
            count++;
        }
    }
    return count;
}
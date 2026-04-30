#define DATA_MANAGER_TEST
#include "data_manager.hpp"
#include <cassert>

const bool LAB_TEST=true;

/** 
* @brief 测试构造函数
* @note 测试一般情况的图
*/
void normalConstructorTest();

/**
 * @brief 测试构造函数
 * @note 测试空图 
 */
void emptyConstructorTest();

/**
 * @brief 测试构造函数
 * @note 测试所有节点坐标相同的图 
 */
void sameConstructorTest();

/**
 * @brief 测试构造函数
 * @note 测试所有节点都为nullptr
 */
void nullConstructorTest();

/**
 * @brief 测试均匀网格哈希查找
 * @note 测试一般情况
 */
void hashSearchTest();

/**
 * @brief 测试均匀网格哈希查找
 * @note 反向边界
 */
void hashSearchReverseBoundaryTest();

/**
 * @brief 测试均匀网格哈希查找
 * @note 点在边界上
 */
void hashSearchOnBoundaryTest();

/**
 * @brief 测试均匀网格哈希查找
 * @note 点在边界外
 */
void hashSearchOutsideBoundaryTest();

/**
 * @brief 测试均匀网格哈希查找
 * @note 边界在图外
 */
void hashSearchOutsideGraphTest();

/** 
 * @brief 测试均匀网格哈希查找
 * @note 边界和图重叠
*/
void hashSearchOnGraphTest();

/**
 * @brief 测试均匀网格哈希查找
 * @note 边界重叠
 */
void hashSearchOverlapBoundaryTest();

/**
 * @brief 测试均匀网格哈希查找
 * @note 图中有nullptr节点
 */
void hashSearchNullNodeTest();

/**
 * @brief 测试 priorityQueueSearch 的基础 TopK 点初始化
 * @note 仅初始化点集，不补断言与查询逻辑
 */
void priorityQueueSearchBasicTopKTest();

/**
 * @brief 测试函数
 */
int main()
{
    if(LAB_TEST)
    {
       normalConstructorTest();
       emptyConstructorTest();
       sameConstructorTest();
       nullConstructorTest();
       hashSearchTest();
       hashSearchReverseBoundaryTest();
       hashSearchOnBoundaryTest();
       hashSearchOutsideBoundaryTest();
       hashSearchOutsideGraphTest();
       hashSearchOnGraphTest();
       hashSearchOverlapBoundaryTest();
       hashSearchNullNodeTest();
       priorityQueueSearchBasicTopKTest();
    }
}


void normalConstructorTest()
{
    std::set<const Node*> nodes={
        new Node{"A", {}, 0, 0, {0}},
        new Node{"B", {}, 1, 0, {1}},
        new Node{"C", {}, 0, 1, {2}},
        new Node{"D", {}, 1, 1, {3}}
    };
    Graph graph={nodes,{}};
    DataManager dataManager(graph);
    assert(dataManager.getColNums()==2);
    assert(dataManager.getRowNums()==2);
    assert(dataManager.getLeftBound()==0);
    assert(dataManager.getRightBound()==1);
    assert(dataManager.getTopBound()==1);
    assert(dataManager.getBottomBound()==0);
    assert(dataManager.getCellWidth()==0.5);
    assert(dataManager.getCellHeight()==0.5);
    dataManager.printCellBucket();
    for(auto it:nodes)
    {
        delete it;
    }
}

void emptyConstructorTest()
{
    Graph graph={std::set<const Node*>{}, std::set<const Edge*>{}};
    DataManager dataManager(graph);
    assert(dataManager.getColNums()==1);
    assert(dataManager.getRowNums()==1);
    assert(dataManager.getLeftBound()==0);
    assert(dataManager.getRightBound()==0);
    assert(dataManager.getTopBound()==0);
    assert(dataManager.getBottomBound()==0);
    assert(dataManager.getCellWidth()==1);
    assert(dataManager.getCellHeight()==1);
    dataManager.printCellBucket();
}

void sameConstructorTest()
{
    std::set<const Node*> nodes={
        new Node{"A", {}, 0, 0, {0}},
        new Node{"B", {}, 0, 0, {1}},
        new Node{"C", {}, 0, 0, {2}},
        new Node{"D", {}, 0, 0, {3}}
    };
    Graph graph={nodes,{}};
    DataManager dataManager(graph);
    assert(dataManager.getColNums()==1);
    assert(dataManager.getRowNums()==1);
    assert(dataManager.getLeftBound()==-0.5);
    assert(dataManager.getRightBound()==0.5);
    assert(dataManager.getTopBound()==0.5);
    assert(dataManager.getBottomBound()==-0.5);
    assert(dataManager.getCellWidth()==1);
    assert(dataManager.getCellHeight()==1);
    dataManager.printCellBucket();
    for(auto it:nodes)
    {
        delete it;
    }
}

void nullConstructorTest()
{
    std::set<const Node*> nodes={nullptr, nullptr, nullptr, nullptr};
    Graph graph={nodes, std::set<const Edge*>{}};
    DataManager dataManager(graph);
    assert(dataManager.getColNums()==1);
    assert(dataManager.getRowNums()==1);
    assert(dataManager.getLeftBound()==0);
    assert(dataManager.getRightBound()==0);
    assert(dataManager.getTopBound()==0);
    assert(dataManager.getBottomBound()==0);
    assert(dataManager.getCellWidth()==1);
    assert(dataManager.getCellHeight()==1);
    dataManager.printCellBucket();
}

/*
A: (0, 0),B: (2, 1),C: (5, 5)
D: (-2, 3),E: (3, -1),F: (10, 10)
*/
void hashSearchTest()
{
    std::set<const Node*> nodes={
        new Node{"A", {}, 0, 0, {0}},
        new Node{"B", {}, 2, 1, {1}},
        new Node{"C", {}, 5, 5, {2}},
        new Node{"D", {}, -2, 3, {3}},
        new Node{"E", {}, 3, -1, {4}},
        new Node{"F", {}, 10, 10, {5}}
    };
    Graph graph={nodes,{}};
    DataManager dataManager(graph);
    auto result=dataManager.hashSearch(-1,3,2,-1,0);
    assert(result.size()==3);
    for(auto it:result)
    {
        assert(it->name=="A" || it->name=="B" || it->name=="E");
    }
    for(auto it:nodes)
    {
        delete it;
    }
}

void hashSearchReverseBoundaryTest()
{
    std::set<const Node*> nodes={
        new Node{"A", {}, 0, 0, {0}},
        new Node{"B", {}, 2, 1, {1}},
        new Node{"C", {}, 5, 5, {2}},
        new Node{"D", {}, -2, 3, {3}},
        new Node{"E", {}, 3, -1, {4}},
        new Node{"F", {}, 10, 10, {5}}
    };
    Graph graph={nodes,{}};
    DataManager dataManager(graph);
    auto result=dataManager.hashSearch(3,-1,-1,2,0);
    assert(result.size()==3);
    for(auto it:result)
    {
        assert(it->name=="A" || it->name=="B" || it->name=="E");
    }
    for(auto it:nodes)
    {
        delete it;
    }
}

void hashSearchOnBoundaryTest()
{
    std::set<const Node*> nodes={
        new Node{"A", {}, 0, 0, {0}},
        new Node{"B", {}, 2, 1, {1}},
        new Node{"C", {}, 5, 5, {2}},
        new Node{"D", {}, -2, 3, {3}},
        new Node{"E", {}, 3, -1, {4}},
        new Node{"F", {}, 10, 10, {5}}
    };
    Graph graph={nodes,{}};
    DataManager dataManager(graph);
    auto result=dataManager.hashSearch(0,2,1,0,0);
    assert(result.size()==2);
    for(auto it:result)
    {
        assert(it->name=="A" || it->name=="B");
    }
    for(auto it:nodes)
    {
        delete it;
    }
}

void hashSearchOutsideBoundaryTest()
{
    std::set<const Node*> nodes={
        new Node{"A", {}, 0, 0, {0}},
        new Node{"B", {}, 2, 1, {1}},
        new Node{"C", {}, 5, 5, {2}},
        new Node{"D", {}, -2, 3, {3}},
        new Node{"E", {}, 3, -1, {4}},
        new Node{"F", {}, 10, 10, {5}}
    };
    Graph graph={nodes,{}};
    DataManager dataManager(graph);
    auto result=dataManager.hashSearch(6,7,7,6,0);
    assert(result.size()==0);
    for(auto it:nodes)
    {
        delete it;
    }
}

void hashSearchOutsideGraphTest()
{
    std::set<const Node*> nodes={
        new Node{"A", {}, 0, 0, {0}},
        new Node{"B", {}, 2, 1, {1}},
        new Node{"C", {}, 5, 5, {2}},
        new Node{"D", {}, -2, 3, {3}},
        new Node{"E", {}, 3, -1, {4}},
        new Node{"F", {}, 10, 10, {5}}
    };
    Graph graph={nodes,{}};
    DataManager dataManager(graph);
    auto result=dataManager.hashSearch(-100,1,1,-100,0);
    assert(result.size()==1);
    assert((*result.begin())->name=="A");
    for(auto it:nodes)
    {
        delete it;
    }
}

void hashSearchOnGraphTest()
{
    std::set<const Node*> nodes={
        new Node{"A", {}, 0, 0, {0}},
        new Node{"B", {}, 2, 1, {1}},
        new Node{"C", {}, 5, 5, {2}},
        new Node{"D", {}, -2, 3, {3}},
        new Node{"E", {}, 3, -1, {4}},
        new Node{"F", {}, 10, 10, {5}}
    };
    Graph graph={nodes,{}};
    DataManager dataManager(graph);
    auto result=dataManager.hashSearch(-100,0,3,-100,0);
    assert(result.size()==2);
    for(auto it:result)
    {
        assert(it->name=="A" || it->name=="D");
    }
    for(auto it:nodes)
    {
        delete it;
    }
}

void hashSearchOverlapBoundaryTest()
{
    std::set<const Node*> nodes={
        new Node{"A", {}, 0, 0, {0}},
        new Node{"B", {}, 2, 1, {1}},
        new Node{"C", {}, 5, 5, {2}},
        new Node{"D", {}, -2, 3, {3}},
        new Node{"E", {}, 3, -1, {4}},
        new Node{"F", {}, 10, 10, {5}}
    };
    Graph graph={nodes,{}};
    DataManager dataManager(graph);
    auto result=dataManager.hashSearch(10,10,10,10,0);
    assert(result.size()==1);
    assert((*result.begin())->name=="F");
    for(auto it:nodes)
    {
        delete it;
    }
}

void hashSearchNullNodeTest()
{
    std::set<const Node*> nodes={
        new Node{"A", {}, 0, 0, {0}},
        nullptr,
        new Node{"C", {}, 5, 5, {2}},
        new Node{"D", {}, -2, 3, {3}},
        nullptr,
        new Node{"F", {}, 10, 10, {5}}
    };
    Graph graph={nodes,{}};
    DataManager dataManager(graph);
    auto result=dataManager.hashSearch(-2,0,3,0,0);
    assert(result.size()==2);
    for(auto it:result)
    {
        assert(it->name=="A" || it->name=="D");
    }
    for(auto it:nodes)
    {
        delete it;
    }
}

void priorityQueueSearchBasicTopKTest()
{
    std::set<const Node*> nodes;
    for(int i=1;i<=101;i++)
    {
        std::string suffix=std::to_string(i);
        if(i<10)
        {
            suffix="00"+suffix;
        }
        else if(i<100)
        {
            suffix="0"+suffix;
        }
        nodes.insert(new Node{"K"+suffix, {}, 0.1*i, 0, {i}});
    }
    nodes.insert(new Node{"L", {}, -100, 0, {102}});
    nodes.insert(new Node{"R", {}, 100, 0, {103}});
    nodes.insert(new Node{"T", {}, 0, 100, {104}});
    nodes.insert(new Node{"B", {}, 0, -100, {105}});

    Graph graph={nodes,{}};
    DataManager dataManager(graph);
    (void)dataManager;

    for(auto it:nodes)
    {
        delete it;
    }
}

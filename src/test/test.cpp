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
 * @brief 测试GraphqueryDataInViewport函数
 * 
 */
void GraphqueryDataInViewportTest();

/**
 * @brief //TODO: 测试 level 兼容性
 * @note 同一查询条件下，level=0 与非0时返回结果应一致
 */
void GraphqueryDataInViewportLevelCompatibility();

/**
 * @brief 测试 GraphqueryDataInViewport 在节点多于100时返回恰好100个节点
 */
void GraphqueryDataInViewportExactly100Test();

/**
 * @brief 测试 GraphqueryDataInViewport 在反向/越界边界下结果一致
 */
void GraphqueryDataInViewportBoundaryConsistencyTest();

/**
 * @brief 测试 priorityQueueSearch 在节点不足100时不会卡住
 */
void priorityQueueSearchLessThan100Test();

/**
 * @brief 测试同中心不同视口时，priorityQueueSearch 结果一致
 */
void priorityQueueSearchSameCenterDifferentViewportTest();

/**
 * @brief 测试 GraphqueryDataInViewport 对空边的鲁棒性
 */
void GraphqueryDataInViewportNullEdgeTest();

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
       priorityQueueSearchLessThan100Test();
       priorityQueueSearchSameCenterDifferentViewportTest();
       GraphqueryDataInViewportTest();
       GraphqueryDataInViewportLevelCompatibility();
       GraphqueryDataInViewportExactly100Test();
       GraphqueryDataInViewportBoundaryConsistencyTest();
       GraphqueryDataInViewportNullEdgeTest();
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
    std::priority_queue<Distancecmp> top100List=dataManager.priorityQueueSearch(-20,20,20,-20,0);
    assert(top100List.size()==100);
    std::set<std::string> nodeName;
    while(!top100List.empty())
    {
        auto node=top100List.top();
        top100List.pop();
        nodeName.insert((node.second)->name);
    }
    assert(nodeName.find("K101") == nodeName.end());
    assert(nodeName.find("K102") == nodeName.end());
    assert(nodeName.find("K103") == nodeName.end());
    assert(nodeName.find("K104") == nodeName.end());
    assert(nodeName.find("K105") == nodeName.end());
    assert(nodeName.find("L") == nodeName.end());
    assert(nodeName.find("R") == nodeName.end());
    assert(nodeName.find("T") == nodeName.end());
    assert(nodeName.find("B") == nodeName.end());
    for(auto it:nodes)
    {
        delete it;
    }
}

void GraphqueryDataInViewportTest()
{
    std::set<Node*> nodes={
        new Node{"A", {}, 0, 0, {0}},
        new Node{"B", {}, 2, 0, {1}},
        new Node{"C", {}, 4, 0, {2}},
        new Node{"D", {}, 8, 0, {3}},
    };
    std::set<Edge*> edges={
        new Edge{"AB",nullptr,nullptr, 2.2360679775, 100, 1.0, 1.0},
        new Edge{"BC", nullptr, nullptr, 4.472135955, 100, 1.0, 1.0},
        new Edge{"CD", nullptr, nullptr,8.94427191 ,100 ,1.0 ,1.0},
        new Edge{"BD", nullptr, nullptr, 8.94427191, 100, 1.0, 1.0}
    };
    for(auto edge:edges)
    {
        for(auto node:nodes)
        {
            if(edge->name[0]==node->name[0])
            {
                edge->from=node;
                node->edges.push_back(edge);
            }
            if(edge->name[1]==node->name[0])
            {
                edge->to=node;
                node->edges.push_back(edge);
            }
        }
    }
    std::set<const Node*> constNodes(nodes.begin(), nodes.end());
    std::set<const Edge*> constEdges(edges.begin(), edges.end());
    Graph graph={constNodes,constEdges};
    DataManager dataManager(graph);
    auto result=dataManager.queryDataInViewport(0,4,0,0,0);
    assert(result.first.size()==4);
    assert(result.second.size()==4);
    for(auto it:result.first)   {
        assert(it->name=="A" || it->name=="B" || it->name=="C" || it->name=="D");
    }
    for(auto it:result.second)
    {
        assert(it->name=="AB" || it->name=="BC" || it->name=="CD" || it->name=="BD");
    }
    for(auto it:nodes)
    {
        delete it;
    }
}

void priorityQueueSearchLessThan100Test()
{
    std::set<const Node*> nodes={
        new Node{"A", {}, 0, 0, {0}},
        new Node{"B", {}, 1, 0, {1}},
        new Node{"C", {}, 0, 1, {2}},
        new Node{"D", {}, 1, 1, {3}}
    };
    Graph graph={nodes,{}};
    DataManager dataManager(graph);
    auto result=dataManager.priorityQueueSearch(100,120,120,100,0);
    assert(result.size()==4);
    std::set<std::string> nodeName;
    while(!result.empty())
    {
        auto node=result.top();
        result.pop();
        nodeName.insert(node.second->name);
    }
    assert(nodeName.size()==4);
    assert(nodeName.find("A") != nodeName.end());
    assert(nodeName.find("B") != nodeName.end());
    assert(nodeName.find("C") != nodeName.end());
    assert(nodeName.find("D") != nodeName.end());
    for(auto it:nodes)
    {
        delete it;
    }
}

void priorityQueueSearchSameCenterDifferentViewportTest()
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

    auto collectNames = [](std::priority_queue<Distancecmp> queue)
    {
        std::set<std::string> names;
        while(!queue.empty())
        {
            names.insert(queue.top().second->name);
            queue.pop();
        }
        return names;
    };

    auto smallViewport=dataManager.priorityQueueSearch(-20,20,20,-20,0);
    auto largeViewport=dataManager.priorityQueueSearch(-200,200,200,-200,0);
    auto smallNames=collectNames(smallViewport);
    auto largeNames=collectNames(largeViewport);

    assert(smallNames.size()==100);
    assert(largeNames.size()==100);
    assert(smallNames==largeNames);

    for(auto it:nodes)
    {
        delete it;
    }
}

void GraphqueryDataInViewportNullEdgeTest()
{
    Node* a=new Node{"A", {}, 0, 0, {0}};
    Node* b=new Node{"B", {}, 1, 0, {1}};
    Node* c=new Node{"C", {}, 2, 0, {2}};
    std::set<Node*> nodes={a,b,c};
    Edge* validEdge=new Edge{"AB",nullptr,nullptr, 1.0, 100, 1.0, 1.0};
    Edge* nullFromEdge=new Edge{"XB",nullptr,nullptr, 1.0, 100, 1.0, 1.0};
    Edge* nullToEdge=new Edge{"AY",nullptr,nullptr, 1.0, 100, 1.0, 1.0};

    validEdge->from=a;
    validEdge->to=b;
    nullFromEdge->from=nullptr;
    nullFromEdge->to=b;
    nullToEdge->from=a;
    nullToEdge->to=nullptr;

    a->edges.push_back(validEdge);
    a->edges.push_back(nullptr);
    a->edges.push_back(nullToEdge);
    b->edges.push_back(validEdge);
    b->edges.push_back(nullFromEdge);
    c->edges.push_back(nullptr);

    std::set<const Node*> constNodes(nodes.begin(), nodes.end());
    std::set<const Edge*> constEdges={validEdge, nullFromEdge, nullToEdge};
    Graph graph={constNodes,constEdges};
    DataManager dataManager(graph);
    auto result=dataManager.queryDataInViewport(0,2,0,0,0);
    assert(result.first.size()==3);
    assert(result.second.size()==1);
    assert((*result.second.begin())->name=="AB");

    for(auto it:nodes)
    {
        delete it;
    }
    delete validEdge;
    delete nullFromEdge;
    delete nullToEdge;
}

void GraphqueryDataInViewportLevelCompatibility()
{
    Node* a=new Node{"A", {}, 0, 0, {0}};
    Node* b=new Node{"B", {}, 2, 0, {1}};
    Node* c=new Node{"C", {}, 4, 0, {2}};
    std::set<Node*> nodes={a,b,c};
    Edge* ab=new Edge{"AB", a, b, 2.0, 100, 1.0, 1.0};
    Edge* bc=new Edge{"BC", b, c, 2.0, 100, 1.0, 1.0};
    a->edges.push_back(ab);
    b->edges.push_back(ab);
    b->edges.push_back(bc);
    c->edges.push_back(bc);

    Graph graph={{a,b,c},{ab,bc}};
    DataManager dataManager(graph);
    auto level0=dataManager.queryDataInViewport(0,4,0,0,0);
    auto level3=dataManager.queryDataInViewport(0,4,0,0,3);

    std::set<std::string> level0Nodes;
    std::set<std::string> level3Nodes;
    std::set<std::string> level0Edges;
    std::set<std::string> level3Edges;
    for(const auto& node:level0.first)
    {
        level0Nodes.insert(node->name);
    }
    for(const auto& node:level3.first)
    {
        level3Nodes.insert(node->name);
    }
    for(const auto& edge:level0.second)
    {
        level0Edges.insert(edge->name);
    }
    for(const auto& edge:level3.second)
    {
        level3Edges.insert(edge->name);
    }

    assert(level0Nodes==level3Nodes);
    assert(level0Edges==level3Edges);

    delete a;
    delete b;
    delete c;
    delete ab;
    delete bc;
}

void GraphqueryDataInViewportExactly100Test()
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
    auto result=dataManager.queryDataInViewport(-20,20,20,-20,0);
    assert(result.first.size()==100);
    assert(result.second.empty());

    std::set<std::string> nodeNames;
    for(const auto& node:result.first)
    {
        nodeNames.insert(node->name);
    }
    assert(nodeNames.find("K101") == nodeNames.end());
    assert(nodeNames.find("L") == nodeNames.end());
    assert(nodeNames.find("R") == nodeNames.end());
    assert(nodeNames.find("T") == nodeNames.end());
    assert(nodeNames.find("B") == nodeNames.end());

    for(auto it:nodes)
    {
        delete it;
    }
}

void GraphqueryDataInViewportBoundaryConsistencyTest()
{
    Node* a=new Node{"A", {}, -2, 0, {0}};
    Node* b=new Node{"B", {}, 0, 0, {1}};
    Node* c=new Node{"C", {}, 2, 0, {2}};
    Node* d=new Node{"D", {}, 0, 2, {3}};
    std::set<Node*> nodes={a,b,c,d};
    Edge* ab=new Edge{"AB", a, b, 2.0, 100, 1.0, 1.0};
    Edge* bc=new Edge{"BC", b, c, 2.0, 100, 1.0, 1.0};
    Edge* bd=new Edge{"BD", b, d, 2.0, 100, 1.0, 1.0};
    a->edges.push_back(ab);
    b->edges.push_back(ab);
    b->edges.push_back(bc);
    c->edges.push_back(bc);
    b->edges.push_back(bd);
    d->edges.push_back(bd);

    Graph graph={{a,b,c,d},{ab,bc,bd}};
    DataManager dataManager(graph);

    auto normal=dataManager.queryDataInViewport(-10,10,10,-10,0);
    auto reversed=dataManager.queryDataInViewport(10,-10,-10,10,0);
    auto oversized=dataManager.queryDataInViewport(-100,100,100,-100,0);

    auto collectNodeNames = [](const Graph& current)
    {
        std::set<std::string> names;
        for(const auto& node:current.first)
        {
            names.insert(node->name);
        }
        return names;
    };
    auto collectEdgeNames = [](const Graph& current)
    {
        std::set<std::string> names;
        for(const auto& edge:current.second)
        {
            names.insert(edge->name);
        }
        return names;
    };

    auto normalNodes=collectNodeNames(normal);
    auto reversedNodes=collectNodeNames(reversed);
    auto oversizedNodes=collectNodeNames(oversized);
    auto normalEdges=collectEdgeNames(normal);
    auto reversedEdges=collectEdgeNames(reversed);
    auto oversizedEdges=collectEdgeNames(oversized);

    assert(normalNodes==reversedNodes);
    assert(normalNodes==oversizedNodes);
    assert(normalEdges==reversedEdges);
    assert(normalEdges==oversizedEdges);

    delete a;
    delete b;
    delete c;
    delete d;
    delete ab;
    delete bc;
    delete bd;
}

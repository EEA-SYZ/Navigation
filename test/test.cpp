#define DATA_MANAGER_TEST
#include "data_manager.hpp"
#include <cassert>
#include <set>
#include <string>
#include <vector>
#include <iostream>

const bool LAB_TEST=true;

void normalConstructorTest();
void emptyConstructorTest();
void sameConstructorTest();
void nullConstructorTest();

void nodeInViewPortBasicTest();
void nodeInViewPortReverseBoundaryTest();
void nodeInViewPortOnBoundaryTest();
void nodeInViewPortOutsideBoundaryTest();
void nodeInViewPortOutsideGraphTest();
void nodeInViewPortOnGraphTest();
void nodeInViewPortOverlapBoundaryTest();
void nodeInViewPortNullNodeTest();

void getNearest100NodesBoundsBasicTest();
void getNearest100NodesBoundsExactly100Test();
void getNearest100NodesBoundsLevelCompatibilityTest();
void queryNearest100SubgraphBasicTest();
void queryNearest100SubgraphExactly100Test();
void queryNearest100SubgraphNullEdgeTest();

void queryDataInViewportLevel0BasicTest();
void queryDataInViewportBoundaryConsistencyTest();
void queryDataInViewportInvalidLevelFallbackTest();
void queryDataInViewportLevelRepresentativeTest();
void queryDataInViewportLevelFallbackTieBreakTest();
void queryDataInViewportNullEdgeTest();

std::set<std::string> collectNodeNames(const std::vector<const Node*> &nodes);
std::set<std::string> collectNodeNames(const Graph &graph);
std::set<std::string> collectEdgeNames(const Graph &graph);

int main()
{
    if(LAB_TEST)
    {
        normalConstructorTest();
        emptyConstructorTest();
        sameConstructorTest();
        nullConstructorTest();

        nodeInViewPortBasicTest();
        nodeInViewPortReverseBoundaryTest();
        nodeInViewPortOnBoundaryTest();
        nodeInViewPortOutsideBoundaryTest();
        nodeInViewPortOutsideGraphTest();
        nodeInViewPortOnGraphTest();
        nodeInViewPortOverlapBoundaryTest();
        nodeInViewPortNullNodeTest();

        getNearest100NodesBoundsBasicTest();
        getNearest100NodesBoundsExactly100Test();
        getNearest100NodesBoundsLevelCompatibilityTest();
        queryNearest100SubgraphBasicTest();
        queryNearest100SubgraphExactly100Test();
        queryNearest100SubgraphNullEdgeTest();

        queryDataInViewportLevel0BasicTest();
        queryDataInViewportBoundaryConsistencyTest();
        queryDataInViewportInvalidLevelFallbackTest();
        queryDataInViewportLevelRepresentativeTest();
        queryDataInViewportLevelFallbackTieBreakTest();
        queryDataInViewportNullEdgeTest();
    }
}

std::set<std::string> collectNodeNames(const std::vector<const Node*> &nodes)
{
    std::set<std::string> names;
    for (const auto& node : nodes)
    {
        if (node != nullptr)
        {
            names.insert(node->name);
        }
    }
    return names;
}

std::set<std::string> collectNodeNames(const Graph &graph)
{
    std::set<std::string> names;
    for (const auto& node : graph.first)
    {
        if (node != nullptr)
        {
            names.insert(node->name);
        }
    }
    return names;
}

std::set<std::string> collectEdgeNames(const Graph &graph)
{
    std::set<std::string> names;
    for (const auto& edge : graph.second)
    {
        if (edge != nullptr)
        {
            names.insert(edge->name);
        }
    }
    return names;
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
    assert(dataManager.getColNums()==2);
    assert(dataManager.getRowNums()==2);
    assert(dataManager.getLeftBound()==-0.5);
    assert(dataManager.getRightBound()==0.5);
    assert(dataManager.getTopBound()==0.5);
    assert(dataManager.getBottomBound()==-0.5);
    assert(dataManager.getCellWidth()==0.5);
    assert(dataManager.getCellHeight()==0.5);
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

void nodeInViewPortBasicTest()
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
    auto result=dataManager.getNodesInViewPort(-1,3,2,-1,0);
    assert(result.size()==3);
    assert((collectNodeNames(result)==std::set<std::string>{"A","B","E"}));
    for(auto it:nodes)
    {
        delete it;
    }
}

void nodeInViewPortReverseBoundaryTest()
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
    auto result=dataManager.getNodesInViewPort(3,-1,-1,2,0);
    assert(result.size()==3);
    assert((collectNodeNames(result)==std::set<std::string>{"A","B","E"}));
    for(auto it:nodes)
    {
        delete it;
    }
}

void nodeInViewPortOnBoundaryTest()
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
    auto result=dataManager.getNodesInViewPort(0,2,1,0,0);
    assert(result.size()==2);
    assert((collectNodeNames(result)==std::set<std::string>{"A","B"}));
    for(auto it:nodes)
    {
        delete it;
    }
}

void nodeInViewPortOutsideBoundaryTest()
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
    auto result=dataManager.getNodesInViewPort(6,7,7,6,0);
    assert(result.empty());
    for(auto it:nodes)
    {
        delete it;
    }
}

void nodeInViewPortOutsideGraphTest()
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
    auto result=dataManager.getNodesInViewPort(-100,1,1,-100,0);
    assert(result.size()==1);
    assert((collectNodeNames(result)==std::set<std::string>{"A"}));
    for(auto it:nodes)
    {
        delete it;
    }
}

void nodeInViewPortOnGraphTest()
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
    auto result=dataManager.getNodesInViewPort(-100,0,3,-100,0);
    assert(result.size()==2);
    assert((collectNodeNames(result)==std::set<std::string>{"A","D"}));
    for(auto it:nodes)
    {
        delete it;
    }
}

void nodeInViewPortOverlapBoundaryTest()
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
    auto result=dataManager.getNodesInViewPort(10,10,10,10,0);
    assert(result.size()==1);
    assert((collectNodeNames(result)==std::set<std::string>{"F"}));
    for(auto it:nodes)
    {
        delete it;
    }
}

void nodeInViewPortNullNodeTest()
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
    auto result=dataManager.getNodesInViewPort(-2,0,3,0,0);
    assert(result.size()==2);
    assert((collectNodeNames(result)==std::set<std::string>{"A","D"}));
    for(auto it:nodes)
    {
        delete it;
    }
}

void getNearest100NodesBoundsBasicTest()
{
    std::set<const Node*> nodes={
        new Node{"A", {}, 0, 0, {0}},
        new Node{"B", {}, 2, 1, {1}},
        new Node{"C", {}, -1, 3, {2}},
        new Node{"D", {}, 4, -2, {3}}
    };
    Graph graph={nodes,{}};
    DataManager dataManager(graph);
    auto bound=dataManager.getNearest100NodesBounds(0,0,0);
    assert((bound==Boundary{-1,4,3,-2}));
    for(auto it:nodes)
    {
        delete it;
    }
}

void getNearest100NodesBoundsExactly100Test()
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
    auto bound=dataManager.getNearest100NodesBounds(0,0,0);
    assert((bound==Boundary{0,10,0,0}));

    for(auto it:nodes)
    {
        delete it;
    }
}

void getNearest100NodesBoundsLevelCompatibilityTest()
{
    std::set<const Node*> nodes={
        new Node{"A", {}, 0, 0, {0,0}},
        new Node{"B", {}, 2, 1, {1,0}},
        new Node{"C", {}, -1, 3, {2,2}},
        new Node{"D", {}, 4, -2, {3,2}}
    };
    Graph graph={nodes,{}};
    DataManager dataManager(graph);
    auto level0=dataManager.getNearest100NodesBounds(0,0,0);
    auto level3=dataManager.getNearest100NodesBounds(0,0,3);
    assert(level0==level3);
    for(auto it:nodes)
    {
        delete it;
    }
}

void queryNearest100SubgraphBasicTest()
{
    Node* a=new Node{"A", {}, 0, 0, {0}};
    Node* b=new Node{"B", {}, 2, 0, {1}};
    Node* c=new Node{"C", {}, 4, 0, {2}};
    Node* d=new Node{"D", {}, 8, 0, {3}};
    Edge* ab=new Edge{"AB", a, b, 2.0, 100, 1.0, 1.0};
    Edge* bc=new Edge{"BC", b, c, 2.0, 100, 1.0, 1.0};
    Edge* cd=new Edge{"CD", c, d, 4.0, 100, 1.0, 1.0};
    Edge* bd=new Edge{"BD", b, d, 6.0, 100, 1.0, 1.0};
    a->edges.push_back(ab);
    b->edges.push_back(ab);
    b->edges.push_back(bc);
    c->edges.push_back(bc);
    c->edges.push_back(cd);
    d->edges.push_back(cd);
    b->edges.push_back(bd);
    d->edges.push_back(bd);

    Graph graph={{a,b,c,d},{ab,bc,cd,bd}};
    DataManager dataManager(graph);
    auto result=dataManager.queryNearest100Subgraph(0,0,0);
    assert((collectNodeNames(result)==std::set<std::string>{"A","B","C","D"}));
    assert((collectEdgeNames(result)==std::set<std::string>{"AB","BC","BD","CD"}));

    delete a;
    delete b;
    delete c;
    delete d;
    delete ab;
    delete bc;
    delete cd;
    delete bd;
}

void queryNearest100SubgraphExactly100Test()
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
    auto result=dataManager.queryNearest100Subgraph(0,0,0);
    assert(result.first.size()==100);
    assert(result.second.empty());

    std::set<std::string> nodeNames=collectNodeNames(result);
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

void queryNearest100SubgraphNullEdgeTest()
{
    Node* a=new Node{"A", {}, 0, 0, {0}};
    Node* b=new Node{"B", {}, 1, 0, {1}};
    Node* c=new Node{"C", {}, 2, 0, {2}};
    Edge* validEdge=new Edge{"AB", a, b, 1.0, 100, 1.0, 1.0};
    Edge* nullFromEdge=new Edge{"XB", nullptr, b, 1.0, 100, 1.0, 1.0};
    Edge* nullToEdge=new Edge{"AY", a, nullptr, 1.0, 100, 1.0, 1.0};

    a->edges.push_back(validEdge);
    a->edges.push_back(nullptr);
    a->edges.push_back(nullToEdge);
    b->edges.push_back(validEdge);
    b->edges.push_back(nullFromEdge);
    c->edges.push_back(nullptr);

    Graph graph={{a,b,c},{validEdge, nullFromEdge, nullToEdge}};
    DataManager dataManager(graph);
    auto result=dataManager.queryNearest100Subgraph(0,0,0);
    assert((collectNodeNames(result)==std::set<std::string>{"A","B","C"}));
    assert((collectEdgeNames(result)==std::set<std::string>{"AB"}));

    delete a;
    delete b;
    delete c;
    delete validEdge;
    delete nullFromEdge;
    delete nullToEdge;
}

void queryDataInViewportLevel0BasicTest()
{
    Node* a=new Node{"A", {}, 0, 0, {0}};
    Node* b=new Node{"B", {}, 2, 0, {1}};
    Node* c=new Node{"C", {}, 4, 0, {2}};
    Node* d=new Node{"D", {}, 8, 0, {3}};
    Edge* ab=new Edge{"AB", a, b, 2.0, 100, 1.0, 1.0};
    Edge* bc=new Edge{"BC", b, c, 2.0, 100, 1.0, 1.0};
    Edge* cd=new Edge{"CD", c, d, 4.0, 100, 1.0, 1.0};
    Edge* bd=new Edge{"BD", b, d, 6.0, 100, 1.0, 1.0};
    a->edges.push_back(ab);
    b->edges.push_back(ab);
    b->edges.push_back(bc);
    c->edges.push_back(bc);
    c->edges.push_back(cd);
    d->edges.push_back(cd);
    b->edges.push_back(bd);
    d->edges.push_back(bd);

    Graph graph={{a,b,c,d},{ab,bc,cd,bd}};
    DataManager dataManager(graph);
    auto result=dataManager.queryDataInViewport(0,4,0,0,0);
    assert((collectNodeNames(result)==std::set<std::string>{"A","B","C"}));
    assert((collectEdgeNames(result)==std::set<std::string>{"AB","BC"}));

    delete a;
    delete b;
    delete c;
    delete d;
    delete ab;
    delete bc;
    delete cd;
    delete bd;
}

void queryDataInViewportBoundaryConsistencyTest()
{
    Node* a=new Node{"A", {}, -2, 0, {0}};
    Node* b=new Node{"B", {}, 0, 0, {1}};
    Node* c=new Node{"C", {}, 2, 0, {2}};
    Node* d=new Node{"D", {}, 0, 2, {3}};
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

    assert(collectNodeNames(normal)==collectNodeNames(reversed));
    assert(collectNodeNames(normal)==collectNodeNames(oversized));
    assert(collectEdgeNames(normal)==collectEdgeNames(reversed));
    assert(collectEdgeNames(normal)==collectEdgeNames(oversized));

    delete a;
    delete b;
    delete c;
    delete d;
    delete ab;
    delete bc;
    delete bd;
}

void queryDataInViewportInvalidLevelFallbackTest()
{
    Node* a=new Node{"A", {}, 0, 0, {1}};
    Node* b=new Node{"B", {}, 2, 0, {2}};
    Node* c=new Node{"C", {}, 4, 0, {3}};
    Graph graph={{a,b,c},{}};
    DataManager dataManager(graph);

    auto level0=dataManager.queryDataInViewport(0,4,0,0,0);
    auto invalidLevel=dataManager.queryDataInViewport(0,4,0,0,5);
    assert(collectNodeNames(level0)==collectNodeNames(invalidLevel));
    assert(collectEdgeNames(level0)==collectEdgeNames(invalidLevel));

    delete a;
    delete b;
    delete c;
}

void queryDataInViewportLevelRepresentativeTest()
{
    Node* a=new Node{"A", {}, 0, 0, {1,1}};
    Node* b=new Node{"B", {}, 1, 0, {2,1}};
    Node* c=new Node{"C", {}, 5, 0, {3,3}};
    Node* d=new Node{"D", {}, 6, 0, {4,3}};
    Edge* ac=new Edge{"AC", a, c, 5.0, 100, 1.0, 1.0};
    Edge* bd=new Edge{"BD", b, d, 5.0, 100, 1.0, 1.0};
    a->edges.push_back(ac);
    c->edges.push_back(ac);
    b->edges.push_back(bd);
    d->edges.push_back(bd);

    Graph graph={{a,b,c,d},{ac,bd}};
    DataManager dataManager(graph);
    auto result=dataManager.queryDataInViewport(-1,7,1,-1,1);
    assert((collectNodeNames(result)==std::set<std::string>{"A","C"}));
    assert((collectEdgeNames(result)==std::set<std::string>{"AC"}));

    delete a;
    delete b;
    delete c;
    delete d;
    delete ac;
    delete bd;
}

void queryDataInViewportLevelFallbackTieBreakTest()
{
    Node* e=new Node{"E", {}, 100, 100, {5,5}};
    Node* f=new Node{"F", {}, 0, 0, {6,5}};
    Node* g=new Node{"G", {}, 10, 0, {7,5}};
    Node* h=new Node{"H", {}, 8, 2, {8,8}};
    Graph graph={{e,f,g,h},{}};
    DataManager dataManager(graph);
    auto result=dataManager.queryDataInViewport(-1,11,5,-1,1);
    assert((collectNodeNames(result)==std::set<std::string>{"F","H"}));

    delete e;
    delete f;
    delete g;
    delete h;
}

void queryDataInViewportNullEdgeTest()
{
    Node* a=new Node{"A", {}, 0, 0, {0}};
    Node* b=new Node{"B", {}, 1, 0, {1}};
    Node* c=new Node{"C", {}, 2, 0, {2}};
    Edge* validEdge=new Edge{"AB", a, b, 1.0, 100, 1.0, 1.0};
    Edge* nullFromEdge=new Edge{"XB", nullptr, b, 1.0, 100, 1.0, 1.0};
    Edge* nullToEdge=new Edge{"AY", a, nullptr, 1.0, 100, 1.0, 1.0};

    a->edges.push_back(validEdge);
    a->edges.push_back(nullptr);
    a->edges.push_back(nullToEdge);
    b->edges.push_back(validEdge);
    b->edges.push_back(nullFromEdge);
    c->edges.push_back(nullptr);

    Graph graph={{a,b,c},{validEdge, nullFromEdge, nullToEdge}};
    DataManager dataManager(graph);
    auto result=dataManager.queryDataInViewport(0,2,0,0,0);
    assert((collectNodeNames(result)==std::set<std::string>{"A","B","C"}));
    assert((collectEdgeNames(result)==std::set<std::string>{"AB"}));

    delete a;
    delete b;
    delete c;
    delete validEdge;
    delete nullFromEdge;
    delete nullToEdge;
}

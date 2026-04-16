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

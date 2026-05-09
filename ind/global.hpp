#ifndef __GLOBAL_HPP__
#define __GLOBAL_HPP__

/**
 * @brief 全局结构体
 */

#include <string>
#include <vector>
#include <utility>
#include <cmath>
#include <unordered_map>
#include <set>

struct Node;
struct Edge;

/**
 * @brief 节点结构体
 */
struct Node {
    std::string name; // 节点名称
    std::vector<Edge *> edges; // 节点连接的边
    double x; // 节点的 x 坐标
    double y; // 节点的 y 坐标
    std::vector<int> address; // 节点地址（层结构的列）
};

/**
 * @brief 边结构体
 */
struct Edge {
    std::string name; // 边名称
    Node *from; // 边的起点
    Node *to; // 边的终点
    double length; // 边的长度
    int volume; // 边的流量容量
    double p1; // 边的时间系数一
    double p2; // 边的时间系数二
    /**
     * @brief 计算边的时间
     * @param n 流量
     * @return 计算的时间
     */
    double getTime(int n) const
    {
        double f;

        auto tmp = 1. * n / volume;
        if (tmp <= p2) f = 1;
        else f = 1 + exp(1) * tmp;

        return p1 * length * f;
    }
};

using Graph = std::pair<std::set<const Node *>, std::set<const Edge *>>;
using VPath = std::vector<const Node *>;
using EPath = std::vector<const Edge *>;

/**
 * @brief 标签结构体
 * 用于存储节点和边的标签
 */
class Tag {
public:
    /**
     * @brief 获取标签实例
     * @return Tag & 标签实例
     */
    static Tag &instance()
    {
        static Tag tag;
        return tag;
    }
    /**
     * @brief 获取节点的标签
     * @param node 节点
     * @return std::unordered_map<std::string, std::string> 节点的标签
     */
    std::unordered_map<std::string, std::string> &operator[](const Node *node)
    {
        static std::unordered_map<const Node *, std::unordered_map<std::string, std::string>> tag;
        return tag[node];
    }
    /**
     * @brief 获取边的标签
     * @param edge 边
     * @return std::unordered_map<std::string, std::string> 边的标签
     */
    std::unordered_map<std::string, std::string> &operator[](const Edge *edge)
    {
        static std::unordered_map<const Edge *, std::unordered_map<std::string, std::string>> tag;
        return tag[edge];
    }
    
    Tag(const Tag &) = delete;
    Tag &operator=(const Tag &) = delete;
    Tag(Tag &&) = delete;
    Tag &operator=(Tag &&) = delete;
private:
    Tag();
    ~Tag();
};

const double PI = std::acos(-1.0);

#endif

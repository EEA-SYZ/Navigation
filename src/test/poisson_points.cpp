#include "data_maker.hpp"


#include <algorithm>
#include <chrono>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>
#include <random>

#include <stdexcept>
#include <string>
#include <vector>

struct Config {
    int count = 200;
    int edgeCount = 0;
    int levelCount = 5;
    int levelVolume = 50;
struct Point {
    std::string name;
    double x;
    double y;
};

struct Config {
    int count = 200;
    double left = 0.0;
    double right = 1000.0;
    double bottom = 0.0;
    double top = 1000.0;
    double radius = 0.0;
    int attempts = 30;
    unsigned int seed = 0;
    bool hasSeed = false;
};

double parseDouble(const char *value, const std::string &option)
{
    try {
        return std::stod(value);
    } catch (...) {
        throw std::runtime_error("invalid value for " + option + ": " + value);
    }
}

int parseInt(const char *value, const std::string &option)
{
    try {
        return std::stoi(value);
    } catch (...) {
        throw std::runtime_error("invalid value for " + option + ": " + value);
    }
}

Config parseArgs(int argc, char **argv)
{
    Config cfg;
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        auto nextValue = [&](const std::string &option) -> const char * {
            if (i + 1 >= argc) {
                throw std::runtime_error("missing value for " + option);
            }
            return argv[++i];
        };

        if (arg == "-n" || arg == "--count") {
            cfg.count = parseInt(nextValue(arg), arg);
        } else if (arg == "-e" || arg == "--edges") {
            cfg.edgeCount = parseInt(nextValue(arg), arg);
        } else if (arg == "-l" || arg == "--levels") {
            cfg.levelCount = parseInt(nextValue(arg), arg);
        } else if (arg == "--level-volume") {
            cfg.levelVolume = parseInt(nextValue(arg), arg);
        } else if (arg == "--left") {
            cfg.left = parseDouble(nextValue(arg), arg);
        } else if (arg == "--right") {
            cfg.right = parseDouble(nextValue(arg), arg);
        } else if (arg == "--bottom") {
            cfg.bottom = parseDouble(nextValue(arg), arg);
        } else if (arg == "--top") {
            cfg.top = parseDouble(nextValue(arg), arg);
        } else if (arg == "-r" || arg == "--radius" || arg == "-k" || arg == "--attempts" || arg == "--seed") {
            // Kept for compatibility with older poisson_points commands. DataMaker
            // owns sampling parameters now, so these values are intentionally ignored.
            nextValue(arg);
        } else if (arg == "-h" || arg == "--help") {
            std::cout
                << "Usage: poisson_points [options]\n"
        } else if (arg == "-r" || arg == "--radius") {
            cfg.radius = parseDouble(nextValue(arg), arg);
        } else if (arg == "-k" || arg == "--attempts") {
            cfg.attempts = parseInt(nextValue(arg), arg);
        } else if (arg == "--seed") {
            cfg.seed = static_cast<unsigned int>(parseInt(nextValue(arg), arg));
            cfg.hasSeed = true;
        } else if (arg == "-h" || arg == "--help") {
            std::cout
                << "Usage: poisson_points [options]\n"
                << "  -n, --count N       target point count, default 200\n"
                << "  --left X            left bound, default 0\n"
                << "  --right X           right bound, default 1000\n"
                << "  --bottom Y          bottom bound, default 0\n"
                << "  --top Y             top bound, default 1000\n"
                << "  -r, --radius R      minimum distance; auto when omitted\n"
                << "  -k, --attempts K    tries per active point, default 30\n"
                << "  --seed S            random seed\n";
                << "  -n, --count N          target node count, default 200\n"
                << "  -e, --edges N          target edge count, default 0\n"
                << "  -l, --levels N         hierarchy level count, default 5\n"
                << "  --level-volume N       level edge volume, default 50\n"
                << "  --left X               left bound, default 0\n"
                << "  --right X              right bound, default 1000\n"
                << "  --bottom Y             bottom bound, default 0\n"
                << "  --top Y                top bound, default 1000\n"
                << "  -r, -k, --seed         accepted for old scripts; ignored\n";
            std::exit(0);
        } else {
            throw std::runtime_error("unknown option: " + arg);
        }
    }
    return cfg;
}

std::string addressToString(const std::vector<int> &address)
std::vector<Point> poissonDiskSample(Config &cfg)
{
    std::ostringstream out;
    for (std::size_t i = 0; i < address.size(); ++i) {
        if (i) {
            out << ';';
        }
        out << address[i];
    }
    return out.str();
}

int representativeLevel(const Node *node)
{
    if (node == nullptr || node->address.empty()) {
        return 0;
    }

    int level = 0;
    int self = node->address.front();
    for (std::size_t i = 1; i < node->address.size(); ++i) {
        if (node->address[i] == self) {
            level = static_cast<int>(i);
        }
    }
    return level;
}

int commonAddressLevel(const Node *lhs, const Node *rhs)
{
    if (lhs == nullptr || rhs == nullptr) {
        return 0;
    }

    int limit = static_cast<int>(std::min(lhs->address.size(), rhs->address.size()));
    int level = 0;
    for (int i = 1; i < limit; ++i) {
        if (lhs->address[i] >= 0 && lhs->address[i] == rhs->address[i]) {
            level = i;
            break;
        }
    }
    return level;
}

double nearestDistance(const std::vector<const Node *> &nodes)
{
    if (nodes.size() < 2) {
        return 0.0;
    }

    double best = std::numeric_limits<double>::infinity();
    for (std::size_t i = 0; i < points.size(); ++i) {
        for (std::size_t j = i + 1; j < points.size(); ++j) {
            double dx = points[i].x - points[j].x;
            double dy = points[i].y - points[j].y;
    for (std::size_t i = 0; i < nodes.size(); ++i) {
        for (std::size_t j = i + 1; j < nodes.size(); ++j) {
            double dx = nodes[i]->x - nodes[j]->x;
            double dy = nodes[i]->y - nodes[j]->y;
            best = std::min(best, std::sqrt(dx * dx + dy * dy));
        }
    }
    return best;
}

double cross(const Node *a, const Node *b, const Node *c)
{
    return (b->x - a->x) * (c->y - a->y) - (b->y - a->y) * (c->x - a->x);
}

bool onSegment(const Node *point, const Node *a, const Node *b)
{
    constexpr double eps = 1e-9;
    if (std::abs(cross(a, b, point)) > eps) {
        return false;
    }

    return point->x >= std::min(a->x, b->x) - eps &&
           point->x <= std::max(a->x, b->x) + eps &&
           point->y >= std::min(a->y, b->y) - eps &&
           point->y <= std::max(a->y, b->y) + eps;
}

bool intersects(const Edge *lhs, const Edge *rhs)
{
    if (lhs->from == rhs->from || lhs->from == rhs->to ||
        lhs->to == rhs->from || lhs->to == rhs->to) {
        return false;
    }

    constexpr double eps = 1e-9;
    double abC = cross(lhs->from, lhs->to, rhs->from);
    double abD = cross(lhs->from, lhs->to, rhs->to);
    double cdA = cross(rhs->from, rhs->to, lhs->from);
    double cdB = cross(rhs->from, rhs->to, lhs->to);

    if (((abC > eps && abD < -eps) || (abC < -eps && abD > eps)) &&
        ((cdA > eps && cdB < -eps) || (cdA < -eps && cdB > eps))) {
        return true;
    }

    return onSegment(rhs->from, lhs->from, lhs->to) ||
           onSegment(rhs->to, lhs->from, lhs->to) ||
           onSegment(lhs->from, rhs->from, rhs->to) ||
           onSegment(lhs->to, rhs->from, rhs->to);
}

int countCrossingEdges(const Graph &graph)
{
    std::vector<const Edge *> edges(graph.second.begin(), graph.second.end());
    int crossings = 0;
    for (std::size_t i = 0; i < edges.size(); ++i) {
        for (std::size_t j = i + 1; j < edges.size(); ++j) {
            if (intersects(edges[i], edges[j])) {
                ++crossings;
            }
        }
    }
    return crossings;
}

int main(int argc, char **argv)
{
    auto start = std::chrono::steady_clock::now();

    try {
        Config cfg = parseArgs(argc, argv);
        DataMaker maker(
            cfg.left,
            cfg.right,
            cfg.top,
            cfg.bottom,
            cfg.count,
            cfg.edgeCount,
            cfg.levelCount,
            cfg.levelVolume
        );
        const Graph &graph = maker.getGraph();

        std::vector<const Node *> nodes(graph.first.begin(), graph.first.end());
        std::sort(nodes.begin(), nodes.end(), [](const Node *lhs, const Node *rhs) {
            return lhs->name < rhs->name;
        });

        std::cerr << "generated nodes: " << nodes.size() << " / " << cfg.count << '\n';
        std::cerr << "generated edges: " << graph.second.size() << " / " << cfg.edgeCount << '\n';
        if (!graph.second.empty() && graph.second.size() <= 5000) {
            std::cerr << "edge crossings: " << countCrossingEdges(graph) << '\n';
        } else if (!graph.second.empty()) {
            std::cerr << "edge crossings: skipped for " << graph.second.size()
                      << " edges\n";
        }
        std::cerr << "levels: " << cfg.levelCount << '\n';
        if (nodes.size() >= 2) {
            std::cerr << "nearest distance: " << std::fixed << std::setprecision(4)
                      << nearestDistance(nodes) << '\n';
        }

        std::cout << "kind,name,x,y,address,level,from,to,length,volume,edge_level\n";
        std::cout << std::fixed << std::setprecision(8);
        for (const Node *node : nodes) {
            std::cout << "node,"
                      << node->name << ','
                      << node->x << ','
                      << node->y << ','
                      << addressToString(node->address) << ','
                      << representativeLevel(node) << ",,,,,\n";
        }

        std::vector<const Edge *> edges(graph.second.begin(), graph.second.end());
        std::sort(edges.begin(), edges.end(), [](const Edge *lhs, const Edge *rhs) {
            return lhs->name < rhs->name;
        });

        for (const Edge *edge : edges) {
            std::cout << "edge,"
                      << edge->name << ",,,,,"
                      << edge->from->name << ','
                      << edge->to->name << ','
                      << edge->length << ','
                      << edge->volume << ','
                      << commonAddressLevel(edge->from, edge->to) << '\n';
        }
    } catch (const std::exception &ex) {
        std::cerr << "error: " << ex.what() << '\n';
        return 1;
    }

    auto end = std::chrono::steady_clock::now();
    auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cerr << "ms: " << dur.count() << '\n';

    return 0;
}

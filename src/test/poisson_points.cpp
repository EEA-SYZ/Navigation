<<<<<<< HEAD
=======
#include "data_maker.hpp"

>>>>>>> c7c9cb91529b0d0f11b58dacd4347cf0784c893d
#include <algorithm>
#include <chrono>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <limits>
<<<<<<< HEAD
#include <random>
=======
#include <sstream>
>>>>>>> c7c9cb91529b0d0f11b58dacd4347cf0784c893d
#include <stdexcept>
#include <string>
#include <vector>

<<<<<<< HEAD
struct Point {
    std::string name;
    double x;
    double y;
};

struct Config {
    int count = 200;
=======
struct Config {
    int count = 200;
    int edgeCount = 0;
    int levelCount = 5;
    int levelVolume = 50;
>>>>>>> c7c9cb91529b0d0f11b58dacd4347cf0784c893d
    double left = 0.0;
    double right = 1000.0;
    double bottom = 0.0;
    double top = 1000.0;
<<<<<<< HEAD
    double radius = 0.0;
    int attempts = 30;
    unsigned int seed = 0;
    bool hasSeed = false;
=======
>>>>>>> c7c9cb91529b0d0f11b58dacd4347cf0784c893d
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
<<<<<<< HEAD
=======
        } else if (arg == "-e" || arg == "--edges") {
            cfg.edgeCount = parseInt(nextValue(arg), arg);
        } else if (arg == "-l" || arg == "--levels") {
            cfg.levelCount = parseInt(nextValue(arg), arg);
        } else if (arg == "--level-volume") {
            cfg.levelVolume = parseInt(nextValue(arg), arg);
>>>>>>> c7c9cb91529b0d0f11b58dacd4347cf0784c893d
        } else if (arg == "--left") {
            cfg.left = parseDouble(nextValue(arg), arg);
        } else if (arg == "--right") {
            cfg.right = parseDouble(nextValue(arg), arg);
        } else if (arg == "--bottom") {
            cfg.bottom = parseDouble(nextValue(arg), arg);
        } else if (arg == "--top") {
            cfg.top = parseDouble(nextValue(arg), arg);
<<<<<<< HEAD
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
=======
        } else if (arg == "-r" || arg == "--radius" || arg == "-k" || arg == "--attempts" || arg == "--seed") {
            // Kept for compatibility with older poisson_points commands. DataMaker
            // owns sampling parameters now, so these values are intentionally ignored.
            nextValue(arg);
        } else if (arg == "-h" || arg == "--help") {
            std::cout
                << "Usage: poisson_points [options]\n"
                << "  -n, --count N          target node count, default 200\n"
                << "  -e, --edges N          target edge count, default 0\n"
                << "  -l, --levels N         hierarchy level count, default 5\n"
                << "  --level-volume N       level edge volume, default 50\n"
                << "  --left X               left bound, default 0\n"
                << "  --right X              right bound, default 1000\n"
                << "  --bottom Y             bottom bound, default 0\n"
                << "  --top Y                top bound, default 1000\n"
                << "  -r, -k, --seed         accepted for old scripts; ignored\n";
>>>>>>> c7c9cb91529b0d0f11b58dacd4347cf0784c893d
            std::exit(0);
        } else {
            throw std::runtime_error("unknown option: " + arg);
        }
    }
    return cfg;
}

<<<<<<< HEAD
std::vector<Point> poissonDiskSample(Config &cfg)
{
    if (cfg.count <= 0) {
        return {};
    }

    if (cfg.left > cfg.right) {
        std::swap(cfg.left, cfg.right);
    }
    if (cfg.bottom > cfg.top) {
        std::swap(cfg.bottom, cfg.top);
    }

    double width = cfg.right - cfg.left;
    double height = cfg.top - cfg.bottom;
    if (width <= 0 || height <= 0) {
        throw std::runtime_error("bounds must enclose a positive area");
    }

    if (cfg.radius <= 0) {
        cfg.radius = std::sqrt(width * height * 0.7 / cfg.count);
    }

    double cellSize = cfg.radius / std::sqrt(2.0);
    int cols = std::max(1, static_cast<int>(std::ceil(width / cellSize)));
    int rows = std::max(1, static_cast<int>(std::ceil(height / cellSize)));
    std::vector<int> grid(cols * rows, -1);
    std::vector<Point> points;
    std::vector<int> active;

    unsigned int seed = cfg.hasSeed
        ? cfg.seed
        : static_cast<unsigned int>(
            std::chrono::steady_clock::now().time_since_epoch().count());
    std::mt19937 rng(seed);
    std::uniform_real_distribution<double> distX(cfg.left, cfg.right);
    std::uniform_real_distribution<double> distY(cfg.bottom, cfg.top);

    auto gridX = [&](double x) {
        int gx = static_cast<int>((x - cfg.left) / cellSize);
        return std::clamp(gx, 0, cols - 1);
    };
    auto gridY = [&](double y) {
        int gy = static_cast<int>((y - cfg.bottom) / cellSize);
        return std::clamp(gy, 0, rows - 1);
    };
    auto addPoint = [&](const Point &point) {
        int gx = gridX(point.x);
        int gy = gridY(point.y);
        grid[gy * cols + gx] = static_cast<int>(points.size());
        active.emplace_back(static_cast<int>(points.size()));
        points.emplace_back(point);
    };
    auto valid = [&](double x, double y) {
        if (x < cfg.left || x > cfg.right || y < cfg.bottom || y > cfg.top) {
            return false;
        }

        int gx = gridX(x);
        int gy = gridY(y);
        double radiusSq = cfg.radius * cfg.radius;
        for (int yy = std::max(0, gy - 2); yy <= std::min(rows - 1, gy + 2); ++yy) {
            for (int xx = std::max(0, gx - 2); xx <= std::min(cols - 1, gx + 2); ++xx) {
                int idx = grid[yy * cols + xx];
                if (idx < 0) {
                    continue;
                }
                const Point &other = points[idx];
                double dx = x - other.x;
                double dy = y - other.y;
                if (dx * dx + dy * dy < radiusSq) {
                    return false;
                }
            }
        }
        return true;
    };

    addPoint(Point{"N0", distX(rng), distY(rng)});

    std::uniform_real_distribution<double> distUnit(0.0, 1.0);
    while (!active.empty() && static_cast<int>(points.size()) < cfg.count) {
        std::uniform_int_distribution<int> distActive(0, static_cast<int>(active.size()) - 1);
        int activeSlot = distActive(rng);
        const Point parent = points[active[activeSlot]];
        bool found = false;

        for (int i = 0; i < cfg.attempts; ++i) {
            double distance = cfg.radius * (1.0 + distUnit(rng));
            double angle = 2.0 * std::acos(-1.0) * distUnit(rng);
            double x = parent.x + distance * std::cos(angle);
            double y = parent.y + distance * std::sin(angle);

            if (valid(x, y)) {
                addPoint(Point{"N" + std::to_string(points.size()), x, y});
                found = true;
                if (static_cast<int>(points.size()) >= cfg.count) {
                    break;
                }
            }
        }

        if (!found) {
            active.erase(active.begin() + activeSlot);
        }
    }

    return points;
}

double nearestDistance(const std::vector<Point> &points)
{
    if (points.size() < 2) {
=======
std::string addressToString(const std::vector<int> &address)
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
>>>>>>> c7c9cb91529b0d0f11b58dacd4347cf0784c893d
        return 0.0;
    }

    double best = std::numeric_limits<double>::infinity();
<<<<<<< HEAD
    for (std::size_t i = 0; i < points.size(); ++i) {
        for (std::size_t j = i + 1; j < points.size(); ++j) {
            double dx = points[i].x - points[j].x;
            double dy = points[i].y - points[j].y;
=======
    for (std::size_t i = 0; i < nodes.size(); ++i) {
        for (std::size_t j = i + 1; j < nodes.size(); ++j) {
            double dx = nodes[i]->x - nodes[j]->x;
            double dy = nodes[i]->y - nodes[j]->y;
>>>>>>> c7c9cb91529b0d0f11b58dacd4347cf0784c893d
            best = std::min(best, std::sqrt(dx * dx + dy * dy));
        }
    }
    return best;
}

<<<<<<< HEAD
int main(int argc, char **argv)
{
    try {
        Config cfg = parseArgs(argc, argv);
        std::vector<Point> points = poissonDiskSample(cfg);

        std::cerr << "generated: " << points.size() << " / " << cfg.count << '\n';
        std::cerr << "radius: " << std::fixed << std::setprecision(4) << cfg.radius << '\n';
        if (points.size() >= 2) {
            std::cerr << "nearest distance: " << nearestDistance(points) << '\n';
        }

        std::cout << "name,x,y\n";
        std::cout << std::fixed << std::setprecision(8);
        for (const auto &point : points) {
            std::cout << point.name << ',' << point.x << ',' << point.y << '\n';
=======
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
>>>>>>> c7c9cb91529b0d0f11b58dacd4347cf0784c893d
        }
    } catch (const std::exception &ex) {
        std::cerr << "error: " << ex.what() << '\n';
        return 1;
    }

<<<<<<< HEAD
=======
    auto end = std::chrono::steady_clock::now();
    auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cerr << "ms: " << dur.count() << '\n';

>>>>>>> c7c9cb91529b0d0f11b58dacd4347cf0784c893d
    return 0;
}

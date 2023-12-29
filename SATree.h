#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <optional>
#include <queue>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>


struct Point
{
    int node_id{-1};
    std::vector<float> v;

    std::string to_string();
    friend bool operator==(const Point & self, const Point & other);
};

using Points = std::vector<Point>;

struct SATreeNode
{
    Point point;
    std::vector<SATreeNode *> neighbours;
    float covering_radius{0};

    SATreeNode(Point p) : point(p) { }
    ~SATreeNode();
};

struct kNNResultTuple
{
    Point p;
    float distance; // Distance from query

    std::string to_string() { return "{" + p.to_string() + ", " + std::to_string(distance) + "}"; }

    friend bool operator<(const kNNResultTuple & self, const kNNResultTuple & other);
    friend bool operator==(const kNNResultTuple & self, const kNNResultTuple & other);
};


using kNNResult = std::vector<kNNResultTuple>;


class SATree
{
    SATreeNode * root;

    std::unordered_map<std::string, float> distance_cache;

    void build(SATreeNode * a, Points & S);
    void print(SATreeNode * node, int space);
    std::string to_string(SATreeNode * node);
    float cached_distance(Point p, Point q);

    std::optional<Point> range_search(SATreeNode * node, Point query, float radius, float digression);

public:
    SATree(std::vector<Point> S);
    ~SATree();

    void print() { return print(root, 0); }
    std::string to_string() { return to_string(root); }
    std::optional<Point> range_search(Point query, float radius);

    kNNResult nearest_neighbour_search(Point query, int k);
};


// Input is a set of points with N-dimensional(take it to be 2 for now) coordinates
// #include <

#include <cstdint>
#include <vector>
#include <cmath>
#include <algorithm>

// template <uint8_t Dimensions>
struct Point {
    int x;
    int y;
};

int distance(Point a, Point b) {
    return std::sqrt(std::pow((b.y - a.y), 2) + std::pow((b.x - a.x), 2));
}

// template <uint8_t Dimensions>
struct SATreeNode {
    Point point;
    // This is sorted by the distance of a point from the node
    std::vector<SATreeNode> neighbours;
    // std::vector<Point<Dimensions>> bag; // State internal to the algorithm
    int covering_radius{0};
};

// template <uint8_t Dimensions>
class SATree {
    SATreeNode root;

    void build(std::vector<Point> S, SATreeNode a);
    // int distance(Point x, Point y);

public:
    // Build the tree
    SATree(std::vector<Point> S, SATreeNode a) {
        root.point = S.back();
        S.pop_back();
        build(S, root);
    }
};

void SATree::build(std::vector<Point> S, SATreeNode a) {
    // Sort in ascending order of distance from `a`
    auto distance_from_a = [&a](Point p, Point q) {
        return distance(a.point, p) < distance(a.point, q);
    };
    std::sort(S.begin(), S.end(), distance_from_a);

    for(auto v: S) {
        a.covering_radius = std::max(a.covering_radius, distance(v, a.point));
        bool add_to_neighbour{true};
        for(auto b: a.neighbours) {
            if(!(distance(v, a.point) < distance(v, b.point))) {
                add_to_neighbour = false;
                break;
            }
        }
        if(add_to_neighbour) {
            SATreeNode x;
            x.point = v;
            a.neighbours.push_back(x);
        }
    }
}
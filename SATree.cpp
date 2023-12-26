#include <algorithm>
#include <cmath>
#include <cstdint>
#include <stdexcept>
#include <vector>

struct Point
{
    int x;
    int y;
};

float distance(Point p, Point q)
{
    return std::sqrt(std::pow((p.y - q.y), 2) + std::pow((p.x - q.x), 2));
}

struct SATreeNode
{
    Point point;
    std::vector<SATreeNode *> neighbours;
    float covering_radius{0};

    SATreeNode(Point p) : point(p) { }
    ~SATreeNode()
    {
        if (neighbours.empty())
            return;

        for (auto n : neighbours)
            if (n)
                delete n;
    }
};

class SATree
{
    SATreeNode * root;
    void build(SATreeNode * a, std::vector<Point> & S);
    Point range_search(SATreeNode *a, Point query, float radius, float digression);

public:
    SATree(std::vector<Point> & S)
    {
        // TODO: Select random element
        root = new SATreeNode(S.back());
        S.pop_back();
        build(root, S);
    }
    ~SATree()
    {
        if (root)
            delete root;
    }

    Point range_search(Point query, float radius);
};

// TODO: reuse distances
void SATree::build(SATreeNode * a, std::vector<Point> & S)
{
    auto distance_from_a = [&a](Point p, Point q) { return distance(p, a->point) < distance(q, a->point); };
    std::sort(S.begin(), S.end(), distance_from_a);

    // Add `v` as neighbour if it is closer to `a` than its neighbours.
    for (auto v = S.begin(); v != S.end(); v++) // for(auto v: S)
    {
        a->covering_radius = std::max(a->covering_radius, distance(*v, a->point));
        bool add_to_neighbour{true};
        // For the case of the first `v`,
        // add it to the neighbour set since it is already sorted by distance to a
        for (auto b : a->neighbours)
        {
            if (!(distance(*v, a->point) < distance(*v, b->point)))
            {
                add_to_neighbour = false;
                break;
            }
        }
        if (add_to_neighbour)
        {
            auto * n = new SATreeNode(*v);
            a->neighbours.push_back(n);
            // Delete from bag also
            S.erase(v);
        }
    }

    // Create bags of the neighbours
    std::vector<std::vector<Point>> bags(a->neighbours.size());

    // Second Pass: Best-fit strategy
    // Put remaining in the bag of the neighbour that is closest to it
    for (auto v : S)
    {
        float min_distance{INFINITY};
        int neighbour_idx; // {0}; Is initialization needed here?
        for (int i = 0; i < bags.size(); i++)
        {
            auto d = distance(v, a->neighbours[i]->point);
            if (d < min_distance)
            {
                min_distance = d;
                neighbour_idx = i;
            }
        }
        bags[neighbour_idx].push_back(v);
    }

    for (int i = 0; i < bags.size(); i++)
        build(a->neighbours[i], bags[i]);
}


Point SATree::range_search(Point query, float radius) {
    return range_search(root, query, radius, 0);
}


Point SATree::range_search(SATreeNode *a, Point query, float radius, float digresion)
{
    return Point{-1, -1};
}

// TODO: How does this terminate and how do we report back the answer??w
// Point SATree::range_search(SATreeNode *a, Point query, float radius, float digression) {
//     auto dist_a_q = distance(a->point, query);
//     if(!(digression <= 2 * radius &&
//         dist_a_q <= a->covering_radius + radius))
//         return Point{-1, -1}; // Sentinel value. TODO: replace

//     if(dist_a_q < radius)
//         return a->point;

//     // Calculate min_d
//     auto min_distance = distance(a->point, query);
//     for(auto c: a->neighbours) {
//         auto d = distance(c->point, query);
//         if(d < min_distance)
//             min_distance = d;
//     }

//     for(auto b: a->neighbours) {
//         auto dist_b_q = distance(b->point, query);
//         if(dist_b_q <= min_distance + (2 * radius))
//             range_search(b, query, radius, std::max(0.0f, digression + (dist_b_q - dist_a_q)));
//     }
// }
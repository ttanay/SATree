#include <algorithm>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <optional>
#include <queue>
#include <stdexcept>
#include <string>
#include <vector>


template <int D>
struct Point
{
    float v[D];

    std::string to_string();
    template <int N>
    friend bool operator==(const Point<N> & self, const Point<N> & other);
};

template <int D>
using Points = std::vector<Point<D>>;

// TODO: Change from operating on points, to a float vector ,i.e., float[].
// float*

template <int D>
struct SATreeNode
{
    Point<D> point;
    std::vector<SATreeNode *> neighbours;
    float covering_radius{0};

    SATreeNode(Point<D> p) : point(p) { }
    ~SATreeNode();
};

template <int D>
struct kNNResultTuple
{
    Point<D> p;
    float distance; // Distance from query

    std::string to_string() { return "{" + p.to_string() + ", " + std::to_string(distance) + "}"; }

    template <int N>
    friend bool operator<(const kNNResultTuple<N> & self, const kNNResultTuple<N> & other);
    template <int N>
    friend bool operator==(const kNNResultTuple<N> & self, const kNNResultTuple<N> & other);
};

template <int D>
using kNNResult = std::vector<kNNResultTuple<D>>;

template <int D>
class SATree
{
    SATreeNode<D> * root;

    void build(SATreeNode<D> * a, Points<D> & S);
    void print(SATreeNode<D> * node, int space);
    std::string to_string(SATreeNode<D> * node);

    std::optional<Point<D>> range_search(SATreeNode<D> * node, Point<D> query, float radius, float digression);

public:
    SATree(std::vector<Point<D>> & S);
    ~SATree();

    void print() { return print(root, 0); }
    std::string to_string() { return to_string(root); }
    std::optional<Point<D>> range_search(Point<D> query, float radius);

    kNNResult<D> nearest_neighbour_search(Point<D> query, int k);
};


template <int D>
std::string Point<D>::to_string()
{
    std::string res = "(";
    for (int i = 0; i < D; i++)
    {
        if (i)
            res += ", ";
        res += std::to_string(v[i]);
    }
    res += ")";
    return res;
}

template <int D>
bool operator==(const Point<D> & self, const Point<D> & other)
{
    for (int i = 0; i < D; i++)
        if (self.v[i] != other.v[i])
            return false;
    return true;
}

template <int D>
float distance(Point<D> p, Point<D> q)
{
    float res{0};
    for (int i = 0; i < D; i++)
        res += std::pow((p.v[i] - q.v[i]), 2);
    return std::sqrt(res);
}

template <int D>
SATreeNode<D>::~SATreeNode()
{
    if (neighbours.empty())
        return;

    for (auto n : neighbours)
        if (n)
            delete n;
}

template <int D>
SATree<D>::SATree(std::vector<Point<D>> & S)
{
    root = new SATreeNode<D>(S.back());
    S.pop_back();
    build(root, S);
}

// TODO: reuse distances
template <int D>
void SATree<D>::build(SATreeNode<D> * a, std::vector<Point<D>> & S)
{
    auto distance_from_a = [&a](Point<D> p, Point<D> q) { return distance(p, a->point) < distance(q, a->point); };
    std::sort(S.begin(), S.end(), distance_from_a);

    // Add `v` as neighbour if it is closer to `a` than its neighbours.
    auto v = S.begin();
    while (v != S.end())
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
        else
            v++;
    }

    // Create bags of the neighbours
    std::vector<std::vector<Point<D>>> bags(a->neighbours.size());

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


template <int D>
SATree<D>::~SATree()
{
    if (root)
        delete root;
}

template <int D>
void SATree<D>::print(SATreeNode<D> * node, int space)
{
    for (int i = 0; i < space; i++)
        std::cout << "  ";
    std::cout << "|- " << node->point.to_string() << " ";
    std::cout << "|N(a)| = " << node->neighbours.size() << std::endl;
    for (auto n : node->neighbours)
        print(n, space + 1);
}

template <int D>
std::string SATree<D>::to_string(SATreeNode<D> * node)
{
    std::string res;
    res += "{" + node->point.to_string() + "}";
    if (node->neighbours.size() > 0)
    {
        res += "(";
        for (auto n : node->neighbours)
        {
            if (n != *(node->neighbours.begin()))
                res += ",";
            res += to_string(n);
        }
        res += ")";
    }
    return res;
}

template <int D>
std::optional<Point<D>> SATree<D>::range_search(Point<D> query, float radius)
{
    return range_search(root, query, radius, 0);
}

template <int D>
std::optional<Point<D>> SATree<D>::range_search(SATreeNode<D> * a, Point<D> query, float radius, float digression)
{
    auto dist_a_q = distance(a->point, query);
    if (!((digression <= 2 * radius) && (dist_a_q <= a->covering_radius + radius)))
        return std::nullopt;

    if (dist_a_q <= radius)
        return a->point;

    // Calculate min distance
    auto min_distance = dist_a_q;
    for (auto c : a->neighbours)
    {
        auto d = distance(c->point, query);
        if (d < min_distance)
            min_distance = d;
    }

    for (auto b : a->neighbours)
    {
        auto dist_b_q = distance(b->point, query);
        if (dist_b_q <= min_distance + (2 * radius))
        {
            auto res = range_search(b, query, radius, std::max(0.0f, digression + (dist_b_q - dist_a_q)));
            if (res)
                return res;
        }
    }
    return std::nullopt;
}

template <int D>
struct PromisingSubtree
{
    SATreeNode<D> * node;
    float lbound;
    float digression;

    template <int T>
    friend bool operator>(const PromisingSubtree<T> & self, const PromisingSubtree<T> & other);
};

template <int D>
bool operator>(const PromisingSubtree<D> & self, const PromisingSubtree<D> & other)
{
    return self.lbound > other.lbound;
}

template <int D>
bool operator<(const kNNResultTuple<D> & self, const kNNResultTuple<D> & other)
{
    return self.distance < other.distance;
}

template <int D>
bool operator==(const kNNResultTuple<D> & self, const kNNResultTuple<D> & other)
{
    return self.p == other.p && self.distance == other.distance;
}

// Hack borrowed from: https://stackoverflow.com/questions/1185252/is-there-a-way-to-access-the-underlying-container-of-stl-container-adaptors
template <class T, class S, class C>
S & Container(std::priority_queue<T, S, C> & q)
{
    struct HackedQueue : private std::priority_queue<T, S, C>
    {
        static S & Container(std::priority_queue<T, S, C> & q) { return q.*&HackedQueue::c; }
    };
    return HackedQueue::Container(q);
}

template <int D>
kNNResult<D> SATree<D>::nearest_neighbour_search(Point<D> query, int k)
{
    if (!k) // Early exit
        return {};

    auto dist_q_a = distance(root->point, query);

    // Min queue by lower bound on distance from the `query`
    std::priority_queue<PromisingSubtree<D>, std::vector<PromisingSubtree<D>>, std::greater<PromisingSubtree<D>>> q;
    q.push({root, std::max(0.0f, dist_q_a - root->covering_radius), 0});

    std::priority_queue<kNNResultTuple<D>> result;
    float radius = INFINITY;

    while (!q.empty())
    {
        auto [b, lbound, digression] = q.top();
        q.pop();

        // No more good answers
        if (lbound > radius)
            return Container(result);

        result.push({b->point, distance(b->point, query)});

        if (result.size() > k)
            result.pop();
        else if (result.size() == k)
            radius = result.top().distance;

        // closest point to query among b U N(b)
        Point<D> c{b->point};
        float closest_neighbour_distance = distance(b->point, query);
        for (auto n : b->neighbours)
        {
            auto d = distance(n->point, query);
            if (d < closest_neighbour_distance)
            {
                closest_neighbour_distance = d;
                c = n->point;
            }
        }

        for (auto n : b->neighbours)
        {
            auto new_digression = std::max(0.0f, digression + distance(query, n->point) - distance(query, root->point));
            PromisingSubtree<D> x{
                n,
                std::max(
                    {lbound, (distance(query, n->point) - distance(query, c)) / 2, new_digression,
                     distance(query, n->point) - n->covering_radius}),
                new_digression};
            q.push(x);
        }
    }
    return Container(result);
}
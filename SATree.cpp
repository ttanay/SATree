
#include "SATree.h"

std::string Point::to_string()
{
    std::string res = "(";
    for (int i = 0; i < v.size(); i++)
    {
        if (i)
            res += ", ";
        res += std::to_string(v[i]);
    }
    res += ")";
    return res;
}

bool operator==(const Point & self, const Point & other)
{
    for (int i = 0; i < self.v.size(); i++)
        if (self.v[i] != other.v[i])
            return false;
    return true;
}

float distance(Point p, Point q)
{
    float res{0};
    for (int i = 0; i < p.v.size(); i++)
        res += std::pow((p.v[i] - q.v[i]), 2);
    return std::sqrt(res);
}


SATreeNode::~SATreeNode()
{
    if (neighbours.empty())
        return;

    for (auto n : neighbours)
        if (n)
            delete n;
}


SATree::SATree(std::vector<Point> S)
{
    int root_idx = rand() % S.size();
    root = new SATreeNode(S[root_idx]);
    S.erase(S.begin() + root_idx);
    build(root, S);
}

// TODO: reuse distances

void SATree::build(SATreeNode * a, std::vector<Point> & S)
{
    auto distance_from_a = [&a](Point p, Point q) { return distance(p, a->point) < distance(q, a->point); };
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


SATree::~SATree()
{
    if (root)
        delete root;
}


std::optional<Point> SATree::range_search(Point query, float radius)
{
    return range_search(root, query, radius, 0);
}


std::optional<Point> SATree::range_search(SATreeNode * a, Point query, float radius, float digression)
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


struct PromisingSubtree
{
    SATreeNode * node;
    float lbound;
    float digression;

    friend bool operator>(const PromisingSubtree & self, const PromisingSubtree & other);
};


bool operator>(const PromisingSubtree & self, const PromisingSubtree & other)
{
    return self.lbound > other.lbound;
}


bool operator<(const kNNResultTuple & self, const kNNResultTuple & other)
{
    return self.distance < other.distance;
}


bool operator==(const kNNResultTuple & self, const kNNResultTuple & other)
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


kNNResult SATree::nearest_neighbour_search(Point query, int k)
{
    if (!k) // Early exit
        return {};

    auto dist_q_a = distance(root->point, query);

    // Min queue by lower bound on distance from the `query`
    std::priority_queue<PromisingSubtree, std::vector<PromisingSubtree>, std::greater<PromisingSubtree>> q;
    q.push({root, std::max(0.0f, dist_q_a - root->covering_radius), 0});

    std::priority_queue<kNNResultTuple> result;
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
        Point c{b->point};
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
            PromisingSubtree x{
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
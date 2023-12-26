#include <optional>
#include <string>
#include <vector>

struct Point
{
    int x;
    int y;

    bool operator==(Point other) { return x == other.x && y == other.y; }
    std::string to_string() { return "(" + std::to_string(x) + ", " + std::to_string(y) + ")"; }
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

class SATree
{
    SATreeNode * root;

    void build(SATreeNode * a, std::vector<Point> & S);
    void print(SATreeNode * node, int space);
    std::string to_string(SATreeNode * node);

    std::optional<Point> range_search(SATreeNode * node, Point query, float radius, float digression);

public:
    SATree(std::vector<Point> & S);
    ~SATree();

    void print() { return print(root, 0); }
    std::string to_string() { return to_string(root); }
    std::optional<Point> range_search(Point query, float radius);
};
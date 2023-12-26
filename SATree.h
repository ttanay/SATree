#include <string>
#include <vector>

struct Point
{
    int x;
    int y;
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

public:
    SATree(std::vector<Point> & S);
    ~SATree();

    void print() { return print(root, 0); }
    std::string to_string() { return to_string(root); }
};
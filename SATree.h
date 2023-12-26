#include <vector>

struct Point { int x; int y; };
using Points = std::vector<Point>;

struct SATreeNode {
    Point point;
    std::vector<SATreeNode *> neighbours;
    float covering_radius{0};

    SATreeNode(Point p): point(p) {}
    ~SATreeNode();
};

class SATree {
    void build(SATreeNode *a, std::vector<Point> &S);
public:
    // TODO: Make private
    SATreeNode * root;
    SATree(std::vector<Point> & S);
    ~SATree();
};
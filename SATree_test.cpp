#include "SATree.h"
#include <iostream>
#include <string>

void print(SATreeNode *node, int space)
{
    for(int i = 0; i < space; i++)
        std::cout << "  ";
    std::cout << "|- (" << node->point.x << ", " << node->point.y << ") ";
    std::cout << "|N(a)| = " << node->neighbours.size() << std::endl;
    for(auto n: node->neighbours)
        print(n, space+1);
}

std::string stringify(SATreeNode *node)
{
    std::string res;
    res += "{" + std::to_string(node->point.x) + ", " + std::to_string(node->point.y) + "}";
    if(node->neighbours.size() > 0) {
        res += "(";
        for(auto n: node->neighbours) {
            if(n != *(node->neighbours.begin()))
                res += ",";
            res += stringify(n);
        }
        res += ")";
    }
    return res;
}

int main(int argc, char **argv)
{
    Points input;
    input.push_back({3, 3});
    input.push_back({5, 3});
    input.push_back({2, 2});
    input.push_back({4, 4});

    SATree tree(input);
    print(tree.root, 0);
    std::cout << stringify(tree.root) << std::endl;
    return 0;
}
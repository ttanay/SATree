#include "SATree.h"
#include <iostream>
#include <string>

enum TestCaseStatus { PASS, FAIL };
struct TestCaseResult
{
    std::string name;
    TestCaseStatus status;
    std::string err;

    void display() {
        if(status == TestCaseStatus::PASS)
            std::cout << "TestCase \"" << name << "\": PASSED" << std::endl;
        else
            std::cout << "TestCase \"" << name << "\": FAILED with" << err << std::endl;
    }
};

TestCaseResult test_case1(const std::string &name)
{
    Points input {
        {3, 3},
        {5, 3},
        {2, 2},
        {4, 4}
    };
    SATree tree(input);
    std::string res = tree.to_string();
    /**
        |- (4, 4) |N(a)| = 2
            |- (3, 3) |N(a)| = 1
                |- (2, 2) |N(a)| = 0
            |- (5, 3) |N(a)| = 0
    */
    std::string expected = "{4, 4}({3, 3}({2, 2}),{5, 3})";
    if(res == expected)
        return {name, TestCaseStatus::PASS, ""};
    else {
        auto err = "Assert failed!\n Expected: " + expected + "; Got: " + res;
        return {name, TestCaseStatus::FAIL, err};
    }
}

TestCaseResult test_case2(const std::string &name) {
    Points input {
        {-3, -3},
        {-3, -4},
        {-5, -3},
        {-4, -4},
        {-1, -1}
    };
    SATree tree(input);
    auto res = tree.to_string();
    // Note: the {-5,-3} should be a child of {-3,-3} because it is the
    //  closest to the pivot(`a`) at that point in time
    /**
    |- (-1, -1) |N(a)| = 1
        |- (-3, -3) |N(a)| = 2
            |- (-3, -4) |N(a)| = 1
                |- (-4, -4) |N(a)| = 0
        |- (-5, -3) |N(a)| = 0

    */
    std::string expected = "{-1, -1}({-3, -3}({-3, -4}({-4, -4}),{-5, -3}))";
    if(res == expected)
        return {name, TestCaseStatus::PASS, ""};
    else {
        auto err = "Assert failed!\n Expected: " + expected + "; Got: " + res;
        return {name, TestCaseStatus::FAIL, err};
    }
}

int main(int argc, char **argv)
{
    test_case1("Basic tree construction").display();
    test_case2("Skipped neighbour").display();
    return 0;
}
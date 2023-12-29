#include <cmath>
#include <iostream>
#include <string>
#include <vector>

#include "Reader.h"
#include "SATree.h"

enum TestCaseStatus
{
    PASS,
    FAIL
};
struct TestCaseResult
{
    std::string name;
    TestCaseStatus status;
    std::string err;

    void display()
    {
        if (status == TestCaseStatus::PASS)
            std::cout << "TestCase \"" << name << "\": PASSED" << std::endl;
        else
            std::cout << "TestCase \"" << name << "\": FAILED with " << err << std::endl;
    }
};

TestCaseResult test_case3(const std::string & name)
{
    Points input{{0, {-3, -3}}, {1, {-3, -4}}, {2, {-5, -3}}, {3, {-4, -4}}, {4, {-1, -1}}};
    SATree tree(input);
    auto res = tree.range_search({-1, {-6, -2}}, 2.0);
    Point expected{2, {-5, -3}};
    if (res && res.value() == expected)
    {
        return {name, TestCaseStatus::PASS, ""};
    }
    else if (!res)
        return {name, TestCaseStatus::FAIL, "returned null"};
    else
    {
        std::string err = "Expected: " + expected.to_string() + "; Got: " + res->to_string();
        return {name, TestCaseStatus::FAIL, err};
    }
}

TestCaseResult test_case4(const std::string & name)
{
    Points input{{0, {-3, -3}}, {1, {-3, -4}}, {2, {-5, -3}}, {3, {-4, -4}}, {4, {-1, -1}}};
    SATree tree(input);
    auto res = tree.range_search({-1, {-6, -2}}, 1.0);
    Point expected{2, {-5, -3}};
    if (!res)
    {
        return {name, TestCaseStatus::PASS, ""};
    }
    else
    {
        std::string err = "Expected: " + expected.to_string() + "; Got: " + res->to_string();
        return {name, TestCaseStatus::FAIL, err};
    }
}

TestCaseResult test_case5(const std::string & name)
{
    Points input{{0, {1, 1}}, {1, {3, 3}}, {2, {5, 3}}, {3, {3, 4}}, {4, {6, 4}}, {5, {-3, -3}}, {6, {-3, -4}}, {7, {-5, -3}}, {8, {-4, -4}}, {9, {-1, -1}}};
    SATree tree(input);
    auto res = tree.nearest_neighbour_search({-1, {-4, -5}}, 2);

    kNNResult expected{
        {{6, {-3, -4}}, (float)std::sqrt(2.0)},
        {{8, {-4, -4}}, 1.0}};

    if (res.size() != 2)
    {
        std::string err = "Expected size: 2; Got: " + std::to_string(res.size());
        return {name, TestCaseStatus::FAIL, err};
    }

    if (res != expected)
    {
        std::string err = "Expected: [";
        for (auto i : expected)
            err += i.to_string();
        err += "]; Got: [";
        for (auto i : res)
            err += i.to_string();
        err += "]";
        return {name, TestCaseStatus::FAIL, err};
    }

    return {name, TestCaseStatus::PASS, ""};
}

TestCaseResult test_case6(const std::string & name)
{
    Points input{{0, {1, 1}}, {1, {3, 3}}, {2, {5, 3}}, {3, {3, 4}}, {4, {6, 4}}, {5, {-3, -3}}, {6, {-3, -4}}, {7, {-5, -3}}, {8, {-4, -4}}, {9, {-1, -1}}};
    SATree tree(input);
    if (!tree.nearest_neighbour_search({-1, {-4, -5}}, 0).empty())
    {
        std::string err = "Expected empty result";
        return {name, TestCaseStatus::FAIL, err};
    }
    return {name, TestCaseStatus::PASS, ""};
}

TestCaseResult test_case7(const std::string & name)
{
    Points input{{0, {1, 1, 1}}, {1, {3, 3, 3}}, {2, {5, 3, 2}}, {3, {3, 4, 1}}, {4, {6, 4, 2}}};
    SATree tree(input);
    auto res = tree.nearest_neighbour_search({-1, {1, 1, 0}}, 1);

    kNNResult expected{{{-1, {1, 1, 1}}, 1.0}};

    if (res.size() != 1)
        return {name, TestCaseStatus::FAIL, "Result does not have 1 element"};

    if (res != expected)
    {
        std::string err = "Expected: [";
        for (auto i : expected)
            err += i.to_string();
        err += "]; Got: [";
        for (auto i : res)
            err += i.to_string();
        err += "]";
        return {name, TestCaseStatus::FAIL, err};
    }

    return {name, TestCaseStatus::PASS, ""};
}

int main(int argc, char ** argv)
{
    test_case3("Range search").display();
    test_case4("Range search null").display();
    test_case5("kNN Search").display();
    test_case6("kNN Search empty").display();
    test_case7("kNN 3-dimensions").display();
    return 0;
}
/*#include "node.hpp"
#include "read_line.hpp"
#include "read_token.hpp"*/
#include <assert.h>
#include <ctype.h>
#include <functional>
#include <iostream>
#include <stack>
#include <string>

enum class Tag {
    PRIOR,
    NON_PRIOR,
    NUMBER,
    ROOT
};

struct Token {

public:
    Token(Tag tag, std::function<double(double, double)> func)
        : tag(std::move(tag)), func(std::move(func))
    {
        assert(tag == Tag::PRIOR || tag == Tag::NON_PRIOR);
    }

    Token(const Tag& tag, double value)
        : tag(std::move(tag)), value(std::move(value))
    {
        assert(tag == Tag::NUMBER);
    }

    Token(const Tag& tag)
        : tag(std::move(tag))
    {
        assert(tag == Tag::ROOT);
    }


    bool isNumber() const { return tag == Tag::NUMBER; }
    bool isFunc() const { return tag == Tag::PRIOR || tag == Tag::NON_PRIOR; }
    bool isRoot() const { return tag == Tag::ROOT; }

    bool isPrior() const
    {
        assert(isFunc());
        return tag == Tag::PRIOR;
    }

    double getValue() const
    {
        assert(isNumber());
        return value;
    }

    double calcurate(const double& a, const double& b) const
    {
        assert(isFunc());
        return func(a, b);
    }

    double getAnswer(const double& b) const
    {
        assert(isRoot());
        return b;
    }

private:
    Tag tag;
    double value;
    std::function<double(double, double)> func;
};

bool isDigit(const char& c) { return int(c) >= int('0') && int(c) - int('0') <= 9; }

Token readNumber(const std::string& line, int& index)
{
    double number = 0;
    while (index < line.size() && isDigit(line.at(index))) {
        number = number * 10 + (int(line.at(index)) - int('0'));
        ++index;
    }
    double decimal = 0.1;
    if (index < line.size() && line.at(index) == '.') {
        ++index;
        while (index < line.size() && isDigit(line.at(index))) {
            number += (int(line.at(index)) - int('0')) * decimal;
            decimal *= 0.1;
            ++index;
        }
    }
    Token token{Tag::NUMBER, number};
    std::cerr << "number: " << number << " index: " << index << std::endl;
    return token;
}

Token readPlus(const std::string& line, int& index)
{
    ++index;
    Token token{Tag::NON_PRIOR,
        [](const auto& a, const auto& b) { return a + b; }};
    return token;
}

Token readMinus(const std::string& line, int& index)
{
    ++index;
    Token token{Tag::NON_PRIOR,
        [](const auto& a, const auto& b) { return a - b; }};
    return token;
}

Token readTimes(const std::string& line, int& index)
{
    ++index;
    Token token{Tag::PRIOR,
        [](const auto& a, const auto& b) { return a * b; }};
    return token;
}

Token readDividedBy(const std::string& line, int& index)
{
    ++index;
    Token token{Tag::PRIOR,
        [](const auto& a, const auto& b) { return a / b; }};
    return token;
}

struct Node {

    Node(Token token) : token(std::move(token)) {}
    ~Node() {}

    void setParent(Node* object) { this->parent = object; }
    void setLeft(Node* object) { this->left = object; }
    void setRight(Node* object) { this->right = object; }

    void insert(Node* object)
    {
        if (this->right != nullptr) {
            object->setLeft(this->right);
            this->right->setParent(object);
        }
        this->setRight(object);
        object->setParent(this);
    }

    double getValue()
    {
        if (token.isNumber()) {
            auto value = token.getValue();
            std::cerr << "NUMBER: " << value << std::endl;
            return value;
        } else if (token.isRoot()) {
            assert(right != nullptr);
            auto value = token.getAnswer(right->getValue());
            delete right;
            std::cerr << "ROOT: " << value << std::endl;
            return value;
        } else {
            assert(left != nullptr);
            assert(right != nullptr);
            auto left_value = left->getValue();
            delete left;
            auto right_value = right->getValue();
            delete right;
            auto value = token.calcurate(left_value, right_value);
            std::cerr << "OPERATOR: " << value << std::endl;
            return value;
        }
    }

private:
    Node* parent;
    Node* left;
    Node* right;
    Token token;
};

struct AdminTree {
public:
    AdminTree(Node* root)
        : root(std::move(root))
    {
        this->prior_insert_point = this->root;
        this->non_prior_insert_point = this->root;
    }

    void init()
    {
        this->prior_insert_point = this->root;
        this->non_prior_insert_point = this->root;
    }

    void startBracket()
    {
        brackets.push(prior_insert_point);
    }

    void endBracket()
    {
        assert(!brackets.empty());
        prior_insert_point = brackets.top();
        brackets.pop();
    }

    Node* getNonPriorInsertPoint()
    {
        return non_prior_insert_point = brackets.empty() ? root : brackets.top();
    }

    Node* getPriorInsertPoint() const
    {
        return prior_insert_point;
    }

    void setPriorInsertPoint(Node* node)
    {
        prior_insert_point = node;
    }

private:
    Node* root;
    Node* prior_insert_point;
    Node* non_prior_insert_point;
    std::stack<Node*> brackets;
};

void insertNode(Token token, AdminTree& admin_tree)
{
    Node* insert_point;
    if (token.isFunc() && !token.isPrior()) {
        insert_point = admin_tree.getNonPriorInsertPoint();
    } else if (token.isNumber() || token.isFunc()) {
        insert_point = admin_tree.getPriorInsertPoint();
    }
    Node* node = new Node{std::move(token)};
    insert_point->insert(node);
    if (token.isFunc()) {
        admin_tree.setPriorInsertPoint(node);
    }
}

void readLine(const std::string& line, AdminTree& admin_tree)
{
    int index = 0;
    while (index < line.size()) {
        if (isDigit(line.at(index))) {
            insertNode(std::move(readNumber(line, index)), admin_tree);
        } else if (line.at(index) == '+') {
            insertNode(std::move(readPlus(line, index)), admin_tree);
        } else if (line.at(index) == '-') {
            insertNode(std::move(readMinus(line, index)), admin_tree);
        } else if (line.at(index) == '*') {
            insertNode(std::move(readTimes(line, index)), admin_tree);
        } else if (line.at(index) == '/') {
            insertNode(std::move(readDividedBy(line, index)), admin_tree);
        } else if (line.at(index) == '(') {
            ++index;
            admin_tree.startBracket();
        } else if (line.at(index) == ')') {
            ++index;
            admin_tree.endBracket();
        } else {
            std::cout << "Invalid character: " << line.at(index) << std::endl;
        }
    }
}


int main()
{
    Token root_token{Tag::ROOT};
    Node* root = new Node{std::move(root_token)};

    AdminTree admin_tree{root};
    double answer;

    while (true) {
        std::string formula;
        std::cin >> formula;
        if (formula == "exit") {
            break;
        }
        readLine(formula, admin_tree);
        answer = root->getValue();
        std::cout << formula << " = " << answer << std::endl;
        admin_tree.init();
    }
    delete root;
    /*
    readLine("0+2", admin_tree);
    answer = root->getValue();
    std::cout << "answer: " << answer << std::endl;
    admin_tree.init();

    readLine("4.3+(1.3+2.5)*((1.23+2.21)*3.45+1.4*2.5)", admin_tree);
    answer = root->getValue();
    std::cout << "answer: " << answer << std::endl;
    admin_tree.init();
    */

    return 0;
}

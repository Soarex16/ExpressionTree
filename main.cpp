//
// Created by Soarex16 on 31.10.2018.
//

#include <iostream>
#include <string>
#include <vector>
#include "Expression.h"

using namespace std;

struct node {
    Expression::token t;
    node *l, *r;
};

/**
* Auxiliary data structure to determine the priorities of operators
*/
std::map<Expression::term, int> opPriority {
        {Expression::PLUS,  1},
        {Expression::MINUS, 1},
        {Expression::MUL,   2},
        {Expression::DIV,   2}
};

node* buildTree(node *t, vector<Expression::token *>& tokens);

int main() {
    string inputExpr;
    getline(cin, inputExpr);

    Expression e;
    e.tokenize(inputExpr);
    bool correct = e.verify();

    if (!correct) {
        cout << "Incorrect expression!";
        return -1;
    }

    node* tree = nullptr;

    vector<Expression::token*> tokens = e.getTokens();
    map<int, string> varDict = e.getVariables();

    tree = buildTree(tree, tokens);

    system("pause");
    return 0;
}

node* buildTree(node *t, vector<Expression::token *>& tokens) {
    if (tokens.size() == 1) {
        t = new node;
        t->l = nullptr;
        t->r = nullptr;
        t->t = *tokens[0];
    } else {
        // find last operator with lowest priority (outside the brackets)
        int brackets = 0, prior = 10, pos = -1;
        for (int i = 0; i < tokens.size(); ++i) {
            if (Expression::isOperator(tokens[i]->t) && brackets == 0) {
                if (opPriority[tokens[i]->t] <= prior) {
                    pos = i;
                }
            } else if (tokens[i]->t == Expression::BRACKET_L) {
                ++brackets;
            } else if (tokens[i]->t == Expression::BRACKET_R) {
                --brackets;
            }
        }

        // no operator found outside the brackets => discard the brackets
        if (pos == -1) {
            vector<Expression::token *> v(tokens.begin() + 1, tokens.end() - 1);
            buildTree(t, v);
        } else {
            t = new node;
            t->l = nullptr;
            t->r = nullptr;
            t->t = *tokens[pos];

            vector<Expression::token *> left(tokens.begin() + pos + 1, tokens.end());
            vector<Expression::token *> right(tokens.begin(), tokens.begin() + pos);

            t->l = buildTree(t->l, left);
            t->r = buildTree(t->r, right);
        }
    }

    return t;
}

//
// Created by Soarex16 on 31.10.2018.
//

#include <iostream>
#include <string>
#include <vector>
#include "Expression.h"

using namespace std;

long long counter = 0;

struct node {
    Expression::token t;
    node *l, *r;
};

node* buildTree(node *t, vector<std::shared_ptr<Expression::token>>& tokens);

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

    vector<std::shared_ptr<Expression::token>> tokens = e.getTokens();
    map<int, string> varDict = e.getVariables();

    tree = buildTree(tree, tokens);

    cout << "Total operations: " << counter << endl;

    system("pause");
    return 0;
}

/**
 * T[1] = 6
 * ~~~~ T[n] = (1 + 5) + n * (8 + [3, 2, 1, 1]) + 1 + [T(n-2), 2*T(n/2)]
 */
node* buildTree(node *t, vector<std::shared_ptr<Expression::token>>& tokens) {
    counter += 1;
    if (tokens.size() == 1) {
        counter += 5;
        t = new node;
        t->l = nullptr;
        t->r = nullptr;
        t->t = *tokens[0];
    } else {
        // find last operator with lowest priority (outside the brackets)
        int brackets = 0, prior = 10, pos = -1;
        for (int i = 0; i < tokens.size(); ++i) {
            counter += (4 + 2 + 2);
            if (Expression::isOperator(tokens[i]->t) && brackets == 0) {
                counter += 2;
                if (Expression::opPriority[tokens[i]->t] <= prior) {
                    counter += 1;
                    pos = i;
                }
            } else if (tokens[i]->t == Expression::BRACKET_L) {
                counter += 1;
                ++brackets;
            } else if (tokens[i]->t == Expression::BRACKET_R) {
                counter += 1;
                --brackets;
            }
        }

        counter += 1;
        // no operator found outside the brackets => discard the brackets
        if (pos == -1) {
            vector<std::shared_ptr<Expression::token>> v(tokens.begin() + 1, tokens.end() - 1);
            buildTree(t, v);
        } else {
            counter += 5;
            t = new node;
            t->l = nullptr;
            t->r = nullptr;
            t->t = *tokens[pos];

            vector<std::shared_ptr<Expression::token>> left(tokens.begin() + pos + 1, tokens.end());
            vector<std::shared_ptr<Expression::token>> right(tokens.begin(), tokens.begin() + pos);

            counter += 2;
            t->l = buildTree(t->l, left);
            t->r = buildTree(t->r, right);
        }
    }

    return t;
}

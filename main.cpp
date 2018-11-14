//
// Created by Soarex16 on 31.10.2018.
//

#include <iostream>
#include <string>
#include <vector>
#include "Expression.h"

using namespace std;

struct node{
    string info;
    node *l, *r;
};


int main() {
    string s;
    getline(cin, s);

    Expression e;
    e.tokenize(s);
    e.infixToPostfix();

    node* tree;

    vector<Expression::token*> tokens = e.getTokens();
    map<int, string> varDict = e.getVariables();

    for (long i = tokens.size() - 1; i >= 0; --i) {
        //addToken(tree, *tokens[i], varDict);
    }

    //system("pause");
    return 0;
}
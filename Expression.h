//
// Created by Soarex16 on 01.11.2018.
//

#pragma once

#include <vector>
#include <map>
#include <string>

class Expression {
public:
    enum term {
        IDENTIFIER,
        NUMBER,
        PLUS = '+',
        MINUS = '-',
        MUL = '*',
        DIV = '/',
        BRACKET_L = '(',
        BRACKET_R = ')'
    };

    enum expressionForm {
        PREFIX,
        INFIX,
        POSTFIX
    };

    struct token {
        term t;
        int val;
    };

    Expression(Expression& expr);
    Expression & operator=(Expression& expr);

    Expression(Expression&& expr);
    Expression & operator=(Expression&& expr);

    void tokenize(std::string s);
    Expression infixToPostfix();

    int eval();
    bool verify();

    //bool isOperator(const term& t);
	/**
	 * Checks if a term is an operator
	 * @param t input term
	 * @return true - if operator, else - false
	 */
	static bool isOperator(const term &t) {
		switch (t) {
		case PLUS:
		case MINUS:
		case MUL:
		case DIV:
			return true;
		default:
			return false;
		}
	}

    const std::vector<token *> &getTokens() const;
    void setTokens(const std::vector<token *> &tokens);

    const std::map<int, std::string> &getVariables() const;
    void setVariables(const std::map<int, std::string> &variables);

    const std::map<std::string, int> &getVarValues() const;

    void setVarValues(const std::map<std::string, int> &varValues);

    std::string toString();

    ~Expression();

    Expression();

private:
	std::vector<token*> tokens;
	std::map<int, std::string> variables;
	std::map<std::string, int> varValues;
	expressionForm form;

	int parseNumber(std::string& s);
	std::string parseIdentifier(std::string& s);

	int applyOperator(int arg1, int arg2, term op);
};

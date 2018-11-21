//
// Created by Soarex16 on 01.11.2018.
//

#include "Expression.h"
#include <stack>

Expression::Expression() {}

/**
 * Copy constructor
 * @param expr - expression to be copied
 */
Expression::Expression(Expression &expr) :
        form(expr.form), tokens(expr.tokens), variables(expr.variables), varValues(expr.varValues) {}

Expression &Expression::operator=(Expression &expr) {
    form = expr.form;
    tokens = expr.tokens;
    variables = expr.variables;
    varValues = expr.varValues;

    return *this;
}

/**
 * Move constructor
 * @param expr - expression to be moved
 */
Expression::Expression(Expression &&expr) :
        form(expr.form), tokens(expr.tokens), variables(expr.variables), varValues(expr.varValues) {}

Expression &Expression::operator=(Expression &&expr) {
    form = expr.form;
    std::swap(tokens, expr.tokens);
    std::swap(variables, expr.variables);
    std::swap(varValues, expr.varValues);

    return *this;
}

/**
 * Recognizes a number from string by the form: <number> :: = <digit> | <digit> <number>
 * Note: Function absorbs the characters that form the number
 * @param s String to be checked
 * @return Number if the recognized an unsigned integer, else -1
 */
int Expression::parseNumber(std::string &s) {
    int num = 0, i, ch;

    for (i = 0; i < s.length(); ++i) {
        ch = s[i];
        if (!isdigit(ch)) {
            break;
        }

        num = num * 10 + (ch - '0');
    }

    if (i == 0) {
        return -1;
    }

    s = s.substr(i, s.length() - i);
    return num;
}

/**
 * Recognizes an identifier from string by the form: <identifier> := <letter>{<letter>|<digit>}
 * Note: Function absorbs the characters that form the identifier
 * @param s String to be checked
 * @return Recognized identifier, else empty string
 */
std::string Expression::parseIdentifier(std::string &s) {
    std::string id;
    int i;
    char chr;

    for (i = 0; i < s.length(); ++i) {
        chr = s[i];
        if (!isalnum(chr)) {
            break;
        }

        id += chr;
    }

    s = s.substr(i, s.length() - i);
    return id;
}

/**
 * Convert the input string representation of infix expression to a token sequence
 * Fills the table of identifiers and the table of their values
 * @param s String to be converted to token sequence
 */
void Expression::tokenize(std::string s) {
    long n = s.length();
    // current variable ID
    int varId = 0;
    char chr;
    token *currentTok;

    // clear class field before parsing new expression
    tokens.clear();
    variables.clear();
    varValues.clear();

    form = INFIX;

    for (long i = 0; i < n; ++i) {
        chr = s[0];
        currentTok = new token;
        switch (chr) {
            case '+':
                currentTok->t = PLUS;
                s = s.substr(1, s.length() - 1);
                break;
            case '-':
                currentTok->t = MINUS;
                s = s.substr(1, s.length() - 1);
                break;
            case '*':
                currentTok->t = MUL;
                s = s.substr(1, s.length() - 1);
                break;
            case '/':
                currentTok->t = DIV;
                s = s.substr(1, s.length() - 1);
                break;
            case '(':
                currentTok->t = BRACKET_L;
                s = s.substr(1, s.length() - 1);
                break;
            case ')':
                currentTok->t = BRACKET_R;
                s = s.substr(1, s.length() - 1);
                break;
            default:
                if (chr == '\0') {
                    continue;
                } else if (isspace(chr)) {
                    s = s.substr(1, s.length() - 1);
                    continue;
                } else if (isdigit(chr)) {
                    int num = parseNumber(s);
                    if (num != -1) {
                        currentTok->t = NUMBER;
                        currentTok->val = num;
                    }
                } else if (isalpha(chr)) {
                    std::string identifier = parseIdentifier(s);
                    currentTok->t = IDENTIFIER;
                    currentTok->val = varId;
                    ++varId;
                    variables[varId] = identifier;
                } else {
                    throw "Parse error! Unresolved symbol";
                }
        }

        tokens.push_back(currentTok);
    }
}

/**
 * Converts the input sequence of tokens from the infix form into the
 * reverse Polish notation using the sorting station algorithm
 * If expression not in infix form - doing nothing
 * @param infixSequence Token sequence to be converted
 * @return Token sequence in reverse Polish notation
 */
Expression Expression::infixToPostfix() {
    Expression e;
    if (form != INFIX) {
        return e;
    }

    /**
    * Auxiliary data structure to determine the priorities of operators
    */
    std::map<term, int> opPriority {
            {PLUS,  1},
            {MINUS, 1},
            {MUL,   2},
            {DIV,   2}
    };

    std::vector<token *> reversedSequence;
    std::stack<token *> tokenBuffer;

    for (long i = 0; i < tokens.size(); ++i) {
        token *tok = tokens[i];

        switch (tok->t) {
            case NUMBER:
            case IDENTIFIER: {
                reversedSequence.push_back(tok);
                break;
            }
            case PLUS:
            case MINUS:
            case MUL:
            case DIV: {
                if (!tokenBuffer.empty()) {
                    while (!tokenBuffer.empty()) {
                        token *top = tokenBuffer.top();
                        if (opPriority[tok->t] <= opPriority[top->t]) {
                            reversedSequence.push_back(top);
                            tokenBuffer.pop();
                        } else {
                            break;
                        }
                    }
                }
                tokenBuffer.push(tok);
                break;
            }
            case BRACKET_L: {
                tokenBuffer.push(tok);
                break;
            }
            case BRACKET_R: {
                while (!tokenBuffer.empty() && tokenBuffer.top()->t != BRACKET_L) {
                    reversedSequence.push_back(tokenBuffer.top());
                    tokenBuffer.pop();
                }

                tokenBuffer.pop();
                break;
            }
        }
    }

    while (!tokenBuffer.empty()) {
        token *top = tokenBuffer.top();
        if (top->t == BRACKET_L || top->t == BRACKET_R) {
            throw "Error in bracket sequence! (ReversePolishNotation conversion)";
        } else {
            reversedSequence.push_back(top);
            tokenBuffer.pop();
        }
    }

    e.tokens = reversedSequence;
    e.form = POSTFIX;
    e.variables = variables;
    e.varValues = varValues;

    return e;
}

/**
 * Getters and setters for class fields
 */
const std::vector<Expression::token *> &Expression::getTokens() const {
    return tokens;
}

void Expression::setTokens(const std::vector<Expression::token *> &tokens) {
    Expression::tokens = tokens;
}

const std::map<int, std::string> &Expression::getVariables() const {
    return variables;
}

void Expression::setVariables(const std::map<int, std::string> &variables) {
    Expression::variables = variables;
}

const std::map<std::string, int> &Expression::getVarValues() const {
    return varValues;
}

void Expression::setVarValues(const std::map<std::string, int> &varValues) {
    Expression::varValues = varValues;
}

/**
 * Class destructor
 */
Expression::~Expression() {
    for (long i = 0; i < tokens.size(); ++i) {
        delete tokens[i];
    }
}

/**
 * Performs calculation of operator on the giver arguments
 * @param arg1 First operand
 * @param arg2 Second operand
 * @param op Operator term (PLUS, MINUS, MUL, DIV)
 * @return Calculation result of the operator
 */
int Expression::applyOperator(int arg1, int arg2, term op) {
    switch (op) {
        case PLUS:
            return arg1 + arg2;
        case MINUS:
            return arg1 - arg2;
        case MUL:
            return arg1 * arg2;
        case DIV:
            return arg1 / arg2;
        default:
            throw "Incorrect operator!";
    }
}

/**
 * Performs expression evaluation using the stack mechanism and postfix notation.
 * If expression in infix form - converts it to postfix, if in prefix - throws error
 * @param postfixSequence Token sequence in postfix form to be calculated
 * @param idTable Identifiers table for token sequence (can skip if expression does not contain variables)
 * @param idValues Values of all variables contained in expression(can skip if expression does not contain variables)
 * @return calculated value
 */
int Expression::eval() {
    Expression e(*this);
    if (form == PREFIX) {
        throw "Can't evaluate expression in prefix form!";
    } else if (form == INFIX) {
        e = infixToPostfix();
    }

    std::stack<token *> calculator;
    for (int i = 0; i < e.tokens.size(); ++i) {
        token *tok = e.tokens[i];

        switch (tok->t) {
            case NUMBER: {
                token *var = new token;
                var->t = NUMBER;
                var->val = tok->val;
                calculator.push(var);
                break;
            }
            case IDENTIFIER: {
                if (e.variables.empty() || e.varValues.empty()) {
                    throw "Expression contains variables, but table of identifiers or the table of their values!";
                }

                token *var = new token;
                var->t = NUMBER;
                var->val = varValues[variables[tok->val]];
                calculator.push(var);

                break;
            }
            case PLUS:
            case MINUS:
            case MUL:
            case DIV: {
                if (calculator.size() >= 2) {
                    token *arg1 = calculator.top();
                    if (arg1->t != NUMBER) {
                        throw "There is operator on top of stack!";
                    }

                    calculator.pop();
                    token *arg2 = calculator.top();
                    if (arg2->t != NUMBER) {
                        throw "There is operator on top of stack!";
                    }

                    int res = applyOperator(arg1->val, arg2->val, tok->t);
                    calculator.top()->val = res;
                } else {
                    throw "Expression error! Not enough operands to perform calculation!";
                }
                break;
            }
        }
    }

    return calculator.top()->val;
}

/**
 * Checks the correctness of the expression in postfix form
 * If expression in infix form - converts it to postfix
 * @return true - if correct, false if expression invalid of in prefix form
 */
bool Expression::verify() {
    Expression e(*this);
    if (form == PREFIX) {
        return false;
    } else if (form == INFIX) {
        e = infixToPostfix();
    }

    std::stack<token *> calculator;
    for (int i = 0; i < e.tokens.size(); ++i) {
        token *tok = e.tokens[i];

        switch (tok->t) {
            case NUMBER:
            case IDENTIFIER: {
                calculator.push(tok);
                break;
            }
            case PLUS:
            case MINUS:
            case MUL:
            case DIV: {
                if (calculator.size() >= 2) {
                    token *arg1 = calculator.top();
                    if (arg1->t != NUMBER && arg1->t != IDENTIFIER) {
                        return false;
                    }
                    calculator.pop();
                    token *arg2 = calculator.top();
                    if (arg2->t != NUMBER && arg2->t != IDENTIFIER) {
                        return false;
                    }
                } else {
                    return false;
                }
                break;
            }
        }
    }

    return true;
}

/**
 * Converts expression to string
 * @return string representation of expression
 */
std::string Expression::toString() {
    std::string result;

    for (long i = 0; i < tokens.size(); ++i) {
        term t = tokens[i]->t;

        switch (t) {
            case PLUS:
                result += "+ ";
                break;
            case MINUS:
                result += "- ";
                break;
            case MUL:
                result += "* ";
                break;
            case DIV:
                result += "/ ";
                break;
            case BRACKET_L:
                result += "( ";
                break;
            case BRACKET_R:
                result += ") ";
                break;
            case IDENTIFIER:
                result += variables[tokens[i]->val] + ' ';
                break;
            case NUMBER:
                result += std::to_string(tokens[i]->val) + ' ';
                break;
        }
    }

    return result;
}

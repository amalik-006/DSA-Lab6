// DSA Lab  06
// Task: 01 Expression Conversion and Evaluation
// Adina Malik
// CMS Id: 540739
// Date: October 13th 2026

#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include <cmath>
#include <cstdlib>
#include <cctype>
using namespace std;

// Custom array-based Stack ADT
template <typename T>
class Stack {
private:
    static const int CAPACITY = 10000; // Maximum number of elements
    T items[CAPACITY];                 // Array used to store stack elements
    int topIndex;                      // Index of the top element

public:
    // Initially, the stack is empty
    Stack() : topIndex(-1) {}

    // Returns true if the stack has no elements
    bool isEmpty() const {
        return topIndex == -1;
    }

    // Returns true if the stack has reached its maximum capacity
    bool isFull() const {
        return topIndex == CAPACITY - 1;
    }

    // Returns the current number of elements
    int size() const {
        return topIndex + 1;
    }

    // Push inserts an element at the top of the stack
    void push(const T& value) {
        if (isFull())
            throw runtime_error("Stack overflow!");

        items[++topIndex] = value;
    }

    // Pop removes and returns the top element
    T pop() {
        if (isEmpty())
            throw runtime_error("Stack underflow!");

        return items[topIndex--];
    }

    // Peek returns the top element without removing it
    T peek() const {
        if (isEmpty())
            throw runtime_error("Stack is empty!");

        return items[topIndex];
    }
};

// Breaks an expression into numbers, operators and parentheses
vector<string> tokenize(const string& expr) {
    vector<string> tokens;
    size_t i = 0;

    while (i < expr.size()) {
        // Ignore spaces between tokens
        if (isspace(static_cast<unsigned char>(expr[i]))) {
            ++i;
            continue;
        }

        // Read a number, including multi-digit numbers and decimals
        if (isdigit(static_cast<unsigned char>(expr[i])) ||
            expr[i] == '.') {
            string number;
            bool hasDigit = false;
            bool hasDot = false;

            while (i < expr.size() &&
                   (isdigit(static_cast<unsigned char>(expr[i])) ||
                    expr[i] == '.')) {
                if (expr[i] == '.') {
                    // A number cannot contain more than one decimal point
                    if (hasDot)
                        throw runtime_error("Invalid number!");

                    hasDot = true;
                } else {
                    hasDigit = true;
                }

                number += expr[i++];
            }

            if (!hasDigit)
                throw runtime_error("Invalid number!");

            tokens.push_back(number);
        }
        // Recognize supported operators and parentheses
        else if (string("+-*/%^()").find(expr[i]) != string::npos) {
            tokens.push_back(string(1, expr[i]));
            ++i;
        }
        // Reject unsupported characters
        else {
            throw runtime_error(string("Invalid character: ") + expr[i]);
        }
    }

    return tokens;
}

// Checks whether a token is a supported operator
bool isOperator(const string& token) {
    return token.size() == 1 &&
           string("+-*/%^").find(token[0]) != string::npos;
}

// Checks whether a token represents a valid numeric value
bool isNumber(const string& token) {
    if (token.empty())
        return false;

    char* end;
    strtod(token.c_str(), &end);

    return end != token.c_str() && *end == '\0';
}

// Returns the precedence of each operator
// Higher precedence operators are evaluated first
int precedence(const string& op) {
    if (op == "^") return 3;
    if (op == "*" || op == "/" || op == "%") return 2;
    if (op == "+" || op == "-") return 1;

    return 0;
}

// Exponentiation is right-associative
bool isRightAssociative(const string& op) {
    return op == "^";
}

// Converts an infix expression into postfix notation
string infix_to_postfix(string expr) {
    vector<string> tokens = tokenize(expr);
    Stack<string> operators;
    vector<string> output;

    // True when the next token should be an operand
    bool expectOperand = true;

    for (const string& token : tokens) {
        if (isNumber(token)) {
            // Two operands cannot appear consecutively without an operator
            if (!expectOperand)
                throw runtime_error("Missing operator between operands!");

            output.push_back(token);
            expectOperand = false;
        }
        else if (token == "(") {
            if (!expectOperand)
                throw runtime_error("Missing operator before '('!");

            operators.push(token);
            expectOperand = true;
        }
        else if (token == ")") {
            if (expectOperand)
                throw runtime_error("Missing operand before ')'!");

            // Move operators to the output until '(' is found
            while (!operators.isEmpty() && operators.peek() != "(")
                output.push_back(operators.pop());

            if (operators.isEmpty())
                throw runtime_error("Mismatched parentheses!");

            operators.pop(); // Remove the opening parenthesis
            expectOperand = false;
        }
        else if (isOperator(token)) {
            if (expectOperand)
                throw runtime_error("Operator is missing a left operand!");

            // Pop operators with higher precedence
            // For equal precedence, pop if the current operator is
            // left-associative
            while (!operators.isEmpty() &&
                   isOperator(operators.peek()) &&
                   (precedence(operators.peek()) > precedence(token) ||
                    (precedence(operators.peek()) == precedence(token) &&
                     !isRightAssociative(token)))) {
                output.push_back(operators.pop());
            }

            operators.push(token);
            expectOperand = true;
        }
    }

    if (tokens.empty())
        throw runtime_error("Expression cannot be empty!");

    if (expectOperand)
        throw runtime_error("Expression ends with an operator!");

    // Move all remaining operators to the output
    while (!operators.isEmpty()) {
        string op = operators.pop();

        if (op == "(")
            throw runtime_error("Mismatched parentheses!");

        output.push_back(op);
    }

    // Join tokens with spaces for readable output
    string result;

    for (const string& token : output) {
        if (!result.empty())
            result += ' ';

        result += token;
    }

    return result;
}

// Converts infix to prefix
// First convert infix to postfix, then build prefix expressions
// by combining each operator with its left and right operands
string infix_to_prefix(string expr) {
    string postfix = infix_to_postfix(expr);
    vector<string> tokens = tokenize(postfix);
    Stack<string> expressions;

    for (const string& token : tokens) {
        if (isNumber(token)) {
            expressions.push(token);
        }
        else if (isOperator(token)) {
            // Pop right operand first, then left operand
            string right = expressions.pop();
            string left = expressions.pop();

            // Combine the operator and operands into prefix form
            expressions.push(token + " " + left + " " + right);
        }
        else {
            throw runtime_error("Invalid postfix expression!");
        }
    }

    // A valid expression must leave exactly one result
    if (expressions.size() != 1)
        throw runtime_error("Invalid expression!");

    return expressions.pop();
}

// Converts postfix to fully parenthesized infix
string postfix_to_infix(string expr) {
    vector<string> tokens = tokenize(expr);
    Stack<string> expressions;

    for (const string& token : tokens) {
        if (isNumber(token)) {
            expressions.push(token);
        }
        else if (isOperator(token)) {
            // Pop the right operand before the left operand
            string right = expressions.pop();
            string left = expressions.pop();

            // Parentheses preserve the original order of operations
            expressions.push("(" + left + " " + token + " " + right + ")");
        }
        else {
            throw runtime_error("Postfix must not contain parentheses!");
        }
    }

    if (expressions.size() != 1)
        throw runtime_error("Invalid postfix expression!");

    return expressions.pop();
}

// Performs the calculation for one binary operator
double applyOperator(double left, double right, const string& op) {
    if (op == "+") return left + right;
    if (op == "-") return left - right;
    if (op == "*") return left * right;

    if (op == "/") {
        // Prevent division by zero
        if (right == 0)
            throw runtime_error("Division by zero!");

        return left / right;
    }

    if (op == "%") {
        // fmod supports remainder calculations for floating-point values
        if (right == 0)
            throw runtime_error("Modulo by zero!");

        return fmod(left, right);
    }

    if (op == "^") {
        double result = pow(left, right);

        // Reject undefined or overflowing exponentiation results.
        if (!isfinite(result))
            throw runtime_error("Invalid exponentiation result!");

        return result;
    }

    throw runtime_error("Unknown operator!");
}

// Evaluates a postfix expression by scanning from left to right
double evaluate_postfix(string expr) {
    vector<string> tokens = tokenize(expr);
    Stack<double> values;

    for (const string& token : tokens) {
        if (isNumber(token)) {
            // Store operands on the stack
            values.push(strtod(token.c_str(), nullptr));
        }
        else if (isOperator(token)) {
            // The most recently pushed operand is the right operand
            double right = values.pop();
            double left = values.pop();

            values.push(applyOperator(left, right, token));
        }
        else {
            throw runtime_error("Postfix must not contain parentheses!");
        }
    }

    // A valid postfix expression must leave one final value
    if (values.size() != 1)
        throw runtime_error("Invalid postfix expression!");

    return values.pop();
}

// Evaluates a prefix expression by scanning from right to left
double evaluate_prefix(string expr) {
    vector<string> tokens = tokenize(expr);
    Stack<double> values;

    for (int i = static_cast<int>(tokens.size()) - 1; i >= 0; --i) {
        const string& token = tokens[i];

        if (isNumber(token)) {
            values.push(strtod(token.c_str(), nullptr));
        }
        else if (isOperator(token)) {
            // Scanning from right to left means left is popped first
            double left = values.pop();
            double right = values.pop();

            values.push(applyOperator(left, right, token));
        }
        else {
            throw runtime_error("Prefix must not contain parentheses!");
        }
    }

    if (values.size() != 1)
        throw runtime_error("Invalid prefix expression!");

    return values.pop();
}

// Main function
int main() {
    int choice;
    string expr;

    do {
        cout << "\n===== TASK 1: EXPRESSION OPERATIONS =====\n";
        cout << "1. Infix to Postfix\n";
        cout << "2. Infix to Prefix\n";
        cout << "3. Postfix to Infix\n";
        cout << "4. Evaluate Postfix\n";
        cout << "5. Evaluate Prefix\n";
        cout << "0. Exit\n";
        cout << "Enter choice: ";
        cin >> choice;

        // Stop the program when the user chooses 0
        if (choice == 0)
            break;

        // Remove the leftover newline before reading the expression
        cin.ignore(10000, '\n');

        cout << "Enter expression: ";
        getline(cin, expr);

        // Catch invalid expressions without terminating the program
        try {
            switch (choice) {
                case 1:
                    cout << "Postfix: " << infix_to_postfix(expr) << '\n';
                    break;

                case 2:
                    cout << "Prefix: " << infix_to_prefix(expr) << '\n';
                    break;

                case 3:
                    cout << "Infix: " << postfix_to_infix(expr) << '\n';
                    break;

                case 4:
                    cout << "Result: " << evaluate_postfix(expr) << '\n';
                    break;

                case 5:
                    cout << "Result: " << evaluate_prefix(expr) << '\n';
                    break;

                default:
                    cout << "Invalid menu choice!\n";
            }
        }
        catch (const exception& e) {
            cout << "Error: " << e.what() << '\n';
        }

    } while (true);

    cout << "Task 1 completed.\n";
    return 0;
}
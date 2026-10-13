// DSA Lab 06
// Task: 02 Expression Validators Using Stack
// Adina Malik
// CMS Id: 540739
// Date: October 13th 2026

#include <iostream>
#include <string>
#include <vector>
#include <cctype>
using namespace std;

// Custom array-based stack implementation
template <typename T>
class Stack {
private:
    static const int CAPACITY = 10000;
    T items[CAPACITY];
    int topIndex;

public:
    // Initially, the stack is empty
    Stack() : topIndex(-1) {}

    // Checks whether the stack has no elements
    bool isEmpty() const {
        return topIndex == -1;
    }

    // Adds an element to the top of the stack
    bool push(const T& value) {
        if (topIndex == CAPACITY - 1)
            return false;

        items[++topIndex] = value;
        return true;
    }

    // Removes and returns the top element
    bool pop(T& value) {
        if (isEmpty())
            return false;

        value = items[topIndex--];
        return true;
    }

    // Returns the top element without removing it
    bool peek(T& value) const {
        if (isEmpty())
            return false;

        value = items[topIndex];
        return true;
    }
};

// Checks whether an opening bracket matches a closing bracket
bool matches(char opening, char closing) {
    return (opening == '(' && closing == ')') ||
           (opening == '{' && closing == '}') ||
           (opening == '[' && closing == ']');
}

// Task 2A: Checks whether brackets are balanced and correctly nested
bool isValidBrackets(const string& s) {
    Stack<char> brackets;

    for (char ch : s) {
        // Push opening brackets onto the stack
        if (ch == '(' || ch == '{' || ch == '[') {
            if (!brackets.push(ch))
                return false;
        }
        // Every closing bracket must match the most recent opening one
        else if (ch == ')' || ch == '}' || ch == ']') {
            char opening;

            if (!brackets.pop(opening))
                return false;     // Closing bracket without an opening one

            if (!matches(opening, ch))
                return false;     // Incorrect bracket nesting
        }
    }

    // Any leftover opening brackets make the expression invalid
    return brackets.isEmpty();
}

// Checks whether a character can appear in a simplified tag name
bool isTagNameChar(char ch) {
    unsigned char c = static_cast<unsigned char>(ch);

    return isalnum(c) || ch == '-' || ch == '_' || ch == ':' || ch == '.';
}

// Extracts a tag name from a tag
string extractTagName(const string& tag) {
    size_t i = 1;

    // Skip the slash in a closing tag
    if (i < tag.size() && tag[i] == '/')
        ++i;

    // Read the tag name until whitespace, '/' or '>'
    size_t start = i;

    while (i < tag.size() && isTagNameChar(tag[i]))
        ++i;

    if (start == i)
        return "";

    return tag.substr(start, i - start);
}

// Task 2B: Validates simplified markup using a stack
// Returns true if valid; otherwise stores explanatory errors
bool validate_markup(const string& s, vector<string>& errors) {
    Stack<string> tags;
    errors.clear();

    size_t i = 0;

    while (i < s.size()) {
        // Ordinary text outside tags is allowed
        if (s[i] != '<') {
            ++i;
            continue;
        }

        // Find the end of the current tag
        size_t end = s.find('>', i + 1);

        if (end == string::npos) {
            errors.push_back("Missing '>' for tag starting at position " +
                             to_string(i) + ".");
            break;
        }

        string tag = s.substr(i, end - i + 1);

        // Empty tags and declarations are rejected by this simplified validator
        if (tag.size() < 3) {
            errors.push_back("Empty or malformed tag: " + tag);
            i = end + 1;
            continue;
        }

        // Reject comments and declarations because this lab validator
        // handles simplified element tags only
        if (tag[1] == '!' || tag[1] == '?') {
            errors.push_back("Comments and declarations are not supported: " + tag);
            i = end + 1;
            continue;
        }

        bool closing = tag[1] == '/';

        // A tag ending in '/>' is self-closing, e.g. <br/>
        size_t last = tag.size() - 2;
        while (last > 0 && isspace(static_cast<unsigned char>(tag[last])))
            --last;

        bool selfClosing = tag[last] == '/';

        string name = extractTagName(tag);

        if (name.empty()) {
            errors.push_back("Invalid tag name in: " + tag);
            i = end + 1;
            continue;
        }

        // Ensure the character after the tag name is allowed
        size_t nameEnd = 1;

        if (closing)
            nameEnd = 2;

        while (nameEnd < tag.size() - 1 &&
               isTagNameChar(tag[nameEnd]))
            ++nameEnd;

        if (nameEnd < tag.size() - 1) {
            char next = tag[nameEnd];

            if (!isspace(static_cast<unsigned char>(next)) &&
                next != '/' && next != '>') {
                errors.push_back("Malformed tag: " + tag);
                i = end + 1;
                continue;
            }
        }

        // Closing tags must match the most recent opening tag
        if (closing) {
            // A closing tag must not be self-closing
            if (selfClosing) {
                errors.push_back("Closing tag cannot be self-closing: " + tag);
                i = end + 1;
                continue;
            }

            string topTag;

            if (!tags.pop(topTag)) {
                errors.push_back("Unexpected closing tag: " + tag);
            }
            else if (topTag != name) {
                errors.push_back("Mismatched closing tag: " + tag +
                                 " (expected </" + topTag + ">).");
            }
        }
        // Opening tags are pushed unless they are self-closing
        else if (!selfClosing) {
            if (!tags.push(name)) {
                errors.push_back("Tag stack capacity exceeded.");
                break;
            }
        }

        i = end + 1;
    }

    // Any remaining opening tags were never closed
    string unclosed;

    while (tags.pop(unclosed)) {
        errors.push_back("Unclosed opening tag: <" + unclosed + ">.");
    }

    return errors.empty();
}

int main() {
    int choice;

    do {
        cout << "\n===== TASK 2: EXPRESSION VALIDATORS =====\n";
        cout << "1. Validate Brackets\n";
        cout << "2. Validate Markup\n";
        cout << "0. Exit\n";
        cout << "Enter choice: ";

        if (!(cin >> choice)) {
            cout << "Invalid input!\n";
            return 1;
        }

        if (choice == 0)
            break;

        cin.ignore(10000, '\n');

        string input;
        cout << "Enter expression or markup: ";
        getline(cin, input);

        if (choice == 1) {
            // Check the bracket expression.
            if (isValidBrackets(input))
                cout << "Result: Valid brackets.\n";
            else
                cout << "Result: Invalid brackets.\n";
        }
        else if (choice == 2) {
            // Check markup and display every detected error.
            vector<string> errors;

            if (validate_markup(input, errors)) {
                cout << "Result: Valid markup.\n";
            }
            else {
                cout << "Result: Invalid markup.\n";

                for (const string& error : errors)
                    cout << "- " << error << '\n';
            }
        }
        else {
            cout << "Invalid menu choice!\n";
        }

    } while (true);

    cout << "Task 2 completed.\n";
    return 0;
}
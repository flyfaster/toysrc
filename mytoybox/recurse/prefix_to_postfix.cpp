#include <iostream>
#include <stack>
#include <string>
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE prefix_to_postfix
#include <boost/test/unit_test.hpp>
#include <boost/test/results_reporter.hpp>

using namespace std;

// funtion to check if character is operator or not
bool isOperator(char currentChar)
{
    switch (currentChar) {
    case '+':
    case '-':
    case '*':
    case '/':
    case '^':
    case '%':
        return true;
    default:
        return false;
    }
}

// returns whether a `lOp` b `rOp` c == (a `lOp` b) `rOp` c
bool precedence(char leftOperator, char rightOperator)
{
    if ( leftOperator == '^' ) {
        return true;
    } else if ( rightOperator == '^' ) {
        return false;
    } else if ( leftOperator == '*' || leftOperator == '/' || leftOperator == '%' ) {
        return true;
    } else if ( rightOperator == '*' || rightOperator == '/' || rightOperator == '%' ) {
        return false;
    }

    return true;
}

// Convert prefix to Postfix expression
string preToPost(const std::string& pre_exp)
{
    stack<string> exp_stack;
    // length of expression
    int length = pre_exp.size();
    // reading from right to left
    for (int i = length - 1; i >= 0; i--)
    {
        if (std::isspace(pre_exp[i]))
            continue;

        if (isOperator(pre_exp[i]))
        {
            string op1 = exp_stack.top();
            exp_stack.pop();
            string op2 = exp_stack.top();
            exp_stack.pop();

            string post_exp = op1 + op2 + pre_exp[i];

            exp_stack.push(post_exp);
        }
        else
        {
            // push the operand to the stack
        	exp_stack.push(string(1, pre_exp[i]));
        }
    }
    // stack contains only the Postfix expression
    return exp_stack.top();
}

bool isOperand(char C)
{
	if(C >= '0' && C <= '9') return true;
	if(C >= 'a' && C <= 'z') return true;
	if(C >= 'A' && C <= 'Z') return true;
	return false;
}

// prefix:  - + A * B C / D + E F
// postfix: A B C * + D E F + / -

// The postfix string is accumulated in "postfix"; the function return
// value is the next character position in "prefix" to be examined.
int prefixToPostfix(const std::string& prefix, std::string& postfix)
{ // https://people.eecs.ku.edu/~jrmiller/Courses/268/Materials/RecursionPart2/PrefixToPostfix.html
    char firstChar = prefix[0];
    if (isOperand(firstChar))
    {
        postfix += firstChar;
        return 1;
    }
    // must be: <operator><prefix><prefix>
    int nextPrefix1 = prefixToPostfix(prefix.substr(1), postfix);
    int nextPrefix2 = prefixToPostfix(prefix.substr(nextPrefix1 + 1), postfix);
    postfix += firstChar; // the operator
    return nextPrefix1 + nextPrefix2 + 1;
}

// Convert postfix to Prefix expression
string postToPre(const std::string& post_exp)
{
    stack<string> exp_stack;
    for (char ch: post_exp)
    {
        if (std::isspace(ch))
        	continue;

        if (isOperator(ch))
        {
            // pop two operands from stack
            string op1 = exp_stack.top();
            exp_stack.pop();
            string op2 = exp_stack.top();
            exp_stack.pop();
            // concat the operands and operator
            string temp = ch + op2 + op1;
            // Push string temp back to stack
            exp_stack.push(temp);
        }
        else
        {
            // push the operand to the stack
        	exp_stack.push(string(1, ch));
        }
    }

    return exp_stack.top();
}

// Convert prefix to Infix expression
string prefix_to_infix(const std::string& pre_exp)
{
    stack<string> s;
    int length = pre_exp.size();
    // reading from right to left
    for (int i = length - 1; i >= 0; i--)
    {
        // check if symbol is operator
        if (isOperator(pre_exp[i]))
        {
            // pop two operands from stack
            string op1 = s.top();
            s.pop();
            string op2 = s.top();
            s.pop();
            // concat the operands and operator
            string temp = "(" + op1 + pre_exp[i] + op2 + ")";
            // Push string temp back to stack
            s.push(temp);
        }
        // if symbol is an operand
        else
        {
            // push the operand to the stack
            s.push(string(1, pre_exp[i]));
        }
    }
    // Stack now contains the Infix expression
    return s.top();
}

string postfix_to_infix(const std::string& exp)
{
    stack<string> s;
    for (int i = 0; exp[i] != '\0'; i++)
    {
        // Push operands
        if (isOperand(exp[i]))
        {
            string op(1, exp[i]);
            s.push(op);
        }
        // We assume that input is
        // a valid postfix and expect
        // an operator.
        else
        {
            string op1 = s.top();
            s.pop();
            string op2 = s.top();
            s.pop();
            s.push("(" + op2 + exp[i] + op1 + ")");
        }
    }
    // There must be a single element
    // in stack now which is the required
    // infix.
    return s.top();
}

double evaluatePrefix(const std::string& exprsn)
{
    stack<double> operands_stack;
    for (int j = exprsn.size() - 1; j >= 0; j--) {
        // Push operand to Stack
        // To convert exprsn[j] to digit subtract
        // '0' from exprsn[j].
        if (std::isspace(exprsn[j]))
        	continue;

        if (isOperand(exprsn[j]))
            operands_stack.push(exprsn[j] - '0');
        else {
            // Operator encountered
            // Pop two elements from Stack
            double o1 = operands_stack.top();
            operands_stack.pop();
            double o2 = operands_stack.top();
            operands_stack.pop();
            // Use switch case to operate on o1
            // and o2 and perform o1 O o2.
            switch (exprsn[j]) {
            case '+':
                operands_stack.push(o1 + o2);
                break;
            case '-':
                operands_stack.push(o1 - o2);
                break;
            case '*':
                operands_stack.push(o1 * o2);
                break;
            case '/':
                operands_stack.push(o1 / o2);
                break;
            }
        }
    }
    return operands_stack.top();
}

// https://csegeek.com/csegeek/view/tutorials/algorithms/stacks_queues/stk_que_part6.php
// returns the value when a specific operator
// operates on two operands
int eval(int op1, int op2, char operate) {
   switch (operate) {
      case '*': return op2 * op1;
      case '/': return op2 / op1;
      case '+': return op2 + op1;
      case '-': return op2 - op1;
      default : return 0;
   }
}

// evaluates the postfix operation
// this module neither supports multiple digit integers
// nor looks for valid expression
// However it can be easily modified and some additional
// code can be added to overcome the above mentioned limitations
// it's a simple function which implements the basic logic to
// evaluate postfix operations using stack
int evalPostfix(const std::string& postfix)
{
    stack<int> operands_stack;
    for (char ch: postfix)
    {
        if (std::isspace(ch))
        	continue;

        if (std::isdigit(ch))
        {
            // push the operand(digit) onto stack
            operands_stack.push(ch - '0');
        }
        else
        {
            // we saw an operator
            // pop off the top two operands from the
            // stack and evalute them using the current
            // operator
            int op1 = operands_stack.top();
            operands_stack.pop();
            int op2 = operands_stack.top();
            operands_stack.pop();
            int val = eval(op1, op2, ch);
            // push the value obtained after evaluating
            // onto the stack
            operands_stack.push(val);
        }
    }
    return operands_stack.top();
}

std::string infixToPostfix(const std::string& infix)
{
    std::stringstream postfix;
    std::stack<char> stack;
    stack.push('('); // Push a left parenthesis ‘(‘ onto the stack.

    for(std::size_t i = 0, l = infix.size(); i < l; ++i) {
        const char current = infix[i];

        if (isspace(current)) {
            continue;
        }
        // If it's a digit or '.' or a letter ("variables"), add it to the output
        else if(isalnum(current) || '.' == current) {
            postfix << current;
        }

        else if('(' == current) {
            stack.push(current);
        }

        else if(isOperator(current)) {
            char rightOperator = current;
            while(!stack.empty() && isOperator(stack.top()) && precedence(stack.top(), rightOperator)) {
                postfix << ' ' << stack.top();
                stack.pop();
            }
            postfix << ' ';
            stack.push(rightOperator);
        }

        // We've hit a right parens
        else if(')' == current) {
            // While top of stack is not a left parens
            while(!stack.empty() && '(' != stack.top()) {
                postfix << ' ' << stack.top();
                stack.pop();
            }
            if (stack.empty()) {
                throw std::runtime_error("missing left paren");
            }
            // Discard the left paren
            stack.pop();
            postfix << ' ';
        } else {
            throw std::runtime_error("invalid input character");
        }
    }

    // Started with a left paren, now close it:
    // While top of stack is not a left paren
    while(!stack.empty() && '(' != stack.top()) {
        postfix << ' ' << stack.top();
        stack.pop();
    }
    if (stack.empty()) {
        throw std::runtime_error("missing left paren");
    }
    // Discard the left paren
    stack.pop();

    // all open parens should be closed now -> empty stack
    if (!stack.empty()) {
        throw std::runtime_error("missing right paren");
    }

    return postfix.str();
}

BOOST_AUTO_TEST_CASE(check_prefix_to_postfix)
{
	string pre_exp = "*-A/BC-/AKL";
	std::string postfix;
	prefixToPostfix(pre_exp, postfix);
    BOOST_TEST_CONTEXT("verify prefix_to_postfix " << pre_exp)
    BOOST_CHECK_EQUAL(preToPost(pre_exp), postfix);
    std::cout << "prefix: " << pre_exp << "\n"
			<< "infix: " << prefix_to_infix(pre_exp) << "\n"
			<< "postfix: " << postfix << "\n"
			<< "infix: " << postfix_to_infix(postfix) << "\n\n";
    std::string infix_exp = "1+2*3-8/4+5";
    postfix = infixToPostfix(infix_exp);
    std::cout << infix_exp << "\n" << postfix << "\n";
    auto res= evalPostfix(postfix);
    std::cout << postfix << "=" << res << "\n";
    pre_exp = postToPre(postfix);
    std::cout << pre_exp << "\t= " << evaluatePrefix(pre_exp) << "\n";
}


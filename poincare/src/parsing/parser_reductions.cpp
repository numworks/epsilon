#include "parser.h"

#include <poincare/equal.h>
#include <poincare/store.h>
#include <poincare/power.h>
#include <poincare/addition.h>
#include <poincare/opposite.h>
#include <poincare/parenthesis.h>
#include <poincare/subtraction.h>
#include <poincare/division.h>
#include <poincare/square_root.h>
#include <poincare/factorial.h>

namespace Poincare {

static void nopReduction(Parser::ExpressionStack * stack, const Token & token) {
}

static void equalReduction(Parser::ExpressionStack * stack, const Token & token) {
  Expression rightHandSide = stack->pop();
  stack->push(Equal(stack->pop(), rightHandSide));
}


// TODO
#if 0
static void stoReduction(Parser::ExpressionStack * stack, const Token & token) {
  Expression rightHandSide = stack->pop();
  stack->push(Store(stack->pop(), rightHandSide));
}
#endif

static void parenthesisReduction(Parser::ExpressionStack * stack, const Token & token) {
  stack->push(Parenthesis(stack->pop()));
}

static void plusReduction(Parser::ExpressionStack * stack, const Token & token) {
  assert(stack->size() >= 2);
  Expression rightHandSide = stack->pop();
  stack->push(Addition(stack->pop(), rightHandSide));
}

void minusReduction(Parser::ExpressionStack * stack, const Token & token) {
  if (Parser::TokenHasTag(token, Parser::TokenTag::UnaryMinus)) {
    stack->push(Opposite(stack->pop()));
  } else {
    Expression rightHandSide = stack->pop();
    stack->push(Subtraction(stack->pop(), rightHandSide));
  }
}

static void timesReduction(Parser::ExpressionStack * stack, const Token & token) {
  Expression rightHandSide = stack->pop();
  stack->push(Multiplication(stack->pop(), rightHandSide));
}

static void slashReduction(Parser::ExpressionStack * stack, const Token & token) {
  Expression rightHandSide = stack->pop();
  stack->push(Division(stack->pop(), rightHandSide));
}

static void powerReduction(Parser::ExpressionStack * stack, const Token & token) {
  Expression rightHandSide = stack->pop();
  stack->push(Power(stack->pop(), rightHandSide));
}

static void squareRootReduction(Parser::ExpressionStack * stack, const Token & token) {
  stack->push(SquareRoot(stack->pop()));
}

static void numberReduction(Parser::ExpressionStack * stack, const Token & token) {
  stack->push(token.expression());
}

static void identifierReduction(Parser::ExpressionStack * stack, const Token & token) {
  if (Parser::TokenHasTag(token, Parser::TokenTag::IdentifierIsFunction)) {
    //TODO
  }
  stack->push(token.expression());
}

static void commaReduction(Parser::ExpressionStack * stack, const Token & token) {
  //TODO, goes with functions
}

static void bangReduction(Parser::ExpressionStack * stack, const Token & token) {
  stack->push(Factorial(stack->pop()));
}

const Parser::Reduction Parser::sReductions[] = {
  &nopReduction,        // EndOfStream
  &equalReduction,      // Equal
  &nopReduction, //&stoReduction,        // Sto
  &nopReduction,        // RightBracket
  &parenthesisReduction,    // LeftBracket
  &nopReduction,        // RightBrace
  &parenthesisReduction,    // LeftBrace
  &nopReduction,        // RightParenthesis
  &parenthesisReduction,    // LeftParenthesis
  &plusReduction,       // Plus
  &minusReduction,      // Minus
  &timesReduction,      // Times
  &slashReduction,      // Slash
  &powerReduction,      // Power
  &squareRootReduction, // SquareRoot
  &bangReduction,       // Bang
  &numberReduction,     // Number
  &identifierReduction, // Identifier
  &commaReduction,      // Comma
  &nopReduction         // Undefined
};

}

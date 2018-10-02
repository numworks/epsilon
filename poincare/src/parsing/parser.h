#ifndef POINCARE_PARSING_PARSER_H
#define POINCARE_PARSING_PARSER_H

#include "tokenizer.h"

#include <poincare/addition.h>
#include <poincare/division.h>
#include <poincare/equal.h>
#include <poincare/factorial.h>
#include <poincare/multiplication.h>
#include <poincare/opposite.h>
#include <poincare/parenthesis.h>
#include <poincare/power.h>
#include <poincare/square_root.h>
#include <poincare/store.h>
#include <poincare/subtraction.h>
#include <poincare/symbol.h>
// matrix ? with brackets
// braces ?

namespace Poincare {

class Parser : public Tokenizer {
public:
  Parser(const char * input) :
    Tokenizer(input),
    m_currentToken(Token(Token::Type::Undefined)),
    m_nextToken(popToken()) {}
  Expression parse();

  Expression parseNumber(Expression leftHandSide);
  Expression parsePlus(Expression leftHandSide);
  Expression parseTimes(Expression leftHandSide);
  Expression parseSlash(Expression leftHandSide);
  Expression parseMinus(Expression leftHandSide);
  Expression parsePower(Expression leftHandSide);
  Expression parseLeftParenthesis(Expression leftHandSide);
  Expression parseSquareRoot(Expression leftHandSide);
  Expression parseBang(Expression leftHandSide);
  Expression parseEqual(Expression leftHandSide);
  Expression noParse(Expression leftHandSide);
private:
  Expression parseUntil(Token::Type stoppingType);
  bool canPopToken(Token::Type stoppingType);

  Token m_currentToken;
  Token m_nextToken;
};

}

#endif

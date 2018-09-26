#ifndef POINCARE_PARSING_PARSER_H
#define POINCARE_PARSING_PARSER_H

#include "tokenizer.h"
#include <poincare/expression.h>

#include <poincare/addition.h>
#include <poincare/division.h>
#include <poincare/equal.h>
#include <poincare/factorial.h>
#include <poincare/opposite.h>
#include <poincare/parenthesis.h>
#include <poincare/number.h>
#include <poincare/power.h>
#include <poincare/square_root.h>
#include <poincare/store.h>
#include <poincare/subtraction.h>

namespace Poincare {

class Parser : public Tokenizer {
public:
  Parser(const char * input) : Tokenizer(input) {}
  Expression parse();
private:
  Expression shift(Expression leftHandSide, Token lookahead, Token::Type stoppingType = Token::Type::EndOfStream);
  bool comparePrecedence(Token currentToken, Token::Type stoppingType) const;
};

}

#endif

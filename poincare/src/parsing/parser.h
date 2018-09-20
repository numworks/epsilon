#ifndef POINCARE_PARSING_PARSER_H
#define POINCARE_PARSING_PARSER_H

#include "tokenizer.h"
#include "stack.h"

namespace Poincare {

class Parser {
public:
  using ExpressionStack = Stack<Expression,100>;
  using TokenStack = Stack<Token,100>;

  Parser(const char * input) :
    m_tokenizer(input),
    m_lookahead(m_tokenizer.popToken()),
    m_tokenStack(),
    m_expressionStack()
  {}
  Expression parse();

  enum class TokenTag {
    None,
    UnaryMinus,
    IdentifierIsFunction
  };
  static void TokenSetTag(Token * t, TokenTag tag) {
    t->setTag(static_cast<int>(tag));
  }
  static bool TokenHasTag(const Token & t, TokenTag tag) {
    return t.tag() == static_cast<int>(tag);
  }

private:
  typedef void (*Reduction)(ExpressionStack * stack, const Token & token);
  static const Reduction sReductions[];

  void shift();
  bool needsReduction();
  void reduce();

  Tokenizer m_tokenizer;
  Token m_lookahead;
  TokenStack m_tokenStack;
  ExpressionStack m_expressionStack;
};

Expression Parse(const char * input);

}

#endif

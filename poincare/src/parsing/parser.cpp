#include "parser.h"

namespace Poincare {

static bool tokenTypesCanBeImplicitlyMultiplied(Token::Type t1, Token::Type t2) {
  return
    (t1 == Token::Type::RightParenthesis || t1 == Token::Type::Number || t1 == Token::Type::Identifier)
    &&
    (t2 == Token::Type::LeftParenthesis || t2 == Token::Type::Number || t2 == Token::Type::Identifier)
  ;
}

static bool minusTokenIsUnaryIfAfter(Token::Type t) {
  return false; //TODO
}

void Parser::reduce() {
  Token popped = m_tokenStack.pop();
  // if(popped.is(Token::Type::Identifier)) TODO
  if(tokenTypesCanBeImplicitlyMultiplied(popped.type(), m_lookahead.type())){
    m_tokenStack.push(Token(Token::Type::Times));
    // this makes implicit multiplication have higher precedence than division, power, square root
  }
  //if minusTokenIsUnaryIfAfter
  sReductions[static_cast<int>(popped.type())](&m_expressionStack, popped);
}

void Parser::shift() {
  m_tokenStack.push(m_lookahead);
  m_lookahead = m_tokenizer.popToken();
}

bool Parser::needsReduction() {
  /* This method compares the precedence of m_tokenStack.top() and m_lookahead.
   * Its return value decides whether to reduce m_tokenStack.
   * In addition to precedence comparison, the method may handle associativity of binary operators:
   *   1+1+1 yields 1+(1+1)
   *   2-5-7 yields (2-5)-7
   *   2/3/4 yields (2/3)/4
   * but this b  */
  //TODO opposite and power are not symmetric
  return (!m_lookahead.isLeftGroupingToken()) && (!m_tokenStack.isEmpty()) && (m_tokenStack.top().type() >= m_lookahead.type());
}

Expression Parser::parse(){
  while(!m_lookahead.isEndOfStream()) {
    shift();
    while (needsReduction()) {
      reduce();
    }
  }
  assert(m_expressionStack.size() == 1); // FIXME: Handle as parsing error
  return m_expressionStack.pop();
}

Expression Parse(const char * input){
  return Parser(input).parse();
}

}

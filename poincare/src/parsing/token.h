#ifndef POINCARE_PARSING_TOKEN_H
#define POINCARE_PARSING_TOKEN_H

#include <poincare/expression.h>

namespace Poincare {

class Token {
public:
  enum class Type : uint8_t { // Ordered from lower to higher precedence
    EndOfStream, // Must be the first
    Equal,
    Sto,
    RightBracket,
    LeftBracket,
    RightBrace,
    LeftBrace,
    RightParenthesis,
    LeftParenthesis,
    Plus,
    Minus, // Subtraction
    Times,
    Slash,
    // Opposite, unary
    Power,
    SquareRoot,
    // Implicit times: see Parser::reduce()
    Bang,
    Number,
    Identifier,
    Comma,
    Undefined
  };

  Token(Type type = Type::Undefined) : m_type(type), m_tag(0) {};

  Type type() const { return m_type; }
  bool is(Type t) const { return m_type == t; }
  bool isEndOfStream() const { return is(Type::EndOfStream); }
  bool isLeftGroupingToken() const {
    return is(Type::LeftBracket) || is(Type::LeftParenthesis) || is(Type::LeftBrace);
  }

  uint8_t tag() const { return m_tag; }
  void setTag(uint8_t t) { m_tag = t; }

  Expression expression() const { return m_expression; }
  void setExpression(Expression expression) { m_expression = expression; }

private:
  Type m_type;
  uint8_t m_tag;
  Expression m_expression;
};

}

#endif

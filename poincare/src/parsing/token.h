#ifndef POINCARE_PARSING_TOKEN_H
#define POINCARE_PARSING_TOKEN_H

#include <poincare/expression.h> // size_t

namespace Poincare {

class Token {
public:
  enum class Type {
    // Ordered from lower to higher precedence to make parser's job easier
    EndOfStream = 0, // Must be the first
    Equal,
    Store,
    RightBracket,
    RightParenthesis,
    RightBrace,
    Comma,
    Plus,
    Minus,
    Times,
    Slash,
    Power,
    SquareRoot,
    Bang,
    LeftBracket,
    LeftParenthesis,
    LeftBrace,
    Number,
    Identifier,
    Undefined
  };

  Token(Type type = Type::Undefined) : m_type(type) {};

  Type type() const { return m_type; }
  bool is(Type t) const { return m_type == t; }
  bool isEndOfStream() const { return is(Type::EndOfStream); }
  Expression expression() const { return m_expression; }
  void setExpression(Expression e) { m_expression = e; }
private:
  Type m_type;
  Expression m_expression;
};

}

#endif

#ifndef POINCARE_PARSING_TOKEN_H
#define POINCARE_PARSING_TOKEN_H

#include <poincare/expression.h> // size_t

namespace Poincare {

class Token {
public:
  enum class Type {
    // Ordered from lower to higher precedence to make parser's job easier
    EndOfStream, // Must be the first
    Equal,
    Store,
    RightBracket,
    LeftBracket,
    RightBrace,
    LeftBrace,
    RightParenthesis,
    LeftParenthesis,
    Plus,
    Minus,
    Times,
    Slash,
    Power,
    SquareRoot,
    Bang,
    Number,
    Identifier,
    Comma,
    Undefined
  };

  Token(Type type = Type::Undefined) : m_type(type) {};

  Type type() const { return m_type; }
  bool is(Type t) const { return m_type == t; }
  bool isEndOfStream() const { return is(Type::EndOfStream); }
  bool isLeftGroupingToken() const {
    return is(Type::LeftBracket) || is(Type::LeftParenthesis) || is(Type::LeftBrace);
  }
  bool isRightGroupingToken() const {
    return is(Type::RightBracket) || is(Type::RightParenthesis) || is(Type::RightBrace);
  }
  const char * text() const { return m_text; }
  void setText(const char * text) { m_text = text; }
  size_t length() const { return m_length; }
  void setLength(size_t length) { m_length = length; }

private:
  Type m_type;
  const char * m_text;
  size_t m_length;
};

}

#endif

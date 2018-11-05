#ifndef POINCARE_PARSING_TOKEN_H
#define POINCARE_PARSING_TOKEN_H

#include <poincare/expression.h>

/* The Token class is meant to be the output type of the Tokenizer.
 * While processing a text input, the Tokenizer indeed produces (pops)
 * the successive Tokens, that are then consumed by the Parser.
 * Each Token has a Type and may hold other relevant data intended for the Parser:
 *   - an Expression, specifically for a Number Token,
 *   - a string (m_text, m_length), specifically for an Identifier Token. */

namespace Poincare {

class Token {
public:
  enum Type {
    // Ordered from lower to higher precedence to make Parser's job easier
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
    ImplicitTimes,
      /* The ImplicitTimes Token allows to parse text where the Times Token is omitted.
       * Eventhough the Tokenizer will never pop ImplicitTimes Tokens,
       * the ImplicitTimes Token Type is defined here with the desired precedence,
       * in order to allow the Parser to insert such Tokens where needed. */
    Caret,
    Bang,
    LeftBracket,
    LeftParenthesis,
    LeftBrace,
    Empty,
    Constant,
    Number,
    Identifier,
    Undefined
  };

  Token(Type type) : m_type(type) {};

  Type type() const { return m_type; }
  bool is(Type t) const { return m_type == t; }
  bool isEndOfStream() const { return is(Type::EndOfStream); }

  Expression expression() const { return m_expression; }
  const char * text() const { return m_text; }
  size_t length() const { return m_length; }

  void setExpression(Expression e) { m_expression = e; }
  void setString(const char * text, size_t length) {
    m_text = text;
    m_length = length;
  }
  int compareTo(const char * name) const {
    // Compare m_text to name, similarly to strcmp, assuming
    //  - m_text is not null-terminated
    //  - name is.
    int diff = strncmp(m_text, name, m_length);
    return (diff != 0) ? diff : strcmp("", name + m_length);
  }
private:
  Type m_type;
  Expression m_expression;
  const char * m_text;
  size_t m_length;
};

}

#endif

#ifndef POINCARE_PARSING_TOKEN_H
#define POINCARE_PARSING_TOKEN_H

#include <poincare/expression.h>

namespace Poincare {

class Token {
public:
  enum Type {
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
    /* The ImplicitTimes Token allows to parse input text where the Times Token is omitted.
     * Eventhough the Tokenizer will never pop ImplicitTimes Tokens,
     * the ImplicitTimes Token Type is defined here with the desired precedence,
     * allowing the Parser to insert such Tokens where needed. */
    ImplicitTimes,
    Caret,
    Bang,
    LeftBracket,
    LeftParenthesis,
    LeftBrace,
    Number,
    Symbol,
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

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
    RightwardsArrow,
    Equal,
      /* Equal should have a higher precedence than RightArrow, because
       * Tokenizer::parseEqual looks for a right hand side until it finds a
       * token of lesser precedence than EqualSymbolAbstract, and this prevents
       * expressions such as "3=4>a". Tokenizer::parseRightArrow uses a special
       * algorithm that prevents (3>4=a). */
    RightSystemParenthesis,
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
    SingleQuote,
    Bang,
    CaretWithParenthesis,
    LeftBracket,
    LeftParenthesis,
    LeftBrace,
    LeftSystemParenthesis,
    Empty,
    Constant,
    Number,
    BinaryNumber,
    HexadecimalNumber,
    Unit,
    Identifier,
    Undefined
  };

  Token(Type type) : m_type(type), m_text(0), m_codePoint(UCodePointNull) {};

  Type type() const { return m_type; }
  bool is(Type t) const { return m_type == t; }
  bool isNumber() const { return m_type == Type::Number || m_type == Type::BinaryNumber || m_type == HexadecimalNumber; }
  bool isEndOfStream() const { return is(Type::EndOfStream); }

  Expression expression() const { return m_expression; }
  const char * text() const { return m_text; }
  size_t length() const { return m_length; }
  CodePoint codePoint() const { return m_codePoint; }

  void setExpression(Expression e) { m_expression = e; }
  void setString(const char * text, size_t length) {
    m_text = text;
    m_length = length;
  }
  void setCodePoint(CodePoint c) { m_codePoint = c; }
  static int CompareNonNullTerminatedName(const char * nonNullTerminatedName, size_t nonNullTerminatedNameLength, const char * nullTerminatedName) {
    /* Compare m_text to name, similarly to strcmp, assuming
     *  - m_text is not null-terminated
     *  - name is.*/
    int diff = strncmp(nonNullTerminatedName, nullTerminatedName, nonNullTerminatedNameLength);
    return (diff != 0) ? diff : strcmp("", nullTerminatedName + nonNullTerminatedNameLength);

  }
  int compareTo(const char * name) const {
    return CompareNonNullTerminatedName(m_text, m_length, name);
  }
private:
  Type m_type;
  Expression m_expression;
  const char * m_text;
  size_t m_length;
  CodePoint m_codePoint;
};

}

#endif

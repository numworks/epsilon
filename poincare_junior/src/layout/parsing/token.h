#ifndef POINCARE_JUNIOR_LAYOUT_PARSING_TOKEN_H
#define POINCARE_JUNIOR_LAYOUT_PARSING_TOKEN_H

/* The Token class is meant to be the output type of the Tokenizer.
 * While processing a text input, the Tokenizer indeed produces (pops)
 * the successive Tokens, that are then consumed by the Parser.
 * Each Token has a Type and may hold other relevant data intended for the Parser:
 *   - an Expression, specifically for a Number Token,
 *   - a string (m_text, m_length), specifically for an Identifier Token. */

#include <poincare_junior/src/memory/node.h>

namespace PoincareJ {

class Token {
public:
  enum class Type {
    // Ordered from lower to higher precedence to make Parser's job easier
    EndOfStream = 0, // Must be the first
    RightwardsArrow,
    AssignmentEqual,
    // RightSystemParenthesis,
    // RightSystemBrace,
    RightBracket,
    RightParenthesis,
    RightBrace,
    Comma,
    Nor,
    Xor,
    Or,
    Nand,
    And,
    Not,
    ComparisonOperator,
    NorthEastArrow,
    SouthEastArrow,
    Plus,
    Minus,
    Times,
    Slash,
    ImplicitTimes,
      /* The ImplicitTimes Token allows to parse text where the Times Token is omitted.
       * Eventhough the Tokenizer will never pop ImplicitTimes Tokens,
       * the ImplicitTimes Token Type is defined here with the desired precedence,
       * in order to allow the Parser to insert such Tokens where needed. */
    Percent,
    Caret,
    Bang,
    CaretWithParenthesis,
    ImplicitAdditionBetweenUnits,
    // ^ Used to parse 4h50min34s and other implicit additions of units
    LeftBracket,
    LeftParenthesis,
    LeftBrace,
    // LeftSystemParenthesis,
    // LeftSystemBrace,
    Empty,
    Constant,
    Number,
    BinaryNumber,
    HexadecimalNumber,
    Unit,
    ReservedFunction,
    SpecialIdentifier,
    CustomIdentifier,
    Layout,
    Undefined
  };

  Token(Type type = Type::Undefined) : m_type(type), m_firstLayout(), m_length(0) {};
  Token(Type type, Node layout, size_t length = 1) : m_type(type), m_firstLayout(layout), m_length(length) {};

  Type type() const { return m_type; }
  void setType(Type t) { m_type = t; }
  bool is(Type t) const { return m_type == t; }
  bool isNumber() const { return m_type == Type::Number || m_type == Type::BinaryNumber || m_type == Type::HexadecimalNumber; }
  bool isEndOfStream() const { return is(Type::EndOfStream); }

  Node firstLayout() const { return m_firstLayout; }
  size_t length() const { return m_length; }

  void setRange(Node firstLayout, size_t length) {
    m_firstLayout = firstLayout;
    m_length = length;
  }

private:
  Type m_type;
  Node m_firstLayout;
  size_t m_length;
};

}

#endif

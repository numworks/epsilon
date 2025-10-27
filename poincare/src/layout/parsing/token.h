#pragma once

/* The Token class is meant to be the output type of the Tokenizer.
 * While processing a text input, the Tokenizer indeed produces (pops)
 * the successive Tokens, that are then consumed by the Parser.
 * Each Token has a Type and a range (firstLayout, length). */

#include <poincare/src/layout/layout_span_decoder.h>
#include <poincare/src/layout/rack_layout_decoder.h>
#include <poincare/src/memory/tree.h>

namespace Poincare::Internal {

class Token {
 public:
  enum class Type : uint8_t {
    // Ordered from lower to higher precedence to make Parser's job easier
    EndOfStream = 0,  // Must be the first
    RightwardsArrow,
    AssignmentEqual,
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
    EuclideanDivision,
    Times,
    Slash,
    ImplicitTimes,
    /* The ImplicitTimes Token allows to parse text where the Times Token is
     * omitted. Eventhough the Tokenizer will never pop ImplicitTimes Tokens,
     * the ImplicitTimes Token Type is defined here with the desired precedence,
     * in order to allow the Parser to insert such Tokens where needed. */
    Percent,
    Caret,
    Bang,
    ImplicitAdditionBetweenUnits,
    // ^ Used to parse 4h50min34s and other implicit additions of units
    LeftBracket,
    LeftParenthesis,
    LeftBrace,
    Constant,
    Number,
    BinaryNumber,
    HexadecimalNumber,
    Unit,
    ReservedFunction,
    SpecialIdentifier,
    CustomIdentifier,
    Layout,
    Subscript,
    Superscript,
    PrefixSuperscript,
    Undefined
  };

  inline static Type TypeBefore(Type t) {
    return static_cast<Type>(static_cast<int>(t) - 1);
  }

  Token(Type type = Type::Undefined)
      : m_firstLayout(), m_length(0), m_type(type){};
  Token(Type type, const Layout* layout, size_t length = 1)
      : m_firstLayout(layout), m_length(length), m_type(type){};
  Token(Type type, LayoutSpan span)
      : m_firstLayout(span.data()), m_length(span.size()), m_type(type){};

  Type type() const { return m_type; }
  void setType(Type t) { m_type = t; }
  bool is(Type t) const { return m_type == t; }
  bool isNumber() const {
    return m_type == Type::Number || m_type == Type::BinaryNumber ||
           m_type == Type::HexadecimalNumber;
  }
  bool isEndOfStream() const { return is(Type::EndOfStream); }

  const Layout* firstLayout() const { return m_firstLayout; }
  size_t length() const { return m_length; }

  void setRange(const Layout* firstLayout, size_t length) {
    m_firstLayout = firstLayout;
    m_length = length;
  }

  LayoutSpan toSpan() const { return LayoutSpan(m_firstLayout, m_length); }

 private:
  const Layout* m_firstLayout;
  uint16_t m_length;
  Type m_type;
};

}  // namespace Poincare::Internal

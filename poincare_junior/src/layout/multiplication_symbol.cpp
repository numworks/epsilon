#include "multiplication_symbol.h"

#include <poincare_junior/src/expression/builtin.h>
#include <poincare_junior/src/expression/unit.h>

#include <algorithm>

namespace PoincareJ {

/* LayoutShape is used to check if the multiplication sign can be omitted
 * between two expressions. It depends on the "layout style" on the right of the
 * left expression. */
enum class LayoutShape {
  Decimal,
  Integer,
  OneLetter,
  MoreLetters,
  BoundaryPunctuation,  // ( [ ∫
  Brace,
  Root,
  NthRoot,
  Fraction,
  Default
};

using enum LayoutShape;

LayoutShape LeftLayoutShape(const Tree* expr) {
  const Builtin* builtin = Builtin::GetReservedFunction(expr);
  if (builtin && !builtin->has2DLayout()) {
    // This builtin will be displayed as : foobar()
    return MoreLetters;
  }
  if (expr->isLogicalOperator()) {
    return Default;
  }
  if (expr->isInteger()) {
    // assert(!m_negative)
    return Integer;
  }
  if (expr->isRational()) {
    // assert(!m_negative)
    return Fraction;
  }
  switch (expr->type()) {
    case BlockType::Abs:
    case BlockType::Ceiling:
    case BlockType::Floor:
    case BlockType::Binomial:
    case BlockType::ListSequence:
    case BlockType::Matrix:
    case BlockType::Point:
    case BlockType::Sum:
    case BlockType::Product:
    case BlockType::Norm:
    case BlockType::Parenthesis:  // TODO PCJ remove this one
      return BoundaryPunctuation;

    case BlockType::Addition:
      /* When beautifying a Multiplication of Additions, Parentheses are added
       * around Additions. */
      return BoundaryPunctuation;

#if O
    case BlockType::BasedInteger:
      return m_base == OMG::Base::Decimal ? Integer : Default;
#endif

    case BlockType::True:
    case BlockType::False:
    case BlockType::Infinity:
    case BlockType::Undefined:  // should be assert(false) ?
      return MoreLetters;

    case BlockType::Equal:
    case BlockType::NotEqual:
    case BlockType::InferiorEqual:
    case BlockType::Inferior:
    case BlockType::SuperiorEqual:
    case BlockType::Superior:
      return Default;

    case BlockType::Conjugate:
    case BlockType::Power:
    case BlockType::Factorial:
    case BlockType::PercentSimple:
    case BlockType::PercentAddition:  // is it true ?
    case BlockType::Subtraction:
      return LeftLayoutShape(expr->child(0));

    case BlockType::ComplexI:
    case BlockType::Pi:
    case BlockType::ExponentialE:
    case BlockType::PhysicalConstant:  // TODO not true for all constants
      return OneLetter;

    case BlockType::Decimal:
      // assert(!m_negative) for decimal
    case BlockType::SingleFloat:
    case BlockType::DoubleFloat:
      return Decimal;

    case BlockType::Dependency:
      // should be assert false ?
      return LeftLayoutShape(expr->child(0));

    case BlockType::Derivative:
      // why ? should be fraction ?
      return MoreLetters;

    case BlockType::Division:
      return Fraction;

#if O
    case BlockType::EmptyExpression:
      return OneLetter;
#endif

    case BlockType::Integral:
      return BoundaryPunctuation;

    case BlockType::List:
      return Brace;

    case BlockType::ListElement:
    case BlockType::ListSlice:
      return BoundaryPunctuation;

#if O  // TODO PCJ
    case BlockType::MixedFraction:
      return Integer;
#endif

    case BlockType::Multiplication:  // from NAry
      // should be assert(false) ?
      return LeftLayoutShape(expr->child(0));

    case BlockType::NthRoot:
      return NthRoot;

    case BlockType::SquareRoot:
      return Root;

    case BlockType::Opposite:
      // leftLayoutShape of Opposite is only called from Conjugate
      // assert(parent() && parent()->type() == Type::Conjugate);
      return OneLetter;

    case BlockType::Piecewise:
      return Default;

#if O
    case BlockType::RightwardsArrow:
      assert(false);
      return MoreLetters;
#endif

    case BlockType::UserSymbol:
    case BlockType::UserFunction:
    case BlockType::UserSequence:
      return OneLetter;  // TODO PCJ: or MoreLetters

    case BlockType::Unit:
      return OneLetter;  // had "TODO" in poincare

    default:
      return Default;
  }
}

LayoutShape RightLayoutShape(const Tree* expr) {
  const Builtin* builtin = Builtin::GetReservedFunction(expr);
  if (builtin && !builtin->has2DLayout()) {
    // This builtin will be displayed as : foobar()
    return BoundaryPunctuation;
  }
  switch (expr->type()) {
    case BlockType::Conjugate:
    case BlockType::Subtraction:
      return RightLayoutShape(expr->child(0));

    case BlockType::Dependency:
      // should be assert false ?
      // was not there
      return RightLayoutShape(expr->child(0));

    case BlockType::Derivative:
      return BoundaryPunctuation;

    case BlockType::Integral:
      return MoreLetters;

#if O  // TODO PCJ
    case BlockType::MixedFraction:
      return Fraction;
#endif

    case BlockType::Multiplication:  // from NAry
      // should be assert(false) ?
      return RightLayoutShape(expr->lastChild());

    case BlockType::NthRoot:
    case BlockType::SquareRoot:
      return Root;

    case BlockType::Opposite:
      return RightLayoutShape(expr->child(0));

    case BlockType::Factorial:
    case BlockType::Power:
    case BlockType::PercentSimple:
    case BlockType::PercentAddition:  // is it true ?
      return BoundaryPunctuation;

    case BlockType::UserFunction:
    case BlockType::UserSequence:
      return BoundaryPunctuation;

    case BlockType::UserSymbol:
      return LeftLayoutShape(expr);

    default:
      return LeftLayoutShape(expr);
  }
}

enum class Symbol : uint8_t {
  // The order matters !
  Empty = 0,
  MiddleDot = 1,
  MultiplicationSign = 2,
};

using enum Symbol;

CodePoint CodePointForOperatorSymbol(Symbol symbol) {
  switch (symbol) {
    case Empty:
      return UCodePointNull;
    case MiddleDot:
      return UCodePointMiddleDot;
    default:
      assert(symbol == MultiplicationSign);
      return UCodePointMultiplicationSign;
  }
}

// clang-format off
/* Operative symbol between two expressions depends on the layout shape on the
 * left and the right of the operator:
 *
 * Left  \ Right | Decimal | Integer | OneLetter | MoreLetters | BundaryPunct. | Root | NthRoot | Fraction | Unit |   Default
 * --------------+---------+---------+-----------+-------------+---------------+------+---------+----------+------+-------------
 * Decimal       |    ×    |    ×    |     ø     |      ×      |       ×       |  ×   |    ×    |    ×     |  ø   |      •
 * --------------+---------+---------+-----------+-------------+---------------+------+---------+----------+------+-------------
 * Integer       |    ×    |    ×    |     ø     |      •      |       ø       |  ø   |    •    |    ×     |  ø   |      •
 * --------------+---------+---------+-----------+-------------+---------------+------+---------+----------+------+-------------
 * OneLetter     |    ×    |    •    |     •     |      •      |       •       |  ø   |    •    |    ø     |  ø   |      •
 * --------------+---------+---------+-----------+-------------+---------------+------+---------+----------+------+-------------
 * MoreLetters   |    ×    |    •    |     •     |      •      |       •       |  ø   |    •    |    ø     |  ø   |      •
 * --------------+---------+---------+-----------+-------------+---------------+------+---------+----------+------+-------------
 * BundaryPunct. |    ×    |    ×    |     ø     |      ø      |       ø       |  ø   |    •    |    ×     |  ø   |      •
 * --------------+---------+---------+-----------+-------------+---------------+------+---------+----------+------+-------------
 * Brace         |    •    |    •    |     •     |      •      |       ×       |  •   |    •    |    •     |  ø   |      •
 * --------------+---------+---------+-----------+-------------+---------------+------+---------+----------+------+-------------
 * Root          |    ×    |    ×    |     ø     |      ø      |       ø       |  ø   |    •    |    ×     |  ø   |      •
 * --------------+---------+---------+-----------+-------------+---------------+------+---------+----------+------+-------------
 * Fraction      |    ×    |    ×    |     ø     |      ø      |       ø       |  ø   |    •    |    ×     |  ø   |      •
 * --------------+---------+---------+-----------+-------------+---------------+------+---------+----------+------+-------------
 * Default       |    •    |    •    |     •     |      •      |       •       |  •   |    •    |    •     |  ø   |      •
 *
 * Two Units are separated by a •, Unit on the left is treated according to its type
 * */
// clang-format on

Symbol OperatorSymbolBetween(LayoutShape left, LayoutShape right) {
  if (left == Default || right == Default || right == Brace) {
    return MiddleDot;
  }
  switch (left) {
    case Decimal:
      switch (right) {
        case OneLetter:
          return Empty;
        default:
          return MultiplicationSign;
      }
    case Integer:
      switch (right) {
        case Integer:
        case Decimal:
        case Fraction:
          return MultiplicationSign;
        case MoreLetters:
        case NthRoot:
          return MiddleDot;
        default:
          return Empty;
      }
    case OneLetter:
    case MoreLetters:
      switch (right) {
        case Decimal:
          return MultiplicationSign;
        case Fraction:
        case Root:
          return Empty;
        default:
          return MiddleDot;
      }
    case Brace:
      switch (right) {
        case BoundaryPunctuation:
          return MultiplicationSign;
        default:
          return MiddleDot;
      }
    // fall-through
    case BoundaryPunctuation:
    case Fraction:
    case Root:
      switch (right) {
        case Decimal:
        case Integer:
        case Fraction:
          return MultiplicationSign;
        case NthRoot:
          return MiddleDot;
        default:
          return Empty;
      }
    default:
      assert(false);
  }
}

CodePoint MultiplicationSymbol(const Tree* mult) {
  int sign = -1;
  int childrenNumber = mult->numberOfChildren();
  for (int i = 0; i < childrenNumber - 1; i++) {
    /* The operator symbol must be the same for all operands of the
     * multiplication. If one operator has to be '×', they will all be '×'. Idem
     * for '·'. */
    const Tree* left = mult->child(i);
    const Tree* right = mult->child(i + 1);
    Symbol symbol;
    if (Units::Unit::IsUnitOrPowerOfUnit(right)) {
      symbol = Units::Unit::IsUnitOrPowerOfUnit(left) ? MiddleDot : Empty;
    } else {
      symbol =
          OperatorSymbolBetween(RightLayoutShape(left), LeftLayoutShape(right));
    }
    sign = std::max(sign, static_cast<int>(symbol));
  }
  assert(sign >= 0);
  return CodePointForOperatorSymbol(static_cast<Symbol>(sign));
}

}  // namespace PoincareJ

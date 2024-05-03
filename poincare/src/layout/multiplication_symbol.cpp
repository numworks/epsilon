#include "multiplication_symbol.h"

#include <poincare/src/expression/builtin.h>
#include <poincare/src/expression/dependency.h>
#include <poincare/src/expression/unit.h>

#include <algorithm>

namespace Poincare::Internal {

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
  if (expr->isUndefined()) {
    // should be assert(false) ?
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
    case Type::Abs:
    case Type::Ceil:
    case Type::Floor:
    case Type::Binomial:
    case Type::ListSequence:
    case Type::Matrix:
    case Type::Point:
    case Type::Sum:
    case Type::Product:
    case Type::Norm:
    case Type::Parenthesis:  // TODO_PCJ remove this one
      return BoundaryPunctuation;

    case Type::Add:
      /* When beautifying a Multiplication of Additions, Parentheses are added
       * around Additions. */
      return BoundaryPunctuation;

#if O
    case Type::BasedInteger:
      return m_base == OMG::Base::Decimal ? Integer : Default;
#endif

    case Type::True:
    case Type::False:
    case Type::Inf:
      return MoreLetters;

    case Type::Equal:
    case Type::NotEqual:
    case Type::InferiorEqual:
    case Type::Inferior:
    case Type::SuperiorEqual:
    case Type::Superior:
      return Default;

    case Type::Conj:
    case Type::Pow:
    case Type::Fact:
    case Type::PercentSimple:
    case Type::PercentAddition:  // is it true ?
    case Type::Sub:
      return LeftLayoutShape(expr->child(0));

    case Type::ComplexI:
    case Type::Pi:
    case Type::EulerE:
    case Type::PhysicalConstant:  // TODO not true for all constants
      return OneLetter;

    case Type::Decimal:
      // assert(!m_negative) for decimal
    case Type::SingleFloat:
    case Type::DoubleFloat:
      return Decimal;

    case Type::Dependency:
      // should be assert false ?
      return LeftLayoutShape(Dependency::Main(expr));

    case Type::Diff:
      // why ? should be fraction ?
      return MoreLetters;

    case Type::Div:
      return Fraction;

#if O
    case Type::EmptyExpression:
      return OneLetter;
#endif

    case Type::Integral:
      return BoundaryPunctuation;

    case Type::List:
      return Brace;

    case Type::ListElement:
    case Type::ListSlice:
      return BoundaryPunctuation;

#if O  // TODO_PCJ
    case Type::MixedFraction:
      return Integer;
#endif

    case Type::Mult:  // from NAry
      // should be assert(false) ?
      return LeftLayoutShape(expr->child(0));

    case Type::Root:
      return NthRoot;

    case Type::Sqrt:
      return Root;

    case Type::Opposite:
      // leftLayoutShape of Opposite is only called from Conjugate
      // assert(parent() && parent()->type() == Type::Conj);
      return OneLetter;

    case Type::Piecewise:
      return Default;

#if O
    case Type::RightwardsArrow:
      assert(false);
      return MoreLetters;
#endif

    case Type::UserSymbol:
    case Type::UserFunction:
    case Type::UserSequence:
      return OneLetter;  // TODO_PCJ: or MoreLetters

    case Type::Unit:
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
    case Type::Conj:
    case Type::Sub:
      return RightLayoutShape(expr->child(0));

    case Type::Dependency:
      // should be assert false ?
      // was not there
      return RightLayoutShape(Dependency::Main(expr));

    case Type::Diff:
      return BoundaryPunctuation;

    case Type::Integral:
      return MoreLetters;

#if O  // TODO_PCJ
    case Type::MixedFraction:
      return Fraction;
#endif

    case Type::Mult:  // from NAry
      // should be assert(false) ?
      return RightLayoutShape(expr->lastChild());

    case Type::Root:
    case Type::Sqrt:
      return Root;

    case Type::Opposite:
      return RightLayoutShape(expr->child(0));

    case Type::Fact:
    case Type::Pow:
    case Type::PercentSimple:
    case Type::PercentAddition:  // is it true ?
      return BoundaryPunctuation;

    case Type::UserFunction:
    case Type::UserSequence:
      return BoundaryPunctuation;

    case Type::UserSymbol:
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

}  // namespace Poincare::Internal

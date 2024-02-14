#include "layout_shape.h"

#include <poincare_junior/src/expression/builtin.h>

namespace PoincareJ {

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
      // case Parenthesis
      return BoundaryPunctuation;

    case BlockType::Addition:
      /* When beautifying a Multiplication of Additions, Parentheses are added
       * around Additions. As leftLayoutShape is called after beautifying, we
       * should never call it on an Addition. */
      assert(false);

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

}  // namespace PoincareJ

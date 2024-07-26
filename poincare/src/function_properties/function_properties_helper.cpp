#include <poincare/function_properties_helper.h>
#include <poincare/src/expression/degree.h>
#include <poincare/src/expression/division.h>
#include <poincare/src/expression/simplification.h>
#include <poincare/src/expression/trigonometry.h>
#include <poincare/src/memory/n_ary.h>
#include <poincare/src/memory/pattern_matching.h>

#include "helper.h"

namespace Poincare {

using namespace Internal;

FunctionPropertiesHelper::LineType FunctionPropertiesHelper::PolarLineType(
    const SystemExpression& analyzedExpression, const char* symbol,
    ProjectionContext projectionContext) {
  assert(analyzedExpression.type() != ExpressionNode::Type::Dependency);
  assert(analyzedExpression.dimension(projectionContext.m_context).isScalar());

  /* Detect polar lines
   * 1/sinOrCos(theta + B) --> Line
   * 1/cos(theta) --> Vertical line
   * 1/cos(theta + pi/2) --> Horizontal line */

  const Tree* e = analyzedExpression.tree();
  if (!e->isMult() && !e->isPow()) {
    return LineType::None;
  }

  TreeRef numerator, denominator;
  Division::GetNumeratorAndDenominator(e, numerator, denominator);
  assert(numerator && denominator);
  double a, b, c;
  bool polarLine = Degree::Get(numerator, symbol, projectionContext) == 0 &&
                   DetectLinearPatternOfTrig(denominator, projectionContext,
                                             symbol, &a, &b, &c, false) &&
                   std::abs(b) == 1.0;
  numerator->removeTree();
  denominator->removeTree();

  if (polarLine) {
    assert(0.0 <= c && c <= 2 * M_PI);
    c = std::fmod(c, M_PI);
    if (c == 0.0) {
      return LineType::Vertical;
    }
    if (c == M_PI_2) {
      return LineType::Horizontal;
    }
    return LineType::Diagonal;
  }
  return LineType::None;
}

FunctionPropertiesHelper::LineType FunctionPropertiesHelper::ParametricLineType(
    const SystemExpression& analyzedExpression, const char* symbol,
    ProjectionContext projectionContext) {
  assert(analyzedExpression.type() == ExpressionNode::Type::Point);

  const Tree* xOfT = analyzedExpression.tree()->child(0);
  const Tree* yOfT = xOfT->nextTree();
  int degOfTinX = Degree::Get(xOfT, symbol, projectionContext);
  int degOfTinY = Degree::Get(yOfT, symbol, projectionContext);
  if (degOfTinX == 0) {
    if (degOfTinY == 0) {
      // The curve is a dot
      return LineType::None;
    }
    return LineType::Vertical;
  }
  if (degOfTinY == 0) {
    assert(degOfTinX != 0);
    return LineType::Horizontal;
  }
  if (degOfTinX == 1 && degOfTinY == 1) {
    return LineType::Diagonal;
  }
  assert(degOfTinX != 0 && degOfTinY != 0);
  if (degOfTinX != degOfTinY) {
    // quotient can't be independent on t
    return LineType::None;
  }
  /* Remove constant terms in x(t) and y(t) and build x(t) / y(t)
   * Reduction doesn't handle Division (removed at projection)
   * so we create x(t) * y(t)^-1 */
  Tree* quotient = SharedTreeStack->pushMult(2);
  Tree* variableX = xOfT->cloneTree();
  RemoveConstantTermsInAddition(variableX, symbol, projectionContext);
  SharedTreeStack->pushPow();
  Tree* variableY = yOfT->cloneTree();
  RemoveConstantTermsInAddition(variableY, symbol, projectionContext);
  (-1_e)->cloneTree();
  Simplification::ReduceSystem(quotient, false);
  bool diag = Degree::Get(quotient, symbol, projectionContext) == 0;
  quotient->removeTree();
  if (diag) {
    return LineType::Diagonal;
  }
  return LineType::None;
}

FunctionPropertiesHelper::FunctionType
FunctionPropertiesHelper::CartesianFunctionType(
    const SystemExpression& analyzedExpression, const char* symbol,
    ProjectionContext projectionContext) {
  assert(analyzedExpression.type() != ExpressionNode::Type::Dependency);
  assert(analyzedExpression.dimension(projectionContext.m_context).isScalar());

  const Tree* e = analyzedExpression.tree();

  // f(x) = piecewise(...)
  if (e->hasDescendantSatisfying(
          [](const Internal::Tree* t) { return t->isPiecewise(); })) {
    return FunctionType::Piecewise;
  }

  int xDeg = Degree::Get(e, symbol, projectionContext);
  // f(x) = a
  if (xDeg == 0) {
    return FunctionType::Constant;
  }

  // f(x) = a·x + b
  if (xDeg == 1) {
    // TODO: what if e is not an Add but is affine?
    return e->isAdd() ? FunctionType::Affine : FunctionType::Linear;
  }

  // f(x) = a·x^n + b·x^ + ... + z
  if (xDeg > 1) {
    return FunctionType::Polynomial;
  }

  // f(x) = a·logM(b·x+c) + d·logN(e·x+f) + ... + z
  if (IsLinearCombinationOfFunction(
          e, symbol, projectionContext,
          [](const Tree* e, const char* symbol,
             ProjectionContext projectionContext) {
            return e->isLogarithm() &&
                   Degree::Get(e->child(0), symbol, projectionContext) == 1;
          })) {
    return FunctionType::Logarithmic;
  }

  // f(x) = a·exp(b·x+c) + d·exp(e·x+f) + ... + z
  if (IsLinearCombinationOfFunction(
          e, symbol, projectionContext,
          [](const Tree* e, const char* symbol,
             ProjectionContext projectionContext) {
            return e->isExp() &&
                   Degree::Get(e->child(0), symbol, projectionContext) == 1;
          })) {
    return FunctionType::Exponential;
  }

  // f(x) = polynomial / polynomial
  if (IsLinearCombinationOfFunction(e, symbol, projectionContext,
                                    IsFractionOfPolynomials)) {
    return FunctionType::Rational;
  }

  // f(x) = cos(b·x+c) + sin(e·x+f) + tan(h·x+i) + ... + z
  Tree* clone = e->cloneTree();
  /* tan doesn't exist in system expressions
   * trig(A,1)/trig(A,0) -> tan(A) */
  PatternMatching::MatchReplace(
      clone,
      KAdd(KA_s,
           KMult(KB_s, KPow(KTrig(KC, 0_e), -1_e), KD_s, KTrig(KC, 1_e), KE_s),
           KF_s),
      KAdd(KA_s, KMult(KB_s, KD_s, KTan(KC), KE_s), KF_s));
  bool isTrig = IsLinearCombinationOfFunction(
      clone, symbol, projectionContext,
      [](const Tree* e, const char* symbol,
         ProjectionContext projectionContext) {
        return (e->isTrig() || e->isTan()) &&
               Degree::Get(e->child(0), symbol, projectionContext) == 1;
      });
  clone->removeTree();
  if (isTrig) {
    return FunctionType::Trigonometric;
  }

  return FunctionType::Default;
}

}  // namespace Poincare

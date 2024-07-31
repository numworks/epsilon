#include <poincare/function_properties/function_type.h>
#include <poincare/src/expression/degree.h>
#include <poincare/src/expression/division.h>
#include <poincare/src/expression/polynomial.h>
#include <poincare/src/expression/simplification.h>
#include <poincare/src/expression/trigonometry.h>
#include <poincare/src/memory/n_ary.h>
#include <poincare/src/memory/pattern_matching.h>

#include "helper.h"

namespace Poincare {

using namespace Internal;

FunctionType::LineType FunctionType::PolarLineType(
    const SystemExpression& analyzedExpression, const char* symbol) {
  assert(analyzedExpression.type() != ExpressionNode::Type::Dependency);
  assert(analyzedExpression.dimension().isScalar());

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
  bool polarLine =
      Degree::Get(numerator, symbol) == 0 &&
      DetectLinearPatternOfTrig(denominator, symbol, &a, &b, &c, false) &&
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

FunctionType::LineType FunctionType::ParametricLineType(
    const SystemExpression& analyzedExpression, const char* symbol) {
  assert(analyzedExpression.type() == ExpressionNode::Type::Point);

  const Tree* xOfT = analyzedExpression.tree()->child(0);
  const Tree* yOfT = xOfT->nextTree();
  int degOfTinX = Degree::Get(xOfT, symbol);
  int degOfTinY = Degree::Get(yOfT, symbol);
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
  RemoveConstantTermsInAddition(variableX, symbol);
  SharedTreeStack->pushPow();
  Tree* variableY = yOfT->cloneTree();
  RemoveConstantTermsInAddition(variableY, symbol);
  (-1_e)->cloneTree();
  Simplification::ReduceSystem(quotient, false);
  bool diag = Degree::Get(quotient, symbol) == 0;
  quotient->removeTree();
  if (diag) {
    return LineType::Diagonal;
  }
  return LineType::None;
}

bool isFractionOfPolynomials(const Tree* e, const char* symbol) {
  if (!e->isMult() && !e->isPow()) {
    return false;
  }
  TreeRef numerator, denominator;
  Division::GetNumeratorAndDenominator(e, numerator, denominator);
  assert(numerator && denominator);
  int numeratorDegree = Degree::Get(numerator, symbol);
  int denominatorDegree = Degree::Get(denominator, symbol);
  numerator->removeTree();
  denominator->removeTree();
  return denominatorDegree >= 0 && numeratorDegree >= 0;
}

typedef bool (*PatternTest)(const Tree*, const char*);

bool isLinearCombinationOfFunction(const Tree* e, const char* symbol,
                                   PatternTest testFunction) {
  if (testFunction(e, symbol) || Degree::Get(e, symbol) == 0) {
    return true;
  }
  if (e->isAdd()) {
    for (const Tree* child : e->children()) {
      if (!isLinearCombinationOfFunction(child, symbol, testFunction)) {
        return false;
      }
    }
    return true;
  }
  if (e->isMult()) {
    bool patternFound = false;
    for (const Tree* child : e->children()) {
      if (Degree::Get(child, symbol) == 0) {
        continue;
      }
      if (isLinearCombinationOfFunction(child, symbol, testFunction)) {
        if (patternFound) {
          return false;
        }
        patternFound = true;
      } else {
        return false;
      }
    }
    return patternFound;
  }
  return false;
}

FunctionType::CartesianType FunctionType::CartesianFunctionType(
    const SystemExpression& analyzedExpression, const char* symbol) {
  assert(analyzedExpression.type() != ExpressionNode::Type::Dependency);
  assert(analyzedExpression.dimension().isScalar());

  const Tree* e = analyzedExpression.tree();

  // f(x) = piecewise(...)
  if (e->hasDescendantSatisfying(
          [](const Internal::Tree* t) { return t->isPiecewise(); })) {
    return CartesianType::Piecewise;
  }

  int xDeg = Degree::Get(e, symbol);
  // f(x) = a
  if (xDeg == 0) {
    return CartesianType::Constant;
  }

  // f(x) = a·x + b
  if (xDeg == 1) {
    // TODO: what if e is not an Add but is affine?
    return e->isAdd() ? CartesianType::Affine : CartesianType::Linear;
  }

  // f(x) = a·x^n + b·x^ + ... + z
  if (xDeg > 1) {
    return CartesianType::Polynomial;
  }

  // f(x) = a·logM(b·x+c) + d·logN(e·x+f) + ... + z
  if (isLinearCombinationOfFunction(
          e, symbol, [](const Tree* e, const char* symbol) {
            return e->isLogarithm() && Degree::Get(e->child(0), symbol) == 1;
          })) {
    return CartesianType::Logarithmic;
  }

  // f(x) = a·exp(b·x+c) + d·exp(e·x+f) + ... + z
  if (isLinearCombinationOfFunction(
          e, symbol, [](const Tree* e, const char* symbol) {
            return e->isExp() && Degree::Get(e->child(0), symbol) == 1;
          })) {
    return CartesianType::Exponential;
  }

  // f(x) = polynomial / polynomial
  if (isLinearCombinationOfFunction(e, symbol, isFractionOfPolynomials)) {
    return CartesianType::Rational;
  }

  // f(x) = cos(b·x+c) + sin(e·x+f) + tan(h·x+i) + ... + z
  Tree* clone = e->cloneTree();
  /* tan doesn't exist in system expressions
   * trig(A,1)/trig(A,0) -> tan(A) */

  while (PatternMatching::MatchReplace(
      clone,
      KAdd(KA_s,
           KMult(KB_s, KPow(KTrig(KC, 0_e), -1_e), KD_s, KTrig(KC, 1_e), KE_s),
           KF_s),
      KAdd(KA_s, KMult(KB_s, KD_s, KTan(KC), KE_s), KF_s))) {
  }
  bool isTrig = isLinearCombinationOfFunction(
      clone, symbol, [](const Tree* e, const char* symbol) {
        return (e->isTrig() || e->isTan()) &&
               Degree::Get(e->child(0), symbol) == 1;
      });
  clone->removeTree();
  if (isTrig) {
    return CartesianType::Trigonometric;
  }

  return CartesianType::Default;
}

}  // namespace Poincare

#include <poincare/function_properties_helper.h>
#include <poincare/src/expression/degree.h>
#include <poincare/src/expression/division.h>
#include <poincare/src/expression/polynomial.h>
#include <poincare/src/expression/projection.h>
#include <poincare/src/expression/simplification.h>
#include <poincare/src/expression/trigonometry.h>
#include <poincare/src/memory/n_ary.h>
#include <poincare/src/memory/pattern_matching.h>

namespace Poincare {

using namespace Internal;

static inline double positiveModulo(double i, double n) {
  return std::fmod(std::fmod(i, n) + n, n);
}

// Detect a·cos(b·x+c) + k
bool detectLinearPatternOfTrig(const Tree* e,
                               ProjectionContext projectionContext,
                               const char* symbol, double* a, double* b,
                               double* c, bool acceptConstantTerm) {
  // TODO_PCJ: Trees need to be projected (for approx, and because we look for
  // Trig nodes)

  assert(a && b && c);

  // Detect a·cos(b·x+c) + d·cos(b·x+c) + k
  if (e->isAdd()) {
    *a = 0.0;
    bool cosFound = false;
    for (const Tree* child : e->children()) {
      double tempA, tempB, tempC;
      if (!detectLinearPatternOfTrig(child, projectionContext, symbol, &tempA,
                                     &tempB, &tempC, acceptConstantTerm)) {
        if (acceptConstantTerm &&
            Degree::Get(child, symbol, projectionContext) == 0) {
          continue;
        }
        return false;
      }
      if (cosFound) {
        if (tempB != *b || tempC != *c) {
          return false;
        }
      } else {
        cosFound = true;
        *b = tempB;
        *c = tempC;
      }
      *a += tempA;
    }
    return cosFound;
  }

  // Detect a·trig(b·x+c)
  if (e->isMult()) {
    assert(e->numberOfChildren() > 1);
    int indexOfCos = -1;
    for (IndexedChild<const Tree*> child : e->indexedChildren()) {
      if (detectLinearPatternOfTrig(child, projectionContext, symbol, a, b, c,
                                    false)) {
        indexOfCos = child.index;
        break;
      }
    }
    if (indexOfCos < 0) {
      return false;
    }
    Tree* eWithoutCos = e->cloneTree();
    NAry::RemoveChildAtIndex(eWithoutCos, indexOfCos);
    if (Degree::Get(eWithoutCos, symbol, projectionContext) != 0) {
      return false;
    }
    *a *= Approximation::To<double>(eWithoutCos);
    return true;
  }

  // Detect trig(b·x+c)
  if (e->isTrig()) {
    const Tree* child = e->child(0);
    assert(child->nextTree()->isZero() || child->nextTree()->isOne());
    bool isSin = child->nextTree()->isOne();
    if (Degree::Get(child, symbol, projectionContext) != 1) {
      return false;
    }

    Tree* coefList = PolynomialParser::GetCoefficients(child, symbol, false);
    assert(coefList->numberOfChildren() == 2);
    // b·x+c
    Tree* bTree = coefList->child(0);
    Tree* cTree = bTree->nextTree();
    assert(bTree && cTree);

    *a = 1.0;
    *b = Approximation::To<double>(bTree);
    *c = Approximation::To<double>(cTree) - isSin * M_PI_2;
    *c = positiveModulo(*c, 2 * M_PI);
    coefList->removeTree();
    return true;
  }

  return false;
}

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
                   detectLinearPatternOfTrig(denominator, projectionContext,
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

void removeConstantTermsInAddition(Tree* e, const char* symbol,
                                   ProjectionContext projectionContext) {
  if (!e->isAdd()) {
    return;
  }
  const int n = e->numberOfChildren();
  int nRemoved = 0;
  Tree* child = e->child(0);
  for (int i = 0; i < n; i++) {
    if (Degree::Get(child, symbol, projectionContext) == 0) {
      child->removeTree();
      nRemoved++;
    } else {
      child = child->nextTree();
    }
  }
  assert(nRemoved <= n);
  NAry::SetNumberOfChildren(e, n - nRemoved);
  NAry::SquashIfPossible(e);
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
  removeConstantTermsInAddition(variableX, symbol, projectionContext);
  SharedTreeStack->pushPow();
  Tree* variableY = yOfT->cloneTree();
  removeConstantTermsInAddition(variableY, symbol, projectionContext);
  (-1_e)->cloneTree();
  Simplification::ReduceSystem(quotient, false);
  bool diag = Degree::Get(quotient, symbol, projectionContext) == 0;
  quotient->removeTree();
  if (diag) {
    return LineType::Diagonal;
  }
  return LineType::None;
}

bool isFractionOfPolynomials(const Tree* e, const char* symbol,
                             ProjectionContext projectionContext) {
  if (!e->isMult() && !e->isPow()) {
    return false;
  }
  TreeRef numerator, denominator;
  Division::GetNumeratorAndDenominator(e, numerator, denominator);
  assert(numerator && denominator);
  int numeratorDegree = Degree::Get(numerator, symbol, projectionContext);
  int denominatorDegree = Degree::Get(denominator, symbol, projectionContext);
  numerator->removeTree();
  denominator->removeTree();
  return denominatorDegree >= 0 && numeratorDegree >= 0;
}

typedef bool (*PatternTest)(const Tree*, const char*, ProjectionContext);

bool isLinearCombinationOfFunction(const Tree* e, const char* symbol,
                                   ProjectionContext projectionContext,
                                   PatternTest testFunction) {
  if (testFunction(e, symbol, projectionContext) ||
      Degree::Get(e, symbol, projectionContext) == 0) {
    return true;
  }
  if (e->isAdd()) {
    for (const Tree* child : e->children()) {
      if (!isLinearCombinationOfFunction(child, symbol, projectionContext,
                                         testFunction)) {
        return false;
      }
    }
    return true;
  }
  if (e->isMult()) {
    bool patternFound = false;
    for (const Tree* child : e->children()) {
      if (Degree::Get(child, symbol, projectionContext) == 0) {
        continue;
      }
      if (isLinearCombinationOfFunction(child, symbol, projectionContext,
                                        testFunction)) {
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
  if (isLinearCombinationOfFunction(
          e, symbol, projectionContext,
          [](const Tree* e, const char* symbol,
             ProjectionContext projectionContext) {
            return e->isLogarithm() &&
                   Degree::Get(e->child(0), symbol, projectionContext) == 1;
          })) {
    return FunctionType::Logarithmic;
  }

  // f(x) = a·exp(b·x+c) + d·exp(e·x+f) + ... + z
  if (isLinearCombinationOfFunction(
          e, symbol, projectionContext,
          [](const Tree* e, const char* symbol,
             ProjectionContext projectionContext) {
            return e->isExp() &&
                   Degree::Get(e->child(0), symbol, projectionContext) == 1;
          })) {
    return FunctionType::Exponential;
  }

  // f(x) = polynomial / polynomial
  if (isLinearCombinationOfFunction(e, symbol, projectionContext,
                                    isFractionOfPolynomials)) {
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
  bool isTrig = isLinearCombinationOfFunction(
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

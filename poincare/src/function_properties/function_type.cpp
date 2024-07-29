#include <poincare/function_properties/function_type.h>
#include <poincare/src/expression/degree.h>
#include <poincare/src/expression/division.h>
#include <poincare/src/expression/polynomial.h>
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
bool FunctionType::DetectLinearPatternOfTrig(const Tree* e, const char* symbol,
                                             double* a, double* b, double* c,
                                             bool acceptConstantTerm) {
  // TODO_PCJ: Trees need to be projected (for approx, and because we look for
  // Trig nodes)

  assert(a && b && c);

  // Detect a·cos(b·x+c) + d·cos(b·x+c) + k
  if (e->isAdd()) {
    *a = 0.0;
    bool cosFound = false;
    for (const Tree* child : e->children()) {
      double tempA, tempB, tempC;
      if (!DetectLinearPatternOfTrig(child, symbol, &tempA, &tempB, &tempC,
                                     acceptConstantTerm)) {
        if (acceptConstantTerm && Degree::Get(child, symbol) == 0) {
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
      if (DetectLinearPatternOfTrig(child, symbol, a, b, c, false)) {
        indexOfCos = child.index;
        break;
      }
    }
    if (indexOfCos < 0) {
      return false;
    }
    Tree* eWithoutCos = e->cloneTree();
    NAry::RemoveChildAtIndex(eWithoutCos, indexOfCos);
    if (Degree::Get(eWithoutCos, symbol) != 0) {
      eWithoutCos->removeTree();
      return false;
    }
    *a *= Approximation::To<double>(eWithoutCos);
    eWithoutCos->removeTree();
    return true;
  }

  // Detect trig(b·x+c)
  if (e->isTrig()) {
    const Tree* child = e->child(0);
    assert(child->nextTree()->isZero() || child->nextTree()->isOne());
    bool isSin = child->nextTree()->isOne();
    if (Degree::Get(child, symbol) != 1) {
      return false;
    }

    Tree* coefList = PolynomialParser::GetCoefficients(child, symbol);
    assert(coefList->numberOfChildren() == 2);
    // b·x+c
    Tree* cTree = coefList->child(0);
    Tree* bTree = cTree->nextTree();
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

FunctionType::LineType FunctionType::PolarLineType(
    const SystemExpression& analyzedExpression, const char* symbol) {
  assert(analyzedExpression.type() != ExpressionNode::Type::Dependency);

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

void FunctionType::RemoveConstantTermsInAddition(Tree* e, const char* symbol) {
  if (!e->isAdd()) {
    return;
  }
  const int n = e->numberOfChildren();
  int nRemoved = 0;
  Tree* child = e->child(0);
  for (int i = 0; i < n; i++) {
    if (Degree::Get(child, symbol) == 0) {
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

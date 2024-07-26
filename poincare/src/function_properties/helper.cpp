#include "helper.h"

#include <poincare/src/expression/degree.h>
#include <poincare/src/expression/division.h>
#include <poincare/src/expression/polynomial.h>
#include <poincare/src/memory/n_ary.h>

namespace Poincare::Internal {

bool IsFractionOfPolynomials(const Tree* e, const char* symbol,
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

void RemoveConstantTermsInAddition(Tree* e, const char* symbol,
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

bool IsLinearCombinationOfFunction(const Tree* e, const char* symbol,
                                   ProjectionContext projectionContext,
                                   PatternTest testFunction) {
  if (testFunction(e, symbol, projectionContext) ||
      Degree::Get(e, symbol, projectionContext) == 0) {
    return true;
  }
  if (e->isAdd()) {
    for (const Tree* child : e->children()) {
      if (!IsLinearCombinationOfFunction(child, symbol, projectionContext,
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
      if (IsLinearCombinationOfFunction(child, symbol, projectionContext,
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

static inline double positiveModulo(double i, double n) {
  return std::fmod(std::fmod(i, n) + n, n);
}

// Detect a·cos(b·x+c) + k
bool DetectLinearPatternOfTrig(const Tree* e,
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
      if (!DetectLinearPatternOfTrig(child, projectionContext, symbol, &tempA,
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
      if (DetectLinearPatternOfTrig(child, projectionContext, symbol, a, b, c,
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

}  // namespace Poincare::Internal

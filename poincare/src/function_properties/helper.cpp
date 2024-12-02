#include "helper.h"

#include <poincare/function_properties/function_type.h>
#include <poincare/src/expression/approximation.h>
#include <poincare/src/expression/degree.h>
#include <poincare/src/expression/division.h>
#include <poincare/src/expression/polynomial.h>
#include <poincare/src/expression/simplification.h>
#include <poincare/src/expression/trigonometry.h>
#include <poincare/src/memory/n_ary.h>
#include <poincare/src/memory/pattern_matching.h>

namespace Poincare::Internal {

using namespace Internal;

double PositiveModulo(double i, double n) {
  return std::fmod(std::fmod(i, n) + n, n);
}

// Detect a·cos(b·x+c) + k
bool DetectLinearPatternOfTrig(const Tree* e, const char* symbol, double* a,
                               double* b, double* c, bool acceptConstantTerm) {
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
    *a *= Approximation::To<double>(eWithoutCos, Approximation::Parameters{});
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
    *b = Approximation::To<double>(bTree, Approximation::Parameters{});
    *c = Approximation::To<double>(cTree, Approximation::Parameters{}) -
         isSin * M_PI_2;
    *c = PositiveModulo(*c, 2 * M_PI);
    coefList->removeTree();
    return true;
  }

  return false;
}

void RemoveConstantTermsInAddition(Tree* e, const char* symbol) {
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

}  // namespace Poincare::Internal

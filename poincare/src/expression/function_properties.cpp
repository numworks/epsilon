#include "function_properties.h"

#include <poincare/src/memory/n_ary.h>

#include "degree.h"
#include "division.h"
#include "simplification.h"
#include "trigonometry.h"

namespace Poincare::Internal {

FunctionProperties::LineType FunctionProperties::PolarLineType(
    const SystemExpression& e, const char* symbol,
    ProjectionContext projectionContext) {
  /* Detect polar lines
   * 1/sinOrCos(theta + B) --> Line
   * 1/cos(theta) --> Vertical line
   * 1/cos(theta + pi/2) --> Horizontal line */

  const Tree* tree = e.tree();
  if (!tree->isMult() && !tree->isPow()) {
    return LineType::None;
  }

  TreeRef numerator, denominator;
  Division::GetNumeratorAndDenominator(tree, numerator, denominator);
  assert(numerator && denominator);
  double a, b, c;
  bool polarLine =
      Degree::Get(numerator, symbol, projectionContext) == 0 &&
      Trigonometry::DetectLinearPatternOfTrig(denominator, projectionContext,
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

FunctionProperties::LineType FunctionProperties::ParametricLineType(
    const SystemExpression& e, const char* symbol,
    ProjectionContext projectionContext) {
  const Tree* xOfT = e.tree()->child(0);
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

}  // namespace Poincare::Internal

#include "function_properties.h"

#include "degree.h"
#include "division.h"
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

}  // namespace Poincare::Internal

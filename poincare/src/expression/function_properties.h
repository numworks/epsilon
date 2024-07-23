#ifndef POINCARE_EXPRESSION_FUNCTION_PROPERTIES_H
#define POINCARE_EXPRESSION_FUNCTION_PROPERTIES_H

#include <poincare/old/junior_expression.h>
#include <poincare/src/memory/tree_ref.h>

#include "projection.h"

namespace Poincare::Internal {

class FunctionProperties {
 public:
  enum class LineType { Vertical, Horizontal, Diagonal, None };

  static LineType PolarLineType(const SystemExpression& e, const char* symbol,
                                ProjectionContext projectionContext);
  static LineType ParametricLineType(const SystemExpression& e,
                                     const char* symbol,
                                     ProjectionContext projectionContext);
};

}  // namespace Poincare::Internal

#endif

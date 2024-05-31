#ifndef POINCARE_JAVASCRIPT_EXPRESSION_WRAPPER_H
#define POINCARE_JAVASCRIPT_EXPRESSION_WRAPPER_H

#include <poincare/old/junior_expression.h>

#include "js_tree.h"

namespace Poincare {

namespace JSBridge {

/* Static functions bounded to JavaScript that apply expression methods
 * on JSTrees.
 * Ideally, we want each of these functions to exactly match one
 * JuniorExpression function
 * */

class Expression {
 public:
  static JSTree ParseLatex(std::string latex);
  static std::string ToLatex(const JSTree& jsTree);
  static JSTree CloneAndSimplify(const JSTree& jsTree);
  static double ApproximateToScalar(const JSTree& jsTree);
  static JSTree GetSystemFunction(const JSTree& jsTree, std::string var);
  static double ApproximateToScalarWithValue(const JSTree& jsTree, double val);
};

}  // namespace JSBridge
}  // namespace Poincare
#endif

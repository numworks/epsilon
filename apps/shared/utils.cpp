#include "utils.h"
#include <apps/exam_mode_configuration.h>
#include <poincare/unit.h>
#include "poincare_helpers.h"

using namespace Poincare;

namespace Shared {

namespace Utils {

template <class T>
T ValueOfFloatAsDisplayed(T t, int precision, Poincare::Context * context) {
  assert(precision <= PrintFloat::k_numberOfStoredSignificantDigits);
  constexpr size_t bufferSize = PrintFloat::charSizeForFloatsWithPrecision(PrintFloat::k_numberOfStoredSignificantDigits);
  char buffer[bufferSize];
  // Get displayed value
  size_t numberOfChar = PoincareHelpers::ConvertFloatToText<T>(t, buffer, bufferSize, precision);
  assert(numberOfChar <= bufferSize);
  // Silence compiler warnings for assert
  (void) numberOfChar;
  // Extract displayed value
  return PoincareHelpers::ApproximateToScalar<T>(buffer, context);
}


bool ShouldOnlyDisplayApproximation(Poincare::Expression input, Poincare::Expression exactOutput, Poincare::Context * context) {
    // Exact output with remaining dependency are not displayed to avoid 2 ≈ undef
  return exactOutput.type() == ExpressionNode::Type::Dependency
    // Lists or Matrices with only nonreal/undefined children
    || (exactOutput.isOfType({ExpressionNode::Type::List, ExpressionNode::Type::Matrix}) && exactOutput.allChildrenAreUndefined())
    // Force all outputs to be ApproximateOnly if required by the exam mode configuration
    || ExamModeConfiguration::exactExpressionIsForbidden(exactOutput)
      /* If the output contains the following types, we only display the
       * approximate output. (this can occur for pi > 3 for example, since
       * it's handled by approximation and not by reduction) */
   || exactOutput.recursivelyMatches(
        [](const Expression e, Context * c) {
          return e.isOfType({
            ExpressionNode::Type::Comparison
          });
        }, context)
    /* If the input contains the following types, we only display the
      * approximate output. Same if it contains an unit other than an angle. */
    || input.recursivelyMatches(
      [](const Expression e, Context * c) {
        return e.isOfType({
          ExpressionNode::Type::ConstantPhysics,
          ExpressionNode::Type::Randint,
          ExpressionNode::Type::RandintNoRepeat,
          ExpressionNode::Type::Random,
          ExpressionNode::Type::Round,
          ExpressionNode::Type::FracPart,
          ExpressionNode::Type::Integral,
          ExpressionNode::Type::Product,
          ExpressionNode::Type::Sum,
          ExpressionNode::Type::Derivative,
          ExpressionNode::Type::Sequence,
          ExpressionNode::Type::DistributionDispatcher,
        }) || (e.type() == ExpressionNode::Type::Unit && static_cast<const Unit&>(e).representative()->dimensionVector() != Unit::AngleRepresentative::Default().dimensionVector());
      }, context);
}

bool ShouldOnlyDisplayExactOutput(Poincare::Expression input) {
  /* If the input is a "store in a function", do not display the approximate
   * result. This prevents x->f(x) from displaying x = undef. */
  assert(!input.isUninitialized());
  return input.type() == ExpressionNode::Type::Store
    && input.childAtIndex(1).type() == ExpressionNode::Type::Function;
}

template float ValueOfFloatAsDisplayed<float>(float t, int precision, Poincare::Context * context);
template double ValueOfFloatAsDisplayed<double>(double t, int precision, Poincare::Context * context);

}

}
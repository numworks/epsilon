#include "expression_display_permissions.h"

#include <poincare/unit.h>

using namespace Poincare;

namespace Shared {

namespace ExpressionDisplayPermissions {

static bool isPrimeFactorization(Expression expression) {
  /* A prime factorization can only be built with integers, powers of integers,
   * and a multiplication. */
  return !expression.recursivelyMatches([](const Expression e, Context*) {
    return e.isUninitialized() ||
           !(e.type() == ExpressionNode::Type::BasedInteger ||
             e.type() == ExpressionNode::Type::Multiplication ||
             (e.type() == ExpressionNode::Type::Power &&
              e.childAtIndex(0).type() == ExpressionNode::Type::BasedInteger &&
              e.childAtIndex(1).type() == ExpressionNode::Type::BasedInteger));
  });
}

bool ExactExpressionIsForbidden(Expression e) {
  if (!Preferences::sharedPreferences->examMode().forbidExactResults()) {
    return false;
  }
  bool isFraction = e.type() == ExpressionNode::Type::Division &&
                    e.childAtIndex(0).isNumber() &&
                    e.childAtIndex(1).isNumber();
  return !(e.isNumber() || isFraction || isPrimeFactorization(e));
}

bool ShouldNeverDisplayReduction(Poincare::Expression input,
                                 Poincare::Context* context) {
  return input.recursivelyMatches(
      [](const Expression e, Context* c) {
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
        });
      },
      context);
}

bool ShouldNeverDisplayExactOutput(Poincare::Expression exactOutput,
                                   Poincare::Context* context) {
  return
      // Force all outputs to be ApproximateOnly if required by the exam mode
      // configuration
      ExactExpressionIsForbidden(exactOutput) ||
      // Lists or Matrices with only nonreal/undefined children
      (exactOutput.isOfType(
           {ExpressionNode::Type::List, ExpressionNode::Type::Matrix}) &&
       exactOutput.allChildrenAreUndefined()) ||
      /* 1. If the output contains a comparison, we only display the
       * approximate output. (this can occur for pi > 3 for example, since
       * it's handled by approximation and not by reduction)
       * 2. If the output has remaining depencies, the exact output is not
       * displayed to avoid outputs like 5 ≈ undef and also because it could
       * be a reduction that failed and was interrupted which can lead to
       * dependencies not being properly bubbled-up */
      exactOutput.recursivelyMatches(
          [](const Expression e, Context* c) {
            return e.isOfType({ExpressionNode::Type::Comparison,
                               ExpressionNode::Type::Dependency});
          },
          context) ||
      (exactOutput.hasUnit() && !exactOutput.isInRadians(context));
}

bool ShouldOnlyDisplayApproximation(Poincare::Expression input,
                                    Poincare::Expression exactOutput,
                                    Poincare::Context* context) {
  return ShouldNeverDisplayExactOutput(exactOutput, context) ||
         ShouldNeverDisplayReduction(input, context);
}

}  // namespace ExpressionDisplayPermissions

}  // namespace Shared

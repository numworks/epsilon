#include "trigonometry_list_controller.h"
#include "../app.h"
#include "apps/shared/poincare_helpers.h"
#include "poincare/expression_node.h"
#include <poincare/variable_context.h>
#include <poincare/symbol.h>

using namespace Poincare;

namespace Calculation {

constexpr char TrigonometryListController::k_symbol[];

void TrigonometryListController::setExpression(Poincare::Expression e) {
  assert(e.type() == ExpressionNode::Type::Cosine || e.type() == ExpressionNode::Type::Sine);
  IllustratedListController::setExpression(e.childAtIndex(0));

  VariableContext context = illustratedListContext();

  // Fill calculation store
  m_calculationStore.push("tan(θ)", &context, CalculationHeight);
  m_calculationStore.push("sin(θ)", &context, CalculationHeight);
  m_calculationStore.push("cos(θ)", &context, CalculationHeight);
  m_calculationStore.push("θ", &context, CalculationHeight);

  // Set trigonometry illustration
  Expression copy = e.childAtIndex(0).clone();
  Expression unit;
  Shared::PoincareHelpers::ReduceAndRemoveUnit(&copy, &context, ExpressionNode::ReductionTarget::User, &unit);
  assert(unit.isUninitialized() || static_cast<Unit &>(unit).representative()->dimensionVector() == Unit::AngleRepresentative::Default().dimensionVector());
  float angle = Shared::PoincareHelpers::ApproximateToScalar<float>(copy, &context);
  m_model.setAngle(angle);
}

KDCoordinate TrigonometryListController::nonMemoizedRowHeight(int j) {
  if (typeAtIndex(j) == k_illustrationCellType) {
    return k_illustrationHeight;
  }
  return IllustratedListController::nonMemoizedRowHeight(j);
}

}

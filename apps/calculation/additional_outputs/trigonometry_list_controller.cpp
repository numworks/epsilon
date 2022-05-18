#include "trigonometry_list_controller.h"
#include "../app.h"
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
  m_calculationStore.push("sin(θ)", &context, CalculationHeight);
  m_calculationStore.push("cos(θ)", &context, CalculationHeight);
  m_calculationStore.push("θ", &context, CalculationHeight);

  // Set trigonometry illustration
  Expression approximate = m_calculationStore.calculationAtIndex(0)->approximateOutput( Calculation::NumberOfSignificantDigits::Maximal);
  assert(!approximate.isUninitialized());
  float angle = Shared::PoincareHelpers::ApproximateToScalar<float>(approximate, &context);
  m_model.setAngle(angle);
}

}

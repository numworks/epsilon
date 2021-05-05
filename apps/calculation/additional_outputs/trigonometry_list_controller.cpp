#include "trigonometry_list_controller.h"
#include "../app.h"

using namespace Poincare;

namespace Calculation {

void TrigonometryListController::setExpression(Poincare::Expression e) {
  assert(e.type() == ExpressionNode::Type::Cosine || e.type() == ExpressionNode::Type::Sine);
  IllustratedListController::setExpression(e.childAtIndex(0));

  // Fill calculation store
  Poincare::Context * context = App::app()->localContext();
  m_calculationStore.push("sin(θ)", context, CalculationHeight);
  m_calculationStore.push("cos(θ)", context, CalculationHeight);
  m_calculationStore.push("θ", context, CalculationHeight);

  // Set trigonometry illustration
  Expression approximate = m_calculationStore.calculationAtIndex(0)->approximateOutput(context, Calculation::NumberOfSignificantDigits::Maximal);
  assert(!approximate.isUninitialized());
  float angle = Shared::PoincareHelpers::ApproximateToScalar<float>(approximate, context);
  m_model.setAngle(angle);
}

}

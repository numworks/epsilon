#include "trigonometry_list_controller.h"
#include "../app.h"

using namespace Poincare;

namespace Calculation {

void TrigonometryListController::setExpression(Poincare::Expression e) {
  assert(e.type() == ExpressionNode::Type::Cosine || e.type() == ExpressionNode::Type::Sine);
  IllustratedListController::setExpression(e.childAtIndex(0));

  // Fill calculation store
  Poincare::Context * context = App::app()->localContext();
  m_calculationStore.push("sin(θ)", context);
  m_calculationStore.push("cos(θ)", context);
  m_calculationStore.push("θ", context);

  // Set trigonometry illustration
  float angle = Shared::PoincareHelpers::ApproximateToScalar<float>(m_calculationStore.calculationAtIndex(0)->approximateOutput(context, Calculation::NumberOfSignificantDigits::Maximal), context);
  m_model.setAngle(angle);
}

}

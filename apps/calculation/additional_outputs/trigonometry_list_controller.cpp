#include "trigonometry_list_controller.h"
#include "../app.h"

using namespace Poincare;

namespace Calculation {

void TrigonometryListController::setExpression(Poincare::Expression e) {
  IllustratedListController::setExpression(e.childAtIndex(0));
  //TODO
  //m_model.setAngle(std::complex<float>(1.2f,2.3f));

  Poincare::Context * context = App::app()->localContext();
  m_calculationStore.push("sin(θ)", context);
  m_calculationStore.push("cos(θ)", context);
  m_calculationStore.push("θ", context);
}

}

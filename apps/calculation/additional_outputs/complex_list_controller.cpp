#include "complex_list_controller.h"

namespace Calculation {

void ComplexListController::fillCalculationStoreFromExpression(Poincare::Expression e) {
  //TODO
  m_model.setComplex(std::complex<float>(1.2f,2.3f));
  IllustratedListController::fillCalculationStoreFromExpression(e);
}

}

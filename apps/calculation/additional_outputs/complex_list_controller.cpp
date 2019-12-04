#include "complex_list_controller.h"
#include "../app.h"

using namespace Poincare;

namespace Calculation {

void ComplexListController::setExpression(Poincare::Expression e) {
  IllustratedListController::setExpression(e);
  //TODO
  m_model.setComplex(std::complex<float>(1.2f,2.3f));

  Poincare::Preferences * preferences = Poincare::Preferences::sharedPreferences();
  Poincare::Preferences::ComplexFormat currentComplexFormat = preferences->complexFormat();
  if (currentComplexFormat == Poincare::Preferences::ComplexFormat::Real) {
    // Temporary change complex format to avoid all additional expressions to be "unreal"
    preferences->setComplexFormat(Poincare::Preferences::ComplexFormat::Cartesian);
  }
  Poincare::Context * context = App::app()->localContext();
  // Fill Calculation Store
  m_calculationStore.push("im(z)", context);
  m_calculationStore.push("re(z)", context);
  m_calculationStore.push("arg(z)", context);
  m_calculationStore.push("abs(z)", context);

  // Reset complex format as before
  preferences->setComplexFormat(currentComplexFormat);
}

}

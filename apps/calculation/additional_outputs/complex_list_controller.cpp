#include "complex_list_controller.h"
#include "../app.h"
#include "../../shared/poincare_helpers.h"

using namespace Poincare;
using namespace Shared;

namespace Calculation {

void ComplexListController::setExpression(Poincare::Expression e) {
  IllustratedListController::setExpression(e);

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

  // Set Complex illustration
  float a = Shared::PoincareHelpers::ApproximateToScalar<float>(m_calculationStore.calculationAtIndex(2)->approximateOutput(context), context);
  float b = Shared::PoincareHelpers::ApproximateToScalar<float>(m_calculationStore.calculationAtIndex(3)->approximateOutput(context), context);
  m_model.setComplex(std::complex<float>(a,b));

  // Reset complex format as before
  preferences->setComplexFormat(currentComplexFormat);
}

}

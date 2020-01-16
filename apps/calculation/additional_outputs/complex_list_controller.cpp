#include "complex_list_controller.h"
#include "../app.h"
#include "../../shared/poincare_helpers.h"
#include <poincare/imaginary_part.h>
#include <poincare/real_part.h>
#include "complex_list_controller.h"

using namespace Poincare;
using namespace Shared;

namespace Calculation {

void ComplexListController::viewWillAppear() {
  IllustratedListController::viewWillAppear();
  m_complexGraphCell.reload(); // compute labels
}

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
  // Compute a and b as in Expression::hasDefinedComplexApproximation to ensure the same defined result
  float a = Shared::PoincareHelpers::ApproximateToScalar<float>(RealPart::Builder(e.clone()), context);
  float b = Shared::PoincareHelpers::ApproximateToScalar<float>(ImaginaryPart::Builder(e.clone()), context);
  m_model.setComplex(std::complex<float>(a,b));

  // Reset complex format as before
  preferences->setComplexFormat(currentComplexFormat);
}

}

#include "complex_list_controller.h"
#include "../app.h"
#include "../../shared/poincare_helpers.h"
#include <poincare/imaginary_part.h>
#include <poincare/real_part.h>
#include "complex_list_controller.h"
#include <poincare/variable_context.h>
#include <poincare/symbol.h>

using namespace Poincare;
using namespace Shared;

namespace Calculation {

constexpr char ComplexListController::k_symbol[];

void ComplexListController::viewWillAppear() {
  IllustratedListController::viewWillAppear();
  m_complexGraphCell.reload(); // compute labels
}

void ComplexListController::setExactAndApproximateExpression(Poincare::Expression exactExpression, Poincare::Expression approximateExpression) {
  IllustratedListController::setExactAndApproximateExpression(exactExpression, approximateExpression);

  Poincare::Preferences * preferences = Poincare::Preferences::sharedPreferences();
  Poincare::Preferences::ComplexFormat currentComplexFormat = preferences->complexFormat();
  if (currentComplexFormat == Poincare::Preferences::ComplexFormat::Real) {
    // Temporary change complex format to avoid all additional expressions to be "nonreal"
    preferences->setComplexFormat(Poincare::Preferences::ComplexFormat::Cartesian);
  }

  VariableContext context = illustratedListContext();

  // Fill Calculation Store
  m_calculationStore.push("im(z)", &context, CalculationHeight);
  m_calculationStore.push("re(z)", &context, CalculationHeight);
  m_calculationStore.push("arg(z)", &context, CalculationHeight);
  m_calculationStore.push("abs(z)", &context, CalculationHeight);

  // Set Complex illustration
  /* Compute a and b as in Expression::hasDefinedComplexApproximation to ensure
   * the same defined result. */
  float a = Shared::PoincareHelpers::ApproximateToScalar<float>(RealPart::Builder(approximateExpression.clone()), &context);
  float b = Shared::PoincareHelpers::ApproximateToScalar<float>(ImaginaryPart::Builder(approximateExpression.clone()), &context);
  m_model.setComplex(std::complex<float>(a,b));

  // Reset complex format as before
  preferences->setComplexFormat(currentComplexFormat);
}

}

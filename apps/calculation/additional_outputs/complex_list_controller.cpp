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

void ComplexListController::setExpression(Poincare::Expression e) {
  IllustratedListController::setExpression(e);

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
   * the same defined result. Since in Calculation::additionalInformationType,
   * we check for Complex additional output with the approximate output,
   * which is computed in doubles, the approximation must be done with doubles
   * here too. */
  float a = static_cast<float>(Shared::PoincareHelpers::ApproximateToScalar<double>(RealPart::Builder(e.clone()), &context));
  float b = static_cast<float>(Shared::PoincareHelpers::ApproximateToScalar<double>(ImaginaryPart::Builder(e.clone()), &context));
  m_model.setComplex(std::complex<float>(a,b));

  // Reset complex format as before
  preferences->setComplexFormat(currentComplexFormat);
}

}

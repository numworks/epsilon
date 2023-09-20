#include "complex_list_controller.h"

#include <apps/shared/poincare_helpers.h>
#include <poincare/absolute_value.h>
#include <poincare/complex_argument.h>
#include <poincare/imaginary_part.h>
#include <poincare/layout_helper.h>
#include <poincare/real_part.h>
#include <poincare/symbol.h>
#include <poincare/variable_context.h>
#include <poincare_layouts.h>

#include "../app.h"
#include "complex_list_controller.h"

using namespace Poincare;
using namespace Shared;

namespace Calculation {

void ComplexListController::computeAdditionalResults(
    const Expression input, const Expression exactOutput,
    const Expression approximateOutput) {
  assert(AdditionalResultsType::HasComplex(approximateOutput));
  ComputationContext computationContext(
      App::app()->localContext(), Preferences::ComplexFormat::Cartesian,
      Preferences::sharedPreferences->angleUnit());
  ApproximationContext approximationContext(computationContext);

  // Fill Calculation Store
  Expression e = exactOutput.clone();
  Expression z = Symbol::Builder(k_symbol);
  size_t index = 0;
  setLineAtIndex(index++, AbsoluteValue::Builder(z), AbsoluteValue::Builder(e),
                 computationContext);
  setLineAtIndex(index++, ComplexArgument::Builder(z),
                 ComplexArgument::Builder(e), computationContext);
  setLineAtIndex(index++, RealPart::Builder(z), RealPart::Builder(e),
                 computationContext);
  setLineAtIndex(index++, ImaginaryPart::Builder(z), ImaginaryPart::Builder(e),
                 computationContext);

  // Set Complex illustration
  double realPart;
  double imagPart;
  bool hasComplexApprox =
      approximateOutput.hasDefinedComplexApproximation<double>(
          approximationContext, &realPart, &imagPart);

  assert(hasComplexApprox);
  (void)hasComplexApprox;  // Silence the compiler;

  std::complex<float> floatZ(realPart, imagPart);
  m_model.setComplex(floatZ);
  setShowIllustration(std::isfinite(floatZ.real()) &&
                      std::isfinite(floatZ.imag()));
}

}  // namespace Calculation

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
  /* TODO:
   * - save values of re(z), im(z) during setLineAtIndex to directly use them in
   * setComplex ?
   * - do the same for abs(z) and arg(z) for exponential form ? */
  assert(AdditionalResultsType::HasComplex(approximateOutput, m_complexFormat,
                                           m_angleUnit));
  ComputationContext computationContext(App::app()->localContext(),
                                        Preferences::ComplexFormat::Cartesian,
                                        m_angleUnit);
  ApproximationContext approximationContext(computationContext);

  // Fill Calculation Store
  Expression e = exactOutput.clone();
  Expression z = Symbol::Builder(k_symbol);
  size_t index = 0;
  computationContext.setComplextFormat(complexFormToDisplay());
  setLineAtIndex(index++, z, e, computationContext);
  computationContext.setComplextFormat(Preferences::ComplexFormat::Cartesian);
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

I18n::Message ComplexListController::messageAtIndex(int index) {
  return index == 0
             ? complexFormToDisplay() == Preferences::ComplexFormat::Cartesian
                   ? I18n::Message::CartesianForm
                   : I18n::Message::ExponentialForm
             : 
          index == 1
            ? I18n::Message::ComplexAbsoluteValue
            :
          index == 2
            ? I18n::Message::Argument
            :
          index == 3
            ? I18n::Message::RealPart
            :
          index == 4
            ? I18n::Message::ImaginaryPart
            : I18n::Message::Default;;
};

Preferences::ComplexFormat ComplexListController::complexFormToDisplay() const {
  return m_complexFormat == Preferences::ComplexFormat::Polar
             ? Preferences::ComplexFormat::Cartesian
             : Preferences::ComplexFormat::Polar;
}

}  // namespace Calculation

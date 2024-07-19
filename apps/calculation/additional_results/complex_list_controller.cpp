#include "complex_list_controller.h"

#include <apps/shared/poincare_helpers.h>
#include <poincare/layout.h>
#include <poincare/old/absolute_value.h>
#include <poincare/old/complex_argument.h>
#include <poincare/old/imaginary_part.h>
#include <poincare/old/real_part.h>
#include <poincare/old/symbol.h>
#include <poincare/old/variable_context.h>
#include <poincare/src/expression/projection.h>

#include "../app.h"
#include "complex_list_controller.h"

using namespace Poincare;
using namespace Shared;

namespace Calculation {

void ComplexListController::computeAdditionalResults(
    const UserExpression input, const UserExpression exactOutput,
    const UserExpression approximateOutput) {
  /* TODO:
   * - save values of re(z), im(z) during setLineAtIndex to directly use them in
   * setComplex ?
   * - do the same for abs(z) and arg(z) for exponential form ? */
  assert(AdditionalResultsType::HasComplex(approximateOutput,
                                           m_calculationPreferences));
  assert(complexFormat() != Preferences::ComplexFormat::Real);
  Context* context = App::app()->localContext();
  Internal::ProjectionContext ctx = {
      .m_complexFormat = Preferences::ComplexFormat::Cartesian,
      .m_angleUnit = angleUnit(),
      .m_symbolic =
          SymbolicComputation::ReplaceAllSymbolsWithDefinitionsOrUndefined,
      .m_context = context};

  // Fill Calculation Store
  Expression e = exactOutput.clone();
  Expression z = Symbol::Builder(k_symbol);
  size_t index = 0;
  ctx.m_complexFormat = complexFormToDisplay();
  setLineAtIndex(index++, z, e, &ctx);
  ctx.m_complexFormat = Preferences::ComplexFormat::Cartesian;
  setLineAtIndex(index++, AbsoluteValue::Builder(z), AbsoluteValue::Builder(e),
                 &ctx);
  setLineAtIndex(index++, ComplexArgument::Builder(z),
                 ComplexArgument::Builder(e), &ctx);
  setLineAtIndex(index++, RealPart::Builder(z), RealPart::Builder(e), &ctx);
  setLineAtIndex(index++, ImaginaryPart::Builder(z), ImaginaryPart::Builder(e),
                 &ctx);

  // Set Complex illustration
  ApproximationContext approximationContext(context, ctx.m_complexFormat,
                                            ctx.m_angleUnit);
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
             : I18n::Message::Default;
};

Preferences::ComplexFormat ComplexListController::complexFormToDisplay() const {
  return complexFormat() == Preferences::ComplexFormat::Polar
             ? Preferences::ComplexFormat::Cartesian
             : Preferences::ComplexFormat::Polar;
}

}  // namespace Calculation

#include "complex_list_controller.h"

#include <apps/shared/poincare_helpers.h>
#include <poincare/k_tree.h>
#include <poincare/layout.h>
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
  constexpr KTree k_symbol = "z"_e;
  size_t index = 0;
  ctx.m_complexFormat = complexFormToDisplay();
  setLineAtIndex(index++, UserExpression::Create(k_symbol, {}), exactOutput,
                 &ctx);
  ctx.m_complexFormat = Preferences::ComplexFormat::Cartesian;
  setLineAtIndex(index++, UserExpression::Create(KAbs(k_symbol), {}),
                 UserExpression::Create(KAbs(KA), {.KA = exactOutput}), &ctx);
  setLineAtIndex(index++, UserExpression::Create(KArg(k_symbol), {}),
                 UserExpression::Create(KArg(KA), {.KA = exactOutput}), &ctx);
  setLineAtIndex(index++, UserExpression::Create(KRe(k_symbol), {}),
                 UserExpression::Create(KRe(KA), {.KA = exactOutput}), &ctx);
  setLineAtIndex(index++, UserExpression::Create(KIm(k_symbol), {}),
                 UserExpression::Create(KIm(KA), {.KA = exactOutput}), &ctx);

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

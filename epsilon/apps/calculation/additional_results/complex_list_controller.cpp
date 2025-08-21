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
  assert(AdditionalResultsType::HasComplex(
      approximateOutput, m_calculationPreferences, App::app()->localContext()));
  assert(complexFormat() != ComplexFormat::Real);
  Internal::ProjectionContext ctx = {
      .m_complexFormat = ComplexFormat::Cartesian,
      .m_angleUnit = angleUnit(),
      .m_symbolic = SymbolicComputation::ReplaceAllSymbols,
      .m_context = App::app()->localContext()};

  // Fill Calculation Store
  constexpr KTree k_symbol = "z"_e;
  size_t index = 0;
  ctx.m_complexFormat = complexFormToDisplay();
  setLineAtIndex(index++, UserExpression::Builder(k_symbol), exactOutput, &ctx);
  ctx.m_complexFormat = ComplexFormat::Cartesian;
  setLineAtIndex(index++, UserExpression::Builder(KAbs(k_symbol)),
                 UserExpression::Create(KAbs(KA), {.KA = exactOutput}), &ctx);
  setLineAtIndex(index++, UserExpression::Builder(KArg(k_symbol)),
                 UserExpression::Create(KArg(KA), {.KA = exactOutput}), &ctx);
  setLineAtIndex(index++, UserExpression::Builder(KRe(k_symbol)),
                 UserExpression::Create(KRe(KA), {.KA = exactOutput}), &ctx);
  setLineAtIndex(index++, UserExpression::Builder(KIm(k_symbol)),
                 UserExpression::Create(KIm(KA), {.KA = exactOutput}), &ctx);

  // Set Complex illustration
  double realPart;
  double imagPart;
  bool hasComplexApprox =
      approximateOutput.hasDefinedComplexApproximation<double>(
          ctx.m_angleUnit, ctx.m_complexFormat, App::app()->localContext(),
          &realPart, &imagPart);

  assert(hasComplexApprox);
  (void)hasComplexApprox;  // Silence the compiler;

  std::complex<float> floatZ(realPart, imagPart);
  m_model.setComplex(floatZ);
  setShowIllustration(std::isfinite(floatZ.real()) &&
                      std::isfinite(floatZ.imag()));
}

I18n::Message ComplexListController::messageAtIndex(int index) {
  return index == 0 ? complexFormToDisplay() == ComplexFormat::Cartesian
                          ? I18n::Message::CartesianForm
                          : I18n::Message::ExponentialForm
                    : I18n::Message::Default;
};

ComplexFormat ComplexListController::complexFormToDisplay() const {
  return complexFormat() == ComplexFormat::Polar ? ComplexFormat::Cartesian
                                                 : ComplexFormat::Polar;
}

}  // namespace Calculation

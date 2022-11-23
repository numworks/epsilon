#include "complex_list_controller.h"
#include "../app.h"
#include "../../shared/poincare_helpers.h"
#include <poincare/imaginary_part.h>
#include <poincare/real_part.h>
#include <poincare/complex_argument.h>
#include <poincare/absolute_value.h>
#include <poincare_layouts.h>
#include "complex_list_controller.h"
#include <poincare/variable_context.h>
#include <poincare/symbol.h>
#include <poincare/layout_helper.h>

using namespace Poincare;
using namespace Shared;

namespace Calculation {

void ComplexListController::viewWillAppear() {
  IllustratedExpressionsListController::viewWillAppear();
  m_complexGraphCell.reload(); // compute labels
}

void ComplexListController::setExactAndApproximateExpression(Poincare::Expression exactExpression, Poincare::Expression approximateExpression) {
  IllustratedExpressionsListController::setExactAndApproximateExpression(exactExpression, approximateExpression);

  Poincare::Preferences preferencesComplex = *Preferences::sharedPreferences();
  preferencesComplex.setComplexFormat(Poincare::Preferences::ComplexFormat::Cartesian);
  Context * context = App::app()->localContext();

  // Fill Calculation Store
  Expression e = exactExpression;
  Expression z = Symbol::Builder(k_symbol);
  size_t index = 0;
  appendLine(index++, AbsoluteValue::Builder(z), AbsoluteValue::Builder(e), context, &preferencesComplex);
  appendLine(index++, ComplexArgument::Builder(z), ComplexArgument::Builder(e), context, &preferencesComplex);
  appendLine(index++, RealPart::Builder(z), RealPart::Builder(e), context, &preferencesComplex);
  appendLine(index++, ImaginaryPart::Builder(z), ImaginaryPart::Builder(e), context, &preferencesComplex);

  // Set Complex illustration
  float realPart;
  float imagPart;
  bool hasComplexApprox = approximateExpression.hasDefinedComplexApproximation(context, preferencesComplex.complexFormat(), preferencesComplex.angleUnit(), &realPart, &imagPart);

  assert(hasComplexApprox);
  (void) hasComplexApprox; // Silence the compiler;

  m_model.setComplex(std::complex<float>(realPart,imagPart));
  setShowIllustration(true);
}

}

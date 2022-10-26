#include "function_list_controller.h"
#include "../app.h"
#include <apps/shared/poincare_helpers.h>
#include <ion/unicode/code_point.h>
#include <poincare/expression.h>
#include <poincare/layout_helper.h>
#include <poincare/rational.h>
#include <apps/global_preferences.h>
#include <poincare/constant.h>
#include <poincare/code_point_layout.h>
#include <poincare/horizontal_layout.h>
#include <poincare/vertical_offset_layout.h>
#include <poincare/symbol.h>
#include <string.h>


using namespace Poincare;
using namespace Shared;

namespace Calculation {

void FunctionListController::setExpression(Poincare::Expression e) {
  ExpressionsListController::setExpression(e);
  assert(!m_expression.isUninitialized());
  static_assert(k_maxNumberOfRows >= k_maxNumberOfOutputRows, "k_maxNumberOfRows must be greater than k_maxNumberOfOutputRows");

  Preferences * preferences = Preferences::sharedPreferences();
  Context * context = App::app()->localContext();

  float abscissa = e.getNumericalValue();
  Symbol variable = Symbol::Builder(UCodePointUnknown);
  e.replaceNumericalValuesWithSymbol(variable);

  Expression reducedExpression = e;
  PoincareHelpers::CloneAndReduce(&reducedExpression, context, ExpressionNode::ReductionTarget::SystemForApproximation);
  m_model.setParameters(reducedExpression, abscissa);

  m_layouts[0] = LayoutHelper::String("y");
  m_approximatedLayouts[0] = e.replaceSymbolWithExpression(variable, Symbol::Builder(k_symbol)).createLayout(preferences->displayMode(), preferences->numberOfSignificantDigits(), context);

  illustrationCell()->reloadCell();
  setShowIllustration(true);
}

void FunctionListController::viewDidDisappear() {
  ExpressionsListController::viewDidDisappear();
  m_model.tidy();
}

I18n::Message FunctionListController::messageAtIndex(int index) {
  assert(index == 0);
  return I18n::Message::CurveEquation;
}

}

#include "function_list_controller.h"
#include "../app.h"
#include "../../shared/poincare_helpers.h"
#include "ion/unicode/code_point.h"
#include "poincare/expression.h"
#include "poincare/layout_helper.h"
#include "poincare/rational.h"
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
  Preferences::ComplexFormat currentComplexFormat = preferences->complexFormat();
  if (currentComplexFormat == Preferences::ComplexFormat::Real) {
    /* Temporary change complex format to avoid all additional expressions to be
     * "nonreal" (with [i] for instance). As additional results are computed from
     * the output, which is built taking ComplexFormat into account, there are
     * no risks of displaying additional results on an nonreal output. */
    preferences->setComplexFormat(Preferences::ComplexFormat::Cartesian);
  }

  Context * context = App::app()->localContext();

  float abscissa = e.getNumericalValue();
  Symbol variable = Symbol::Builder(UCodePointUnknown);
  e = e.replaceNumericalValuesWithSymbol(variable);

  int index = 0;
  m_indexMessageMap[index] = 0;
  m_layouts[index] = LayoutHelper::String("y");
  m_approximatedLayouts[index] = e.replaceSymbolWithExpression(variable, Symbol::Builder(k_symbol)).createLayout(preferences->displayMode(), preferences->numberOfSignificantDigits(), context);
  index++;

  m_model.setParameters(e.replaceSymbolWithExpression(Symbol::Builder('x'), Symbol::Builder(UCodePointUnknown)), abscissa);
  illustrationCell()->reloadCell();

  setShowIllustration(true);
  // Reset complex format as before
  preferences->setComplexFormat(currentComplexFormat);
}

void FunctionListController::viewDidDisappear() {
  ExpressionsListController::viewDidDisappear();
  m_model.tidy();
}

I18n::Message FunctionListController::messageAtIndex(int index) {
  // Message index is mapped in setExpression because it depends on the Matrix.
  assert(index < k_maxNumberOfOutputRows && index >=0);
  I18n::Message messages[k_maxNumberOfOutputRows] = {
    I18n::Message::CartesianType, // TODO: CurveEquation
  };
  return messages[m_indexMessageMap[index]];
}

}

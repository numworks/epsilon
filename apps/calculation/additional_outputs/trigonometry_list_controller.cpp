#include "trigonometry_list_controller.h"
#include "../app.h"
#include "apps/shared/poincare_helpers.h"
#include "poincare/expression_node.h"
#include <poincare/variable_context.h>
#include <poincare/symbol.h>
#include <poincare/multiplication.h>
#include <poincare/sine.h>
#include <poincare/cosine.h>
#include <poincare/tangent.h>
#include <poincare/unit.h>
#include <poincare/unit_convert.h>
#include "poincare/layout_helper.h"
#include "../../shared/poincare_helpers.h"

using namespace Poincare;

namespace Calculation {

constexpr char TrigonometryListController::k_symbol[];

void TrigonometryListController::setExpression(Poincare::Expression e) {
  IllustratedExpressionsListController::setExpression(e);

  Poincare::Preferences * preferences = Poincare::Preferences::sharedPreferences();
  Context * context = App::app()->localContext();
  size_t index = 0;

  // Angle

  // Set trigonometry illustration
  Expression copy = e.clone();
  Expression unit;
  Shared::PoincareHelpers::ReduceAndRemoveUnit(&copy, context, ExpressionNode::ReductionTarget::User, &unit);
  assert(unit.isUninitialized() || static_cast<Unit &>(unit).representative()->dimensionVector() == Unit::AngleRepresentative::Default().dimensionVector());
  e = copy;
  // float angle = Shared::PoincareHelpers::ApproximateToScalar<float>(copy, &context);
  // TODO modulo 2 pi    handleEvent should not insert cos(θ)
  m_layouts[index] = LayoutHelper::String("θ");
  Expression radian = Unit::Builder(Unit::k_angleRepresentatives + Unit::k_radianRepresentativeIndex);
  Expression degrees = Unit::Builder(Unit::k_angleRepresentatives + Unit::k_degreeRepresentativeIndex);
  Expression inRadian = Multiplication::Builder(e, radian);
  m_exactLayouts[index] = getLayoutFromExpression(inRadian, context, preferences);
  m_approximatedLayouts[index] = getLayoutFromExpression(UnitConvert::Builder(inRadian, degrees), context, preferences);
  index++;

  auto appendLine = [&](const char * formula, Expression expression) {
    m_layouts[index] = LayoutHelper::String(formula);
    m_exactLayouts[index] = getLayoutFromExpression(expression, context, preferences);
    m_approximatedLayouts[index] = getLayoutFromExpression(expression.approximate<double>(context, preferences->complexFormat(), preferences->angleUnit()), context, preferences);
    index++;
  };

  appendLine("cos(θ)", Cosine::Builder(e));
  appendLine("sin(θ)", Sine::Builder(e));
  appendLine("tan(θ)", Tangent::Builder(e));
  
  // Set illustration
  float angle = Shared::PoincareHelpers::ApproximateToScalar<float>(e, context);
  m_model.setAngle(angle);
  setShowIllustration(true);
}

KDCoordinate TrigonometryListController::nonMemoizedRowHeight(int j) {
  if (typeAtIndex(j) == k_illustrationCellType) {
    return k_illustrationHeight;
  }
  return IllustratedExpressionsListController::nonMemoizedRowHeight(j);
}

I18n::Message TrigonometryListController::messageAtIndex(int index) {
  if (index == 0) {
    return I18n::Message::AngleUnit;
  }
  return I18n::Message::Default;
}

}

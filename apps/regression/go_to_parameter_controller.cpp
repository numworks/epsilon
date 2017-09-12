#include "go_to_parameter_controller.h"
#include "graph_controller.h"
#include "../apps_container.h"
#include <assert.h>
#include <float.h>
#include <cmath>

using namespace Shared;
using namespace Poincare;

namespace Regression {

GoToParameterController::GoToParameterController(Responder * parentResponder, Store * store, CurveViewCursor * cursor, GraphController * graphController) :
  Shared::GoToParameterController(parentResponder, store, cursor, I18n::Message::X),
  m_store(store),
  m_xPrediction(true),
  m_graphController(graphController)
{
}

void GoToParameterController::setXPrediction(bool xPrediction) {
  m_xPrediction = xPrediction;
}

const char * GoToParameterController::title() {
  if (m_xPrediction) {
    return I18n::translate(I18n::Message::XPrediction);
  }
  return I18n::translate(I18n::Message::YPrediction);
}

double GoToParameterController::parameterAtIndex(int index) {
  assert(index == 0);
  if (m_xPrediction) {
    return m_cursor->x();
  }
  return m_cursor->y();
}

bool GoToParameterController::setParameterAtIndex(int parameterIndex, double f) {
  assert(parameterIndex == 0);
  if (std::fabs(f) > k_maxDisplayableFloat) {
    app()->displayWarning(I18n::Message::ForbiddenValue);
    return false;
  }
  double x = m_store->xValueForYValue(f);
  if (m_xPrediction) {
    x = m_store->yValueForXValue(f);
  }
  if (std::fabs(x) > k_maxDisplayableFloat) {
    app()->displayWarning(I18n::Message::ForbiddenValue);
    return false;
  }
  if (std::isnan(x)) {
    if (m_store->slope() < DBL_EPSILON && f == m_store->yIntercept()) {
      m_graphController->selectRegressionCurve();
      m_cursor->moveTo(m_cursor->x(), f);
      return true;
    }
    app()->displayWarning(I18n::Message::ValueNotReachedByRegression);
    return false;
  }
  m_graphController->selectRegressionCurve();
  if (m_xPrediction) {
    m_cursor->moveTo(f, x);
  } else {
    m_cursor->moveTo(x, f);
  }
  m_graphRange->centerAxisAround(CurveViewRange::Axis::X, m_cursor->x());
  m_graphRange->centerAxisAround(CurveViewRange::Axis::Y, m_cursor->y());
  return true;
}

void GoToParameterController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  if (index == numberOfRows()-1) {
    return;
  }
  MessageTableCellWithEditableText * myCell = (MessageTableCellWithEditableText *) cell;
  if (m_xPrediction) {
    myCell->setMessage(I18n::Message::X);
  } else {
    myCell->setMessage(I18n::Message::Y);
  }
  FloatParameterController::willDisplayCellForIndex(cell, index);
}

}

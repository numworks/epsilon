#include "go_to_parameter_controller.h"
#include "graph_controller.h"
#include "../apps_container.h"
#include <assert.h>
#include <float.h>
#include <cmath>

using namespace Shared;
using namespace Poincare;

namespace Regression {

GoToParameterController::GoToParameterController(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate, Store * store, CurveViewCursor * cursor, GraphController * graphController) :
  Shared::GoToParameterController(parentResponder, inputEventHandlerDelegate, store, cursor, I18n::Message::X),
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
  int series = m_graphController->selectedSeriesIndex();
  if (std::fabs(f) > k_maxDisplayableFloat) {
    app()->displayWarning(I18n::Message::ForbiddenValue);
    return false;
  }
  Poincare::Context * globContext = const_cast<AppsContainer *>(static_cast<const AppsContainer *>(app()->container()))->globalContext();
  double unknown = m_xPrediction ?
    m_store->yValueForXValue(series, f, globContext) :
    m_store->xValueForYValue(series, f, globContext);

  // Forbidden value
  if (std::fabs(unknown) > k_maxDisplayableFloat) {
    app()->displayWarning(I18n::Message::ForbiddenValue);
    return false;
  }

  if (std::isnan(unknown)) {
    if (!m_xPrediction) {
      double x = m_cursor->x();
      unknown = m_store->modelForSeries(series)->evaluate(m_store->coefficientsForSeries(series, globContext), x);
      if (std::fabs(unknown - f) < DBL_EPSILON) {
        // If the computed value is NaN and the current abscissa is solution
        m_graphController->selectRegressionCurve();
        m_cursor->moveTo(x, unknown);
        return true;
      }
    }
    // Value not reached
    app()->displayWarning(I18n::Message::ValueNotReachedByRegression);
    return false;
  }
  m_graphController->selectRegressionCurve();
  if (m_xPrediction) {
    m_cursor->moveTo(f, unknown);
  } else {
    double yFromX = m_store->modelForSeries(series)->evaluate(m_store->coefficientsForSeries(series, globContext), unknown);
    /* We here compute y2 = a*((y1-b)/a)+b, which does not always give y1,
     * because of computation precision. y2 migth thus be invalid. */
    if (std::fabs(yFromX) > k_maxDisplayableFloat || std::isnan(yFromX)) {
      app()->displayWarning(I18n::Message::ForbiddenValue);
      return false;
    }
    m_cursor->moveTo(unknown, yFromX);
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

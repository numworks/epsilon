#include "go_to_parameter_controller.h"

#include <apps/apps_container_helper.h>
#include <assert.h>
#include <float.h>

#include <cmath>

#include "../app.h"
#include "graph_controller.h"

using namespace Shared;
using namespace Poincare;
using namespace Escher;

namespace Regression {

GoToParameterController::GoToParameterController(
    Responder* parentResponder,
    Escher::InputEventHandlerDelegate* inputEventHandlerDelegate,
    InteractiveCurveViewRange* range, Store* store, CurveViewCursor* cursor,
    GraphController* graphController)
    : Shared::GoToParameterController(parentResponder,
                                      inputEventHandlerDelegate, range, cursor),
      m_store(store),
      m_xPrediction(true),
      m_graphController(graphController) {}

void GoToParameterController::setXPrediction(bool xPrediction) {
  m_xPrediction = xPrediction;
  setParameterName(xPrediction ? I18n::Message::X : I18n::Message::Y);
}

const char* GoToParameterController::title() {
  if (m_xPrediction) {
    return I18n::translate(I18n::Message::XPrediction);
  }
  return I18n::translate(I18n::Message::YPrediction);
}

double GoToParameterController::extractParameterAtIndex(int index) {
  assert(index == 0);
  if (m_xPrediction) {
    return m_cursor->x();
  }
  return m_cursor->y();
}

bool GoToParameterController::confirmParameterAtIndex(int parameterIndex,
                                                      double f) {
  assert(parameterIndex == 0);
  int series = m_graphController->selectedSeriesIndex();
  Poincare::Context* globContext =
      AppsContainerHelper::sharedAppsContainerGlobalContext();
  double unknown = m_xPrediction
                       ? m_store->yValueForXValue(series, f, globContext)
                       : m_store->xValueForYValue(series, f, globContext);

  if (std::isnan(unknown) || std::isinf(unknown)) {
    if (!m_xPrediction) {
      double x = m_cursor->x();
      unknown = m_store->modelForSeries(series)->evaluate(
          m_store->coefficientsForSeries(series, globContext), x);
      if (std::fabs(unknown - f) < DBL_EPSILON) {
        // If the computed value is NaN and the current abscissa is solution
        m_graphController->selectRegressionCurve();
        m_cursor->moveTo(x, x, unknown);
        return true;
      }
    }
    // Value not reached
    Container::activeApp()->displayWarning(
        I18n::Message::ValueNotReachedByRegression);
    return false;
  }
  m_graphController->selectRegressionCurve();
  if (m_xPrediction) {
    m_cursor->moveTo(f, f, unknown);
  } else {
    double yFromX = m_store->modelForSeries(series)->evaluate(
        m_store->coefficientsForSeries(series, globContext), unknown);
    /* We here compute y2 = a*((y1-b)/a)+b, which does not always give y1,
     * because of computation precision. y2 migth thus be invalid. */
    if (std::isnan(yFromX) || std::isinf(yFromX)) {
      Container::activeApp()->displayWarning(I18n::Message::ForbiddenValue);
      return false;
    }
    m_cursor->moveTo(unknown, unknown, yFromX);
  }
  m_graphRange->centerAxisAround(CurveViewRange::Axis::X, m_cursor->x());
  m_graphRange->centerAxisAround(CurveViewRange::Axis::Y, m_cursor->y());
  return true;
}

}  // namespace Regression

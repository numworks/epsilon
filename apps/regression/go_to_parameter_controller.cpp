#include "go_to_parameter_controller.h"
#include "../apps_container.h"
#include <assert.h>

using namespace Shared;
using namespace Poincare;

namespace Regression {

GoToParameterController::GoToParameterController(Responder * parentResponder, Store * store, CurveViewCursor * cursor) :
  Shared::GoToParameterController(parentResponder, store, cursor, I18n::Message::X),
  m_store(store),
  m_xPrediction(true)
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

float GoToParameterController::parameterAtIndex(int index) {
  assert(index == 0);
  if (m_xPrediction) {
    return m_cursor->x();
  }
  return m_cursor->y();
}

bool GoToParameterController::setParameterAtIndex(int parameterIndex, float f) {
  assert(parameterIndex == 0);
  if (fabsf(f) > k_maxDisplayableFloat) {
    app()->displayWarning(I18n::Message::ForbiddenValue);
    return false;
  }
  float x = m_store->xValueForYValue(f);
  if (m_xPrediction) {
    x = m_store->yValueForXValue(f);
  }
  if (fabsf(x) > k_maxDisplayableFloat) {
    app()->displayWarning(I18n::Message::ForbiddenValue);
    return false;
  }
  if (isnan(x)) {
    app()->displayWarning(I18n::Message::ValueNotReachedByRegression);
    return false;
  }
  if (m_xPrediction) {
    m_cursor->moveTo(f, x);
  } else {
    m_cursor->moveTo(x, f);
  }
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

#include "function_go_to_parameter_controller.h"
#include "function_app.h"
#include "function.h"
#include <assert.h>
#include <cmath>
#include <ion/display.h>

using namespace Escher;

namespace Shared {

FunctionGoToParameterController::FunctionGoToParameterController(Responder * parentResponder, Escher::InputEventHandlerDelegate * inputEventHandlerDelegate, InteractiveCurveViewController * graphController, InteractiveCurveViewRange * graphRange, CurveViewCursor * cursor) :
  GoToParameterController(parentResponder, inputEventHandlerDelegate, graphRange, cursor),
  m_graphController(graphController)
{}

bool FunctionGoToParameterController::confirmParameterAtIndex(int parameterIndex, double f) {
  assert(parameterIndex == 0);
  FunctionApp * app = FunctionApp::app();
  // If possible, round f so that we go to the evaluation of the displayed f
  double pixelWidth = (m_graphRange->xMax() - m_graphRange->xMin()) / Ion::Display::Width;
  f = FunctionBannerDelegate::GetValueDisplayedOnBanner(f, app->localContext(), Poincare::Preferences::sharedPreferences->numberOfSignificantDigits(), pixelWidth, false);
  m_graphController->moveCursorAndCenterIfNeeded(f);

  return true;
}

void FunctionGoToParameterController::setRecord(Ion::Storage::Record record) {
  m_record = record;
  ExpiringPointer<Function> function = FunctionApp::app()->functionStore()->modelForRecord(m_record);
  setParameterName(function->parameterMessageName());
}

}

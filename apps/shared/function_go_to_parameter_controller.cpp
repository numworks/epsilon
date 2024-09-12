#include "function_go_to_parameter_controller.h"

#include <assert.h>
#include <ion/display.h>

#include <cmath>

#include "function.h"
#include "function_app.h"

using namespace Escher;

namespace Shared {

FunctionGoToParameterController::FunctionGoToParameterController(
    Responder* parentResponder, InteractiveCurveViewController* graphController,
    InteractiveCurveViewRange* graphRange, CurveViewCursor* cursor)
    : GoToParameterController(parentResponder, graphRange, cursor),
      m_graphController(graphController) {}

bool FunctionGoToParameterController::confirmParameterAtIndex(
    int parameterIndex, double f) {
  assert(parameterIndex == 0);
  FunctionApp* app = FunctionApp::app();
  // If possible, round f so that we go to the evaluation of the displayed f
  double pixelWidth =
      (m_graphRange->xMax() - m_graphRange->xMin()) / Ion::Display::Width;
  f = FunctionBannerDelegate::GetValueDisplayedOnBanner(
      f, app->localContext(),
      Poincare::Preferences::SharedPreferences()->numberOfSignificantDigits(),
      pixelWidth, false);
  m_graphController->moveCursorAndCenterIfNeeded(f);

  return true;
}

}  // namespace Shared

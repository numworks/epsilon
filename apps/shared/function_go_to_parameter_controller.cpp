#include "function_go_to_parameter_controller.h"
#include "function_app.h"
#include "function.h"
#include <assert.h>
#include <cmath>
#include <ion/display.h>

using namespace Escher;

namespace Shared {

FunctionGoToParameterController::FunctionGoToParameterController(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate, InteractiveCurveViewController * graphController, InteractiveCurveViewRange * graphRange, CurveViewCursor * cursor) :
  GoToParameterController(parentResponder, inputEventHandlerDelegate, graphRange, cursor),
  m_graphController(graphController)
{}

bool FunctionGoToParameterController::confirmParameterAtIndex(int parameterIndex, double f) {
  assert(parameterIndex == 0);
  FunctionApp * app = FunctionApp::app();
  // If possible, round f so that we go to the evaluation of the displayed f
  double pixelWidth = (m_graphRange->xMax() - m_graphRange->xMin()) / Ion::Display::Width;
  f = FunctionBannerDelegate::GetValueDisplayedOnBanner(f, app->localContext(), Poincare::Preferences::sharedPreferences()->numberOfSignificantDigits(), pixelWidth, false);
  m_graphController->moveCursorAndCenterIfNeeded(f);

  /* The range might have evolved to center around the cursor but we don't want
   * to reinit the cursor position when displaying the graph controller. To
   * prevent this, we update the snapshot range version in order to make the
   * graph controller as if the range has not evolved since last appearance. */
  uint32_t * snapshotRangeVersion = static_cast<FunctionApp::Snapshot *>(app->snapshot())->rangeVersion();
  *snapshotRangeVersion = m_graphRange->rangeChecksum();
  return true;
}

void FunctionGoToParameterController::setRecord(Ion::Storage::Record record) {
  m_record = record;
  ExpiringPointer<Function> function = FunctionApp::app()->functionStore()->modelForRecord(m_record);
  setParameterName(function->parameterMessageName());
}

}

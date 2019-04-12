#include "centered_screen_test_controller.h"

namespace HardwareTest {

void CenteredScreenTestController::ContentView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(rect, KDColorWhite);
  ctx->fillRect(KDRect(k_outlineThickness, k_outlineThickness, bounds().width()-2*k_outlineThickness, bounds().height()-2*k_outlineThickness), KDColorBlack);
}

}

#include "empty_battery_window.h"

#include <apps/i18n.h>
#include <ion.h>
extern "C" {
#include <assert.h>
}

EmptyBatteryWindow::EmptyBatteryWindow() : Window() {}

void EmptyBatteryWindow::drawRect(KDContext* ctx, KDRect rect) const {
  ctx->fillRect(bounds(), KDColorWhite);
  const char* warningMessage = I18n::translate(I18n::Message::LowBattery);
  ctx->alignAndDrawString(warningMessage, KDPointZero,
                          KDSize(Ion::Display::Width, Ion::Display::Height),
                          {.horizontalAlignment = KDGlyph::k_alignCenter});
}

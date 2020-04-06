#include "empty_battery_window.h"
#include <apps/i18n.h>
extern "C" {
#include <assert.h>
}

EmptyBatteryWindow::EmptyBatteryWindow() :
  Window()
{
}

void EmptyBatteryWindow::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(bounds(), KDColorWhite);
  const char * warningMessage = I18n::translate(I18n::Message::LowBattery);
  KDSize warningSize = KDFont::LargeFont->stringSize(warningMessage);
  ctx->drawString(warningMessage, KDPoint((Ion::Display::Width - warningSize.width())/2, (Ion::Display::Height - warningSize.height())/2), KDFont::LargeFont);
}


#include "battery_view.h"
#include <escher/palette.h>

using namespace Escher;

const uint8_t flashMask[BatteryView::k_flashHeight][BatteryView::k_flashWidth] = {
  {0xDB, 0x00, 0x00, 0xFF},
  {0xB7, 0x00, 0x6D, 0xFF},
  {0x6D, 0x00, 0xDB, 0xFF},
  {0x24, 0x00, 0x00, 0x00},
  {0x00, 0x00, 0x00, 0x24},
  {0xFF, 0xDB, 0x00, 0x6D},
  {0xFF, 0x6D, 0x00, 0xB7},
  {0xFF, 0x00, 0x00, 0xDB},
};

const uint8_t tickMask[BatteryView::k_tickHeight][BatteryView::k_tickWidth] = {
  {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xDB, 0x00, 0x24},
  {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x6D, 0x00, 0xDB},
  {0x6D, 0x00, 0xB7, 0xFF, 0xB7, 0x00, 0x24, 0xFF},
  {0xDB, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0xFF},
  {0xFF, 0xB7, 0x00, 0x24, 0x00, 0xB7, 0xFF, 0xFF},
  {0xFF, 0xFF, 0x24, 0x00, 0x24, 0xFF, 0xFF, 0xFF},

};

bool BatteryView::setChargeState(Ion::Battery::Charge chargeState) {
  /* There is no specific battery picto for 'empty' battery as the whole device
   * shut down. Still, there might be a redrawing of the window before shutting
   * down so we handle this case as the 'low' battery one. Plus, we avoid
   * trigerring a redrawing by not marking anything as dirty when switching
   * from 'low' to 'empty' battery. */
  chargeState = chargeState == Ion::Battery::Charge::EMPTY ? Ion::Battery::Charge::LOW : chargeState;
  if (chargeState != m_chargeState) {
    m_chargeState = chargeState;
    markRectAsDirty(bounds());
    return true;
  }
  return false;
}

bool BatteryView::setIsCharging(bool isCharging) {
  if (m_isCharging != isCharging) {
    m_isCharging = isCharging;
    markRectAsDirty(bounds());
    return true;
  }
  return false;
}

bool BatteryView::setIsPlugged(bool isPlugged) {
  if (m_isPlugged != isPlugged) {
    m_isPlugged = isPlugged;
    markRectAsDirty(bounds());
    return true;
  }
  return false;
}

void BatteryView::drawRect(KDContext * ctx, KDRect rect) const {
  assert(m_chargeState != Ion::Battery::Charge::EMPTY);
  /* We draw from left to right. The middle part representing the battery
   *'content' depends on the charge */

  // Draw the left part
  ctx->fillRect(KDRect(0, 0, k_elementWidth, k_batteryHeight), KDColorWhite);

  // Draw the middle part
  constexpr KDCoordinate batteryInsideX = k_elementWidth+k_separatorThickness;
  constexpr KDCoordinate batteryInsideWidth = k_batteryWidth-3*k_elementWidth-2*k_separatorThickness;
  if (m_isCharging) {
    // Charging: Yellow background with flash
    ctx->fillRect(KDRect(batteryInsideX, 0, batteryInsideWidth, k_batteryHeight), Palette::YellowLight);
    KDRect frame((k_batteryWidth-k_flashWidth)/2, 0, k_flashWidth, k_flashHeight);
    KDColor flashWorkingBuffer[k_flashHeight*k_flashWidth];
    ctx->blendRectWithMask(frame, KDColorWhite, (const uint8_t *)flashMask, flashWorkingBuffer);
  } else if (m_chargeState == Ion::Battery::Charge::LOW) {
    assert(!m_isPlugged);
    // Low: Quite empty battery
    ctx->fillRect(KDRect(batteryInsideX, 0, 2*k_elementWidth, k_batteryHeight), Palette::LowBattery);
    ctx->fillRect(KDRect(3*k_elementWidth+k_separatorThickness, 0, k_batteryWidth-5*k_elementWidth-2*k_separatorThickness, k_batteryHeight), Palette::YellowLight);
  } else if (m_chargeState == Ion::Battery::Charge::SOMEWHERE_INBETWEEN) {
    assert(!m_isPlugged);
    // Middle: Half full battery
    constexpr KDCoordinate middleChargeWidth = batteryInsideWidth/2;
    ctx->fillRect(KDRect(batteryInsideX, 0, middleChargeWidth, k_batteryHeight), KDColorWhite);
    ctx->fillRect(KDRect(batteryInsideX+middleChargeWidth, 0, middleChargeWidth, k_batteryHeight), Palette::YellowLight);
  } else {
    assert(m_chargeState == Ion::Battery::Charge::FULL);
    // Full but not plugged: Full battery
    ctx->fillRect(KDRect(batteryInsideX, 0, batteryInsideWidth, k_batteryHeight), KDColorWhite);
    if (m_isPlugged) {
      // Plugged and full: Full battery with tick
      KDRect frame((k_batteryWidth-k_tickWidth)/2, (k_batteryHeight-k_tickHeight)/2, k_tickWidth, k_tickHeight);
      KDColor tickWorkingBuffer[k_tickHeight*k_tickWidth];
      ctx->blendRectWithMask(frame, Palette::YellowDark, (const uint8_t *)tickMask, tickWorkingBuffer);
    }
  }

  // Draw the right part
  ctx->fillRect(KDRect(k_batteryWidth-2*k_elementWidth, 0, k_elementWidth, k_batteryHeight), KDColorWhite);
  ctx->fillRect(KDRect(k_batteryWidth-k_elementWidth, (k_batteryHeight-k_capHeight)/2, k_elementWidth, k_capHeight), KDColorWhite);
}

KDSize BatteryView::minimalSizeForOptimalDisplay() const {
  return KDSize(k_batteryWidth, k_batteryHeight);
}

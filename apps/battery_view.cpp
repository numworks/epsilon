#include "battery_view.h"

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


BatteryView::BatteryView() :
  View(),
  m_chargeState(Ion::Battery::Charge::SOMEWHERE_INBETWEEN),
  m_isCharging(false),
  m_isPlugged(false)
{
}

bool BatteryView::setChargeState(Ion::Battery::Charge chargeState) {
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

KDColor s_flashWorkingBuffer[BatteryView::k_flashHeight*BatteryView::k_flashWidth];
KDColor s_tickWorkingBuffer[BatteryView::k_tickHeight*BatteryView::k_tickWidth];

void BatteryView::drawRect(KDContext * ctx, KDRect rect) const {
  /* We draw from left to right. The middle part representing the battery
   *'content' depends on the charge */
  ctx->fillRect(KDRect(0, 0, k_elementWidth, k_batteryHeight), KDColorWhite);
  if (m_isCharging) {
    ctx->fillRect(KDRect(k_elementWidth+k_separatorThickness, 0, k_batteryWidth-3*k_elementWidth-2*k_separatorThickness, k_batteryHeight), Palette::YellowLight);
    KDRect frame((k_batteryWidth-k_flashWidth)/2, 0, k_flashWidth, k_flashHeight);
    ctx->blendRectWithMask(frame, KDColorWhite, (const uint8_t *)flashMask, s_flashWorkingBuffer);
  }
  if (!m_isCharging && m_isPlugged && m_chargeState == Ion::Battery::Charge::FULL) {
    ctx->fillRect(KDRect(k_elementWidth+k_separatorThickness, 0, k_batteryWidth-3*k_elementWidth-2*k_separatorThickness, k_batteryHeight), KDColorWhite);
    KDRect frame((k_batteryWidth-k_tickWidth)/2, (k_batteryHeight-k_tickHeight)/2, k_tickWidth, k_tickHeight);
    ctx->blendRectWithMask(frame, Palette::YellowDark, (const uint8_t *)tickMask, s_tickWorkingBuffer);
  }
  if (!m_isCharging && m_chargeState == Ion::Battery::Charge::LOW) {
    ctx->fillRect(KDRect(k_elementWidth+k_separatorThickness, 0, 2*k_elementWidth, k_batteryHeight), Palette::LowBattery);
    ctx->fillRect(KDRect(3*k_elementWidth+k_separatorThickness, 0, k_batteryWidth-5*k_elementWidth-2*k_separatorThickness, k_batteryHeight), Palette::YellowLight);
  }
  if (!m_isCharging && m_chargeState == Ion::Battery::Charge::SOMEWHERE_INBETWEEN) {
    ctx->fillRect(KDRect(k_elementWidth+k_separatorThickness, 0, (k_batteryWidth-3*k_elementWidth-2*k_separatorThickness)/2, k_batteryHeight), KDColorWhite);
    ctx->fillRect(KDRect(k_elementWidth+k_separatorThickness+(k_batteryWidth-3*k_elementWidth-2*k_separatorThickness)/2, 0, (k_batteryWidth-3*k_elementWidth-2*k_separatorThickness)/2, k_batteryHeight), Palette::YellowLight);
  }
  if (!m_isCharging && !m_isPlugged && m_chargeState == Ion::Battery::Charge::FULL) {
    ctx->fillRect(KDRect(k_elementWidth+k_separatorThickness, 0, k_batteryWidth-3*k_elementWidth-2*k_separatorThickness, k_batteryHeight), KDColorWhite);
  }
  ctx->fillRect(KDRect(k_batteryWidth-2*k_elementWidth, 0, k_elementWidth, k_batteryHeight), KDColorWhite);
  ctx->fillRect(KDRect(k_batteryWidth-k_elementWidth, (k_batteryHeight-k_capHeight)/2, k_elementWidth, k_capHeight), KDColorWhite);
}

KDSize BatteryView::minimalSizeForOptimalDisplay() const {
  return KDSize(k_batteryWidth, k_batteryHeight);
}

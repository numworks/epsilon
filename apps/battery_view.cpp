#include "battery_view.h"

BatteryView::BatteryView() :
  View(),
  m_chargeState(Ion::Battery::Charge::SOMEWHERE_INBETWEEN)
{
}

void BatteryView::setChargeState(Ion::Battery::Charge chargeState) {
  if (chargeState != m_chargeState) {
    m_chargeState = chargeState;
    markRectAsDirty(bounds());
  }
}

void BatteryView::drawRect(KDContext * ctx, KDRect rect) const {
  /* We draw from left to right. The middle part representing the battery
   *'content' depends on the charge */
  ctx->fillRect(KDRect(0, 0, k_elementWidth, k_batteryHeight), KDColorWhite);
  if (m_chargeState == Ion::Battery::Charge::EMPTY) {
    ctx->fillRect(KDRect(k_elementWidth+k_separatorThickness, 0, 2*k_elementWidth, k_batteryHeight), Palette::LowBattery);
    ctx->fillRect(KDRect(3*k_elementWidth+k_separatorThickness, 0, k_batteryWidth-5*k_elementWidth-2*k_separatorThickness, k_batteryHeight), Palette::YellowLight);
  }
  if (m_chargeState == Ion::Battery::Charge::SOMEWHERE_INBETWEEN) {
    ctx->fillRect(KDRect(k_elementWidth+k_separatorThickness, 0, (k_batteryWidth-3*k_elementWidth-2*k_separatorThickness)/2, k_batteryHeight), KDColorWhite);
    ctx->fillRect(KDRect(k_elementWidth+k_separatorThickness+(k_batteryWidth-3*k_elementWidth-2*k_separatorThickness)/2, 0, (k_batteryWidth-3*k_elementWidth-2*k_separatorThickness)/2, k_batteryHeight), Palette::YellowLight);
  }
  if (m_chargeState == Ion::Battery::Charge::FULL) {
    ctx->fillRect(KDRect(k_elementWidth+k_separatorThickness, 0, k_batteryWidth-3*k_elementWidth-2*k_separatorThickness, k_batteryHeight), KDColorWhite);
  }
  ctx->fillRect(KDRect(k_batteryWidth-2*k_elementWidth, 0, k_elementWidth, k_batteryHeight), KDColorWhite);
  ctx->fillRect(KDRect(k_batteryWidth-k_elementWidth, (k_batteryHeight-k_capHeight)/2, k_elementWidth, k_capHeight), KDColorWhite);
}

KDSize BatteryView::minimalSizeForOptimalDisplay() {
  return KDSize(k_batteryWidth, k_batteryHeight);
}

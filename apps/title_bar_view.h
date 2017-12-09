#ifndef APPS_TITLE_BAR_VIEW_H
#define APPS_TITLE_BAR_VIEW_H

#include <escher.h>
#include "battery_view.h"
#include "shift_alpha_lock_view.h"
#include "i18n.h"

class TitleBarView final : public View {
public:
  TitleBarView();
  void drawRect(KDContext * ctx, KDRect rect) const override;
  void setTitle(I18n::Message title) { m_titleView.setMessage(title); }
  bool setChargeState(Ion::Battery::Charge chargeState) { return m_batteryView.setChargeState(chargeState); }
  bool setIsCharging(bool isCharging) { return m_batteryView.setIsCharging(isCharging); }
  bool setIsPlugged(bool isPlugged) { return m_batteryView.setIsPlugged(isPlugged); }
  bool setShiftAlphaLockStatus(Ion::Events::ShiftAlphaStatus status) { return m_shiftAlphaLockView.setStatus(status); }
  void refreshPreferences();
private:
  constexpr static KDCoordinate k_alphaRightMargin = 5;
  constexpr static KDCoordinate k_examIconWidth = 18;
  constexpr static KDCoordinate k_examIconHeight = 9;
  constexpr static KDCoordinate k_examIconMargin = 93;
  int numberOfSubviews() const override { return 5; }
  void layoutSubviews() override;
  View * subviewAtIndex(int index) override;
  MessageTextView m_titleView;
  BatteryView m_batteryView;
  ShiftAlphaLockView m_shiftAlphaLockView;
  BufferTextView m_preferenceView;
  ImageView m_examModeIconView;
};

#endif

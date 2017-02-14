#ifndef APPS_TITLE_BAR_VIEW_H
#define APPS_TITLE_BAR_VIEW_H

#include <escher.h>
#include "battery_view.h"

class TitleBarView : public View {
public:
  TitleBarView();
  void drawRect(KDContext * ctx, KDRect rect) const override;
  void setTitle(const char * title);
  void setChargeState(Ion::Battery::Charge chargeState);
  void refreshPreferences();
private:
  constexpr static KDCoordinate k_batteryLeftMargin = 5;
  int numberOfSubviews() const override;
  void layoutSubviews() override;
  View * subviewAtIndex(int index) override;
  PointerTextView m_titleView;
  BatteryView m_batteryView;
  BufferTextView m_preferenceView;
};

#endif

#ifndef APPS_WINDOW_H
#define APPS_WINDOW_H

#include <escher.h>
#include "title_bar_view.h"

class AppsWindow final : public Window {
public:
  AppsWindow() : Window(), m_titleBarView(), m_hideTitleBarView(false) {}
  void setTitle(I18n::Message title) {
    m_titleBarView.setTitle(title);
  }
  bool updateBatteryLevel() {
    return m_titleBarView.setChargeState(Ion::Battery::level());
  }
  bool updateIsChargingState() {
    return m_titleBarView.setIsCharging(Ion::Battery::isCharging());
  }
  bool updatePluggedState() {
    return m_titleBarView.setIsPlugged(Ion::USB::isPlugged());
  }
  void refreshPreferences() {
    m_titleBarView.refreshPreferences();
  }
  bool updateAlphaLock() {
    return m_titleBarView.setShiftAlphaLockStatus(Ion::Events::shiftAlphaStatus());
  }
  void hideTitleBarView(bool hide);
private:
  int numberOfSubviews() const override;
  void layoutSubviews() override;
  View * subviewAtIndex(int index) override;
  TitleBarView m_titleBarView;
  bool m_hideTitleBarView;
};

#endif

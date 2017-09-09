#ifndef APPS_WINDOW_H
#define APPS_WINDOW_H

#include <escher.h>
#include "title_bar_view.h"

class AppsWindow : public Window {
public:
  AppsWindow();
  void setTitle(const I18n::Message *title);
  bool updateBatteryLevel();
  bool updateIsChargingState();
  bool updatePluggedState();
  void refreshPreferences();
  bool updateAlphaLock();
  void hideTitleBarView(bool hide);
  void reloadTitleBar();
private:
  constexpr static KDCoordinate k_titleBarHeight = 18;
  int numberOfSubviews() const override;
  void layoutSubviews() override;
  View * subviewAtIndex(int index) override;
  TitleBarView m_titleBarView;
  bool m_hideTitleBarView;
};

#endif

#ifndef APPS_WINDOW_H
#define APPS_WINDOW_H

#include <escher.h>
#include "title_bar_view.h"

class AppsWindow : public Window {
public:
  AppsWindow();
  void setTitle(I18n::Message title);
  bool updateBatteryLevel();
  bool updateIsChargingState();
  bool updatePluggedState();
  void refreshPreferences();
  void reloadTitleBarView();
  bool updateAlphaLock();
  void hideTitleBarView(bool hide);
private:
  int numberOfSubviews() const override;
  void layoutSubviews(bool force = false) override;
  View * subviewAtIndex(int index) override;
  TitleBarView m_titleBarView;
  bool m_hideTitleBarView;
};

#endif

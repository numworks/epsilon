#ifndef APPS_WINDOW_H
#define APPS_WINDOW_H

#include <escher/window.h>

#include "title_bar_view.h"

class AppsWindow : public Escher::Window {
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
  void updateBatteryAnimation();

 private:
#if ESCHER_VIEW_LOGGING
  const char* className() const override;
#endif
  int numberOfSubviews() const override;
  void layoutSubviews(bool force = false) override;
  Escher::View* subviewAtIndex(int index) override;
  TitleBarView m_titleBarView;
  bool m_hideTitleBarView;
};

#endif

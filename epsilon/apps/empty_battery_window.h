#ifndef APPS_EMPTY_BATTERY_WINDOW_H
#define APPS_EMPTY_BATTERY_WINDOW_H

#include <escher/window.h>

class EmptyBatteryWindow : public Escher::Window {
 public:
  EmptyBatteryWindow();
  void drawRect(KDContext* ctx, KDRect rect) const;
};

#endif

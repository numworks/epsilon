#ifndef APPS_EMPTY_BATTERY_WINDOW_H
#define APPS_EMPTY_BATTERY_WINDOW_H

#include <escher.h>

class EmptyBatteryWindow final : public Window {
public:
  EmptyBatteryWindow() : Window() {}
  void drawRect(KDContext * ctx, KDRect rect) const;
};

#endif

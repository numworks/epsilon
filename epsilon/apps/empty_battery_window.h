#pragma once

#include <escher/window.h>

class EmptyBatteryWindow : public Escher::Window {
 public:
  EmptyBatteryWindow();
  void drawRect(KDContext* ctx, KDRect rect) const;
};

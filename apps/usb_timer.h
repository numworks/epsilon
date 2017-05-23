#ifndef APPS_USB_TIMER_H
#define APPS_USB_TIMER_H

#include <escher.h>

class AppsContainer;

class USBTimer : public Timer {
public:
  USBTimer(AppsContainer * container);
private:
  bool fire() override;
  AppsContainer * m_container;
  bool m_previousPluggedState;
};

#endif


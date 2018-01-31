#include "usb_timer.h"
#include "global_preferences.h"
#include "apps_container.h"

USBTimer::USBTimer(AppsContainer * container) :
  Timer(1),
  m_container(container),
  m_previousPluggedState(false)
{
}

bool USBTimer::fire() {
  bool needRedrawing = false;
  if (Ion::USB::isPlugged()) {
    if (!m_previousPluggedState && GlobalPreferences::sharedGlobalPreferences()->examMode() == GlobalPreferences::ExamMode::Activate) {
      m_container->displayExamModePopUp(false);
      needRedrawing = true;
    }
#if LED_WHILE_CHARGING
    Ion::LED::setCharging(Ion::USB::isPlugged(), Ion::Battery::isCharging());
#endif
    if (!m_previousPluggedState) {
      Ion::Backlight::setBrightness(Ion::Backlight::MaxBrightness);
    }
    m_previousPluggedState = true;
  } else {
    if (m_previousPluggedState) {
#if LED_WHILE_CHARGING
      Ion::LED::setCharging(Ion::USB::isPlugged(), Ion::Battery::isCharging());
#endif
      m_previousPluggedState = false;
    }
  }
  return needRedrawing;
}

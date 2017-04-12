#include "usb_timer.h"
#include "global_preferences.h"
#include "apps_container.h"

USBTimer::USBTimer(AppsContainer * container) :
  Timer(1),
  m_container(container),
  m_previousPluggedState(false)
{
}

void USBTimer::fire() {
  if (Ion::USB::isPlugged()) {
    if (!m_previousPluggedState && GlobalPreferences::sharedGlobalPreferences()->examMode() == GlobalPreferences::ExamMode::Activate) {
      m_container->displayExamModePopUp(false, true);
    }
    KDColor LEDColor = Ion::Battery::isCharging() ? KDColorYellow : KDColorGreen;
    Ion::LED::setColor(LEDColor);
    m_previousPluggedState = true;
  } else {
    Ion::LED::setColor(KDColorBlack);
    m_previousPluggedState = false;
  }
}

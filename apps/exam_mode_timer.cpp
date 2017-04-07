#include "exam_mode_timer.h"
#include "global_preferences.h"
#include "apps_container.h"

ExamModeTimer::ExamModeTimer(AppsContainer * container) :
  Timer(3),
  m_container(container),
  m_previousPluggedState(false)
{
}

void ExamModeTimer::fire() {
  if (!m_previousPluggedState && Ion::USB::isPlugged() && GlobalPreferences::sharedGlobalPreferences()->examMode() == GlobalPreferences::ExamMode::Activate) {
    m_container->displayExamModePopUp(false, true);
    m_previousPluggedState = true;
  }
  if (m_previousPluggedState && !Ion::USB::isPlugged()) {
    m_previousPluggedState = false;
  }
}

#ifndef APPS_EXAM_MODE_TIMER_H
#define APPS_EXAM_MODE_TIMER_H

#include <escher.h>

class AppsContainer;

class ExamModeTimer : public Timer {
public:
  ExamModeTimer(AppsContainer * container);
private:
  void fire() override;
  AppsContainer * m_container;
  bool m_previousPluggedState;
};

#endif


#ifndef APPS_BACKLIGHT_DIMMING_TIMER_H
#define APPS_BACKLIGHT_DIMMING_TIMER_H

#include <escher.h>

class BacklightDimmingTimer : public Timer {
public:
  BacklightDimmingTimer();
  void reset();
private:
  constexpr static int k_idleBeforeDimmingDuration = 30*1000; // In miliseconds
  constexpr static int k_dimBacklightBrightness = 0;
  constexpr static int decreaseBy = 15;
  constexpr static int decreasetime = 1*1000; // In miliseconds
  int m_dimerExecutions = 0;
  int m_brightnessLevel;
  int m_dimerSteps;
  int m_nextbrightness;
  float m_timeToSleep; // In miliseconds
  bool fire() override;
  void resetTimer();
};

#endif


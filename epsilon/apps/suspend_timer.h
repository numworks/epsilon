#pragma once

#include <escher/timer.h>

class SuspendTimer : public Escher::Timer {
 public:
  SuspendTimer();

 private:
  // In miliseconds
  constexpr static int k_idleBeforeSuspendDuration = 5 * 60 * 1000;
  bool fire() override;
};

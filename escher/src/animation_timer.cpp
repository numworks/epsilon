#include <apps/apps_container.h>
#include <escher/animation_timer.h>
#include <escher/timer_manager.h>


void AnimationTimer::setAnimated(Animated * animated) {
  m_animated = animated;
  TimerManager::AddTimer(this);
}

void AnimationTimer::removeAnimated(Animated * animated) {
  if (m_animated == animated || animated == nullptr) {
    m_animated = nullptr;
    TimerManager::RemoveTimer(this);
  }
}

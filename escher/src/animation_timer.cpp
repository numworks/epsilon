#include <apps/apps_container.h>
#include <escher/animation_timer.h>


void AnimationTimer::setAnimated(Animated * animated) {
  m_animated = animated;
  AppsContainer::sharedAppsContainer()->addTimer(this);
}

void AnimationTimer::removeAnimated(Animated * animated) {
  if (m_animated == animated || animated == nullptr) {
    m_animated = nullptr;
    AppsContainer::sharedAppsContainer()->removeTimer(this);
  }
}

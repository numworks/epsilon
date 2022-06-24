#ifndef APPS_ANIMATION_TIMER_H
#define APPS_ANIMATION_TIMER_H

#include <escher/timer.h>
#include <escher/animated.h>
#include <assert.h>

class AnimationTimer : public Timer {
public:
  AnimationTimer():
    Timer(1),
    m_animated(nullptr)
  {}
  void setAnimated(Animated * animated);
  void removeAnimated(Animated * animated=nullptr);
private:
  bool fire() override {
    assert(m_animated);
    m_animated->animate();
    return true;
  }
  Animated * m_animated;
};

#endif

#ifndef APPS_ANIMATED_H
#define APPS_ANIMATED_H

class Animated {
public:
  virtual void willStartAnimation() {};
  virtual void didStopAnimation() {};
  virtual void animate() = 0;
};

#endif

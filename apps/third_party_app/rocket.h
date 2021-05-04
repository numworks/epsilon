#ifndef ROCKET_H
#define ROCKET_H

#include "color.h"
#include "display.h"

class Alien;

class Rocket {
public:
  Rocket() : m_x(-1), m_y(-1) {}
  int x() const { return m_x; }
  int y() const { return m_y; }
  void setLocation(int x, int y);
  void forward();
  void tryToKill(Alien * a);
  bool off() const { return m_y < 0; }
  static constexpr int k_period = 10;
private:
  static constexpr int k_length = 10;
  void draw(const Color c) const;
  void switchOff() { m_y = -1; }
  int m_x;
  int m_y;
};

#endif

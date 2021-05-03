#ifndef ALIEN_H
#define ALIEN_H

#include "color.h"
#include "display.h"
#include "spaceship.h"

class Alien {
public:
  Alien(int x = -1);
  int x() const { return m_x; }
  int y() const { return m_y; }
  void draw(const Color c) const;
  void step();
  bool hits(Spaceship * s);
  bool isGhost() const { return m_x < 0; }
  void killed();
  static constexpr int k_stepPeriod = 50;
  static constexpr int k_materializationPeriod = 200;
  static constexpr int k_size = 12;
private:
  void ghostify() { m_x = -1; }
  static constexpr int k_step = 10;
  int m_x;
  int m_y;
};

#endif

#ifndef ALIEN_H
#define ALIEN_H

#include "eadkpp.h"
#include "spaceship.h"

class Alien {
public:
  Alien(int x = -1);
  int x() const { return m_x; }
  int y() const { return m_y; }
  void step();
  bool tryToHit(Spaceship * s);
  bool isGhost() const { return m_x == -1; }
  void killed();
  static constexpr int k_stepPeriod = 10;
  static constexpr int k_materializationPeriod = 20;
  static constexpr int k_width = 15;
  static constexpr int k_height = 18;
private:
  void draw(const EADK::Color c) const;
  void hide() const;
  void ghostify() { m_x = -1; }
  static constexpr int k_step = 10;
  int m_x;
  int m_y;
};

#endif

#ifndef SPACESHIP_H
#define SPACESHIP_H

#include "color.h"
#include "display.h"
#include "life.h"
#include "rocket.h"

class Spaceship {
public:
  Spaceship();
  int x() const { return m_x; }
  int y() const { return m_y; }
  void move(int deltaX, int deltaY);
  bool hit();
  void createRocket();
  int numberOfRockets() const { return k_maxNumberOfRockets; }
  Rocket * rocketAtIndex(int index) { return &m_rockets[index]; }
  static constexpr int k_step = 10;
  static constexpr int k_width = 15;
  static constexpr int k_height = 12;
private:
  static constexpr int k_maxNumberOfRockets = 12;
  static constexpr int k_maxNumberOfLives = 3;
  static constexpr int k_xLowerBound = Display::CommonHorizontalMargin;
  static constexpr int k_xUpperBound = Display::Width - Display::CommonHorizontalMargin;
  static constexpr int k_yLowerBound = 3*Display::CommonVerticalMargin;
  static constexpr int k_yUpperBound = Display::Height - Display::CommonVerticalMargin;
  void draw(const Color c) const;
  int m_x;
  int m_y;
  Rocket m_rockets[k_maxNumberOfRockets];
  int m_numberOfLives;
  Life m_lives[k_maxNumberOfLives];
};

#endif

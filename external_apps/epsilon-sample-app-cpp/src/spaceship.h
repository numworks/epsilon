#ifndef SPACESHIP_H
#define SPACESHIP_H

#include "display.h"
#include "eadkpp.h"
#include "life.h"
#include "rocket.h"
#include "score.h"

class Spaceship {
public:
  Spaceship();
  int x() const { return m_x; }
  int y() const { return m_y; }
  void move(int deltaX, int deltaY);
  bool hit();
  void createRockets();
  void rocketsAction(Alien aliens[], int numberOfAliens);
  void checkForRocketsAliensCollisions(Alien aliens[], int numberOfAliens);
  static constexpr int k_step = 10;
  static constexpr int k_width = 35;
  static constexpr int k_height = 20;
private:
  static constexpr int k_maxNumberOfRockets = 3*50;
  static constexpr int k_maxNumberOfLives = 3;
  static constexpr int k_xLowerBound = Display::CommonHorizontalMargin;
  static constexpr int k_xUpperBound = EADK::Screen::Width - Display::CommonHorizontalMargin;
  static constexpr int k_yLowerBound = 3*Display::CommonVerticalMargin;
  static constexpr int k_yUpperBound = EADK::Screen::Height - Display::CommonVerticalMargin;
  void draw(const EADK::Color c) const;
  void redrawLives();
  int m_x;
  int m_y;
  Rocket m_rockets[k_maxNumberOfRockets];
  int m_numberOfLives;
  Life m_lives[k_maxNumberOfLives];
  Score m_score;
};

#endif

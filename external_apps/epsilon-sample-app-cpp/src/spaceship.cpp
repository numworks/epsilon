#include "spaceship.h"
#include "alien.h"
#include "palette.h"

Spaceship::Spaceship() :
  m_x(EADK::Screen::Width/2),
  m_y(EADK::Screen::Height - Display::CommonVerticalMargin),
  m_numberOfLives(k_maxNumberOfLives)
{

  draw(Yellow);
  for (int i = 0; i < k_maxNumberOfLives; i++) {
    m_lives[i].setIndex(i);
    m_lives[i].draw();
  }
}

void Spaceship::draw(const EADK::Color color) const {
  int xMin = m_x - k_width/2;
  int xMax = xMin + k_width;
  int yMin = m_y - k_height/2;
  EADK::Display::pushRectUniform(EADK::Rect(xMin + 11, yMin + 10, 13, 11), color);
  // Wings
  EADK::Display::pushRectUniform(EADK::Rect(xMin, yMin + 14, k_width, 2), color);
  EADK::Display::pushRectUniform(EADK::Rect(xMin + 3, yMin + 17, k_width - 6, 2), color);
  EADK::Display::pushRectUniform(EADK::Rect(xMin + 2, yMin + 8, 1, 6), color);
  EADK::Display::pushRectUniform(EADK::Rect(xMax - 3, yMin + 8, 1, 6), color);
  // Nose
  EADK::Display::pushRectUniform(EADK::Rect(xMin + 15, yMin + 6, 5, 4), color);
  EADK::Display::pushRectUniform(EADK::Rect(xMin + 16, yMin + 4, 3, 2), color);
  EADK::Display::pushRectUniform(EADK::Rect(xMin + 17, yMin, 1, 4), color);
}

void Spaceship::move(int deltaX, int deltaY) {
  draw(Black);
  m_x += deltaX;
  m_y += deltaY;
  m_x = m_x <= k_xLowerBound ? k_xLowerBound : m_x;
  m_y = m_y <= k_yLowerBound ? k_yLowerBound : m_y;
  m_x = m_x >= k_xUpperBound ? k_xUpperBound : m_x;
  m_y = m_y >= k_yUpperBound ? k_yUpperBound : m_y;
  draw(Yellow);
}

bool Spaceship::hit() {
  m_lives[m_numberOfLives - 1].breaks();
  m_numberOfLives--;
  for (int i = 0; i < 5; i++) {
    draw(Red);
    EADK::Timing::msleep(10);
    draw(Yellow);
    EADK::Timing::msleep(10);
  }
  return m_numberOfLives == 0;
}

void Spaceship::createRockets() {
  int deltaX[] = {-15, 0, 14};
  for (int dx : deltaX) {
    int x = m_x + dx;
    int y = m_y - k_height;
    for (int i = 0; i < k_maxNumberOfRockets; i++) {
      if (m_rockets[i].x() == x && m_rockets[i].y() == y) {
        // A rocket has already been launched at this location
        continue;
      }
    }
    // Launch the first available rocket
    for (int i = 0; i < k_maxNumberOfRockets; i++) {
      if (m_rockets[i].off()) {
        m_rockets[i].setLocation(x, y);
        break;
      }
    }
  }
}

void Spaceship::rocketsAction(Alien aliens[], int numberOfAliens) {
  for (int i = 0; i < k_maxNumberOfRockets; i++) {
    m_rockets[i].forward();
  }
  checkForRocketsAliensCollisions(aliens, numberOfAliens);
  redrawLives();

}

void Spaceship::redrawLives() {
  for (int i = 0; i < k_maxNumberOfLives; i++) {
    m_lives[i].draw();
  }
}

void Spaceship::checkForRocketsAliensCollisions(Alien aliens[], int numberOfAliens) {
  for (int i = 0; i < k_maxNumberOfRockets; i++) {
    for (int j = 0; j < numberOfAliens; j++) {
      if (m_rockets[i].tryToKill(&aliens[j])) {
        m_score.increment();
      }
    }
  }
  m_score.draw();
}

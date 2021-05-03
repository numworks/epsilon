#include "spaceship.h"
#include "svc.h"

Spaceship::Spaceship() :
  m_x(Display::Width/2),
  m_y(Display::Height - Display::CommonVerticalMargin),
  m_numberOfLives(k_maxNumberOfLives)
{

  draw(Grey);
  for (int i = 0; i < k_maxNumberOfLives; i++) {
    m_lives[i].setIndex(i);
    m_lives[i].draw();
  }
}
void Spaceship::draw(const Color color) const {
  Ion::Display::pushRectUniform(Rect(m_x - k_width/2, m_y - k_height/2, k_width, k_height), color);
}

void Spaceship::move(int deltaX, int deltaY) {
  draw(DarkBlue);
  m_x += deltaX;
  m_y += deltaY;
  m_x = m_x <= k_xLowerBound ? k_xLowerBound : m_x;
  m_y = m_y <= k_yLowerBound ? k_yLowerBound : m_y;
  m_x = m_x >= k_xUpperBound ? k_xUpperBound : m_x;
  m_y = m_y >= k_yUpperBound ? k_yUpperBound : m_y;
  draw(Grey);
}

bool Spaceship::hit() {
  m_lives[m_numberOfLives - 1].breaks();
  m_numberOfLives = m_numberOfLives - 1;
  for (int i = 0; i < 5; i++) {
    draw(Red);
    Ion::Timing::msleep(10);
    draw(Grey);
    Ion::Timing::msleep(10);
  }
  return m_numberOfLives == 0;
}

void Spaceship::createRocket() {
  int x = m_x;
  int y = m_y - k_height;
  for (int i = 0; i < k_maxNumberOfRockets; i++) {
    if (m_rockets[i].x() == x && m_rockets[i].y() == y) {
      // A rocket has already been launched at this location
      return;
    }
  }
  for (int i = 0; i < k_maxNumberOfRockets; i++) {
    if (m_rockets[i].off()) {
      m_rockets[i].setLocation(x, y);
      return;
    }
  }
}

#include "alien.h"
#include "../utils/stdlib.h"
#include "../utils/svc.h"

Alien::Alien(int x) :
  m_x(x),
  m_y(2*Display::CommonVerticalMargin)
{
  if (!isGhost()) {
    draw(Orange);
  }
}

void Alien::hide() const {
  Ion::Display::pushRectUniform(Rect(m_x - k_width/2, m_y - k_height/2, k_width, k_height), Black);
}

void Alien::draw(const Color c) const {
  int xMin = m_x - k_width/2;
  int xMax = xMin + k_width;
  int yMin = m_y - k_height/2;
  int yMax = yMin + k_height;
  Ion::Display::pushRectUniform(Rect(xMin, yMin, k_width, k_height - 4), c);
  Ion::Display::pushRectUniform(Rect(xMin + 3, yMax - 4, 2, 4), c);
  Ion::Display::pushRectUniform(Rect(xMax - 4 - 2, yMax - 4, 2, 4), c);
  Ion::Display::pushRectUniform(Rect(xMin + 2, yMin + 3, 3, 7), Black);
  Ion::Display::pushRectUniform(Rect(xMax - 2 - 3, yMin + 3, 3, 7), Black);
}

void Alien::step() {
  if (!isGhost()) {
    hide();
    m_y += k_step;
    if (m_y >= Display::Height) {
      ghostify();
    }
    if (!isGhost()) {
      draw(Orange);
    }
  }
}

bool Alien::hits(Spaceship * s) {
  if (isGhost()) {
   return false;
  }
  if (abs(m_x - s->x()) < Spaceship::k_width/2 + k_width/2 &&
      abs(m_y - s->y()) < Spaceship::k_height/2 + k_height/2) {
    hide();
    ghostify();
    return s->hit();
  }
  return false;
}

void Alien::killed() {
  if (isGhost()) {
    return;
  }
  for (int i = 0; i < 5; i++) {
    draw(Green);
    Ion::Timing::msleep(10);
    draw(Orange);
    Ion::Timing::msleep(10);
  }
  hide();
  ghostify();
}

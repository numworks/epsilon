#include "alien.h"
#include "stdlib.h"
#include "svc.h"

Alien::Alien(int x) :
  m_x(x),
  m_y(2*Display::CommonVerticalMargin)
{
  draw(Green);
}


void Alien::draw(const Color c) const {
  if (isGhost()) {
   return;
  }
  Ion::Display::pushRectUniform(Rect(m_x - k_size/2, m_y - k_size/2, k_size, k_size), c);
}

void Alien::step() {
  draw(DarkBlue);
  if (!isGhost()) {
    m_y += k_step;
    if (m_y >= Display::Height) {
      ghostify();
    }
    draw(Green);
  }
}

bool Alien::hits(Spaceship * s) {
  if (isGhost()) {
   return false;
  }
  if (abs(m_x - s->x()) < Spaceship::k_width/2 + k_size/2 &&
      abs(m_y - s->y()) < Spaceship::k_height/2 + k_size/2) {
    int deltax = abs(m_x - s->x());
    int deltay = abs(m_y - s->y());
    int X = Spaceship::k_width/2 + k_size/2;
    int Y = Spaceship::k_height/2 + k_size/2;
    draw(DarkBlue);
    ghostify();
    return s->hit();
  }
  return false;
}

void Alien::killed() {
  for (int i = 0; i < 5; i++) {
    draw(Red);
    Ion::Timing::msleep(10);
    draw(Green);
    Ion::Timing::msleep(10);
  }
  draw(DarkBlue);
  ghostify();
}

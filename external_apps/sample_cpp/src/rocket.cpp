#include "rocket.h"
#include "alien.h"
#include "palette.h"
#include "stdlib.h"

void Rocket::setLocation(int x, int y) {
  m_x = x;
  m_y = y;
  draw(LightBlue);
}

void Rocket::draw(const EADK::Color c) const {
  if (off()) {
    return;
  }
  EADK::Display::pushRectUniform(EADK::Rect(m_x, m_y - k_length/2, 1, 8), c);
  EADK::Display::pushRectUniform(EADK::Rect(m_x, m_y - k_length/2 + 8 + 1, 1, 2), c);
  EADK::Display::pushRectUniform(EADK::Rect(m_x, m_y + k_length/2, 1, 1), c);
}

void Rocket::forward() {
  draw(Black);
  if (off()) {
    return;
  }
  m_y -= k_length;
  draw(LightBlue);
}

bool Rocket::tryToKill(Alien * a) {
  if (off() || a->isGhost()) {
    return false;
  }
  if (abs(m_x - a->x()) < Alien::k_width/2 &&
      abs(m_y - a->y()) < Alien::k_height/2 + k_length/2) {
    a->killed();
    draw(Black);
    switchOff();
    return true;
  }
  return false;
}

#include "rocket.h"
#include "alien.h"
#include "../utils/stdlib.h"
#include "../utils/svc.h"

void Rocket::setLocation(int x, int y) {
  m_x = x;
  m_y = y;
  draw(LightBlue);
}

void Rocket::draw(const Color c) const {
  if (off()) {
    return;
  }
  Ion::Display::pushRectUniform(Rect(m_x, m_y - k_length/2, 1, 8), c);
  Ion::Display::pushRectUniform(Rect(m_x, m_y - k_length/2 + 8 + 1, 1, 2), c);
  Ion::Display::pushRectUniform(Rect(m_x, m_y + k_length/2, 1, 1), c);
}

void Rocket::forward() {
  draw(Black);
  if (!off()) {
    m_y -= k_length;
  }
  draw(LightBlue);
}

void Rocket::tryToKill(Alien * a) {
  if (off() || a->isGhost()) {
    return;
  }
  if (abs(m_x - a->x()) < Alien::k_width/2 &&
      abs(m_y - a->y()) < Alien::k_height/2 + k_length/2) {
    a->killed();
    draw(Black);
    switchOff();
  }
}

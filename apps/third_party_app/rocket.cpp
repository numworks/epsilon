#include "rocket.h"
#include "alien.h"
#include "stdlib.h"
#include "svc.h"

void Rocket::setLocation(int x, int y) {
  m_x = x;
  m_y = y;
  draw(Red);
}

void Rocket::draw(const Color c) const {
  if (off()) {
    return;
  }
  Ion::Display::pushRectUniform(Rect(m_x, m_y - k_length/2, 1, k_length), c);
}

void Rocket::forward() {
  draw(DarkBlue);
  if (!off()) {
    m_y -= k_length;
  }
  draw(Red);
}

void Rocket::tryToKill(Alien * a) {
  if (off() || a->isGhost()) {
    return;
  }
  if (abs(m_x - a->x()) < Alien::k_size/2 &&
      abs(m_y - a->y()) < Alien::k_size/2 + k_length/2) {
    a->killed();
    draw(DarkBlue);
    switchOff();
  }
}

#include "life.h"
#include "svc.h"

void Life::setIndex(int index) {
  m_x = Display::Width - (index + 1) * Display::CommonHorizontalMargin;
}

void Life::draw() const {
  Color c = m_broken ? DarkBlue : Pink;
  Ion::Display::pushRectUniform(Rect(m_x - k_size/2, m_y - k_size/2, k_size, k_size), c);
}

void Life::breaks() {
  m_broken = true;
  draw();
}

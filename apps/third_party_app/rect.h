#ifndef RECT_H
#define RECT_H

#include "stdint.h"

class Rect {
public:
  constexpr Rect(uint16_t x, uint16_t y,
      uint16_t width, uint16_t height) :
    m_x(x), m_y(y), m_width(width), m_height(height) {}

  uint16_t x() const { return m_x; }
  uint16_t y() const { return m_y; }
  uint16_t width() const { return m_width; }
  uint16_t height() const { return m_height; }
  uint16_t top() const { return m_y; }
  uint16_t right() const { return m_x+m_width-1; }
  uint16_t bottom() const { return m_y+m_height-1; }
  uint16_t left() const { return m_x; }

private:
  uint16_t m_x, m_y, m_width, m_height;
};

#endif

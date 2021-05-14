#ifndef COLOR_H
#define COLOR_H

#include "stdint.h"

class Color {
public:
  constexpr Color() : Color(0) {}
  static constexpr Color RGB24(uint32_t rgb) {
    return Color(((rgb&0xF80000)>>8)|((rgb&0x00FC00)>>5)|((rgb&0x0000F8)>>3));
  }
  static constexpr Color RGB888(uint8_t r, uint8_t g, uint8_t b) {
    return Color((r>>3)<<11 | (g>>2) << 5 | (b>>3));
  }
  uint8_t red() const {
    uint8_t r5 = (m_value>>11)&0x1F;
    return r5 << 3;
  }

  uint8_t green() const {
    uint8_t g6 = (m_value>>5)&0x3F;
    return g6 << 2;
  }

  uint8_t blue() const {
    uint8_t b5 = m_value&0x1F;
    return b5 << 3;
  }
private:
  constexpr Color(uint16_t value) : m_value(value) {}
  uint16_t m_value;
};

constexpr static Color Black = Color::RGB24(0x000000);
constexpr static Color Orange = Color::RGB24(0xD55422);
constexpr static Color Green = Color::RGB24(0x28F52E);
constexpr static Color Grey = Color::RGB24(0xF7F7F7);
constexpr static Color Pink = Color::RGB24(0xCB6E79);
constexpr static Color Red = Color::RGB24(0xFF0000);
constexpr static Color White = Color::RGB24(0xFFFFFF);
constexpr static Color LightBlue = Color::RGB24(0XC9DBFD);

#endif

#ifndef KANDINSKY_COLOR_H
#define KANDINSKY_COLOR_H

#include <stdint.h>

class KDColor {
public:
  constexpr KDColor()
    : m_value(0)
  {
  }

  static constexpr KDColor RGB16(uint16_t rgb16)
  {
    return KDColor(rgb16);
  }

  static constexpr KDColor RGB24(uint32_t rgb24)
  {
    return KDColor ( ((rgb24 >> 8) & 0xF800)
                   | ((rgb24 >> 5) & 0x07E0)
                   | ((rgb24 >> 3) & 0x001F) );
  }

  static constexpr KDColor RGB888(uint8_t r, uint8_t g, uint8_t b)
  {
    return KDColor ( ((r << 8) & 0xF800)
                   | ((g << 3) & 0x07E0)
                   | ((b >> 3) & 0x001F) );
  }

  uint8_t red() const
  {
    const uint8_t r5 = (m_value >> 11) & 0x1F;
    const uint8_t r8 = (r5 << 3) | (r5 & 0x07);

    return r8;
  }

  uint8_t green() const
  {
    const uint8_t g6 = (m_value >> 5) & 0x3F;
    const uint8_t g8 = (g6 << 2) | (g6 & 0x03);

    return g8;
  }

  uint8_t blue() const
  {
    const uint8_t b5 = (m_value >> 0) & 0x1F;
    const uint8_t b8 = (b5 << 3) | (b5 & 0x07);

    return b8;
  }

  operator uint16_t() const
  {
    return m_value;
  }

  static KDColor blend(const KDColor fg, const KDColor bg, const uint8_t alpha);

private:
  constexpr KDColor(const uint16_t value)
    : m_value(value)
  {
  }

private:
  uint16_t m_value;
};

constexpr KDColor KDColorBlack  = KDColor::RGB24(0x000000);
constexpr KDColor KDColorWhite  = KDColor::RGB24(0xFFFFFF);
constexpr KDColor KDColorRed    = KDColor::RGB24(0xFF0000);
constexpr KDColor KDColorGreen  = KDColor::RGB24(0x00FF00);
constexpr KDColor KDColorBlue   = KDColor::RGB24(0x0000FF);
constexpr KDColor KDColorYellow = KDColor::RGB24(0xFFFF00);

#endif

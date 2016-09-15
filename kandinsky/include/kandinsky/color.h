#ifndef KANDINSKY_COLOR_H
#define KANDINSKY_COLOR_H

#include <stdint.h>

class KDColor {
public:
  constexpr KDColor() : m_value(0) {}
  // FIXME: This should not be needed, and is probably wasting CPU cycles

  constexpr KDColor(uint32_t rgb)
    : m_value(((rgb&0xF80000)>>8)|((rgb&0x00FC00)>>5)|((rgb&0x0000F8)>>3)) {}
  constexpr KDColor(uint8_t r, uint8_t g, uint8_t b)
    : m_value((r>>3)<<11 | (g>>2) << 5 | (b>>3)) {}
  uint8_t red();
  uint8_t green();
  uint8_t blue();
  static KDColor blend(KDColor first, KDColor second, uint8_t alpha);
  operator uint16_t() const { return m_value; }
private:
  uint16_t m_value;
};

constexpr KDColor KDColorBlack = KDColor(0x000000);
constexpr KDColor KDColorWhite = KDColor(0xFFFFFF);
constexpr KDColor KDColorRed = KDColor(0xFF0000);
constexpr KDColor KDColorGreen = KDColor(0x00FF00);
constexpr KDColor KDColorBlue = KDColor(0x0000FF);

#endif

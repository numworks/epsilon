#ifndef KANDINSKY_COLOR_H
#define KANDINSKY_COLOR_H

#include <stdint.h>

class KDColor {
public:
  constexpr KDColor() : m_value(0) {}
  // FIXME: This should not be needed, and is probably wasting CPU cycles
  constexpr static KDColor RGB16(uint16_t rgb) {
    return KDColor(rgb);
  }
  constexpr static KDColor RGB24(uint32_t rgb) {
    return KDColor(((rgb&0xF80000)>>8)|((rgb&0x00FC00)>>5)|((rgb&0x0000F8)>>3));
  }
  constexpr static KDColor RGB888(uint8_t r, uint8_t g, uint8_t b) {
    return KDColor((r>>3)<<11 | (g>>2) << 5 | (b>>3));
  }
  uint8_t red() const {
    uint8_t r5 = (m_value>>11)&0x1F;
    return Expand(r5, 5);
  }

  uint8_t green() const {
    uint8_t g6 = (m_value>>5)&0x3F;
    return Expand(g6, 6);
  }

  uint8_t blue() const {
    uint8_t b5 = m_value&0x1F;
    return Expand(b5, 5);
  }

  static KDColor Blend(KDColor first, KDColor second, uint8_t alpha);
  operator uint16_t() const { return m_value; }

  static KDColor HSVBlend(KDColor color1, KDColor color2);

private:
  /* When converting from RGB565 to RGB888 we need to artificially expand the
   * bit precision of each color channel. For example, we need to convert a 5
   * bit red color into an 8 bit one.
   * Of course we cannot create information out of thin air, so a simple way to
   * convert the value would be to just pad them with zeros. From a theoretical
   * standpoint, this is as good as any other alogrithm.
   * But in practices, this yields a rather visible problem : whites end up
   * being too dark. For example, converting a 5 bit pure white (value 0b11111)
   * to 8 bit would give 0b11111000, which is quite different from a pure white
   * in 8 bit (0b11111111). And if padded with ones instead, we would have a
   * similar problem with blacks.
   * Enters a simple trick: fill the padding with the most significant bits from
   * the original value. For example, to expand 0b101010 from 6 to 8 bits we
   * would append the first two bits (0b10), resulting in 0b10101010. This way,
   * full blacks remain black, and full whites remain whites. Yay, contrast! */
  constexpr static uint8_t Expand(uint8_t s, uint8_t nBits) {
    return
    (s << (8-nBits)) // Normal, zero-padded shifted value
    |
    (s >> (nBits-(8-nBits))); // Trick: let's try and fill the padding
  }
  constexpr KDColor(uint16_t value) : m_value(value) {}

  struct HSVColor {
    double H;
    double S;
    double V;
  };
  HSVColor convertToHSV() const;
  static KDColor ConvertHSVToRGB(HSVColor color);

  uint16_t m_value;
};

constexpr KDColor KDColorBlack = KDColor::RGB24(0x000000);
constexpr KDColor KDColorWhite = KDColor::RGB24(0xFFFFFF);
constexpr KDColor KDColorRed = KDColor::RGB24(0xFF0000);
constexpr KDColor KDColorGreen = KDColor::RGB24(0x00FF00);
constexpr KDColor KDColorBlue = KDColor::RGB24(0x0000FF);
constexpr KDColor KDColorYellow = KDColor::RGB24(0xFFFF00);
constexpr KDColor KDColorOrange = KDColor::RGB24(0xFF9900);
constexpr KDColor KDColorPurple = KDColor::RGB24(0xFF00DD);

#endif

#include <assert.h>
#include <escher/palette.h>
#include <kandinsky/color.h>
#include <quiz.h>

#include <array>
#include <cmath>

QUIZ_CASE(kandinsky_color_rgb) {
  // We really want KDColor to be packed
  quiz_assert(sizeof(KDColor) == 2);

  quiz_assert(KDColor::RGB24(0xFF0000) == 0xF800);
  quiz_assert(KDColor::RGB24(0x00FF00) == 0x07E0);
  quiz_assert(KDColor::RGB24(0x0000FF) == 0x1F);
  /* R = 0x12 = 0b 0001 0010. 5 most important bits are 00010.
   * G = 0x34 = 0b 0011 0100. 6 most important bits are 001101.
   * B = 0x56 = 0b 0101 0110. 5 most important bits are 01010.
   * KDColor = 0b 00010 001101 01010
   *         = 0b 0001 0001 1010 1010
   *         = 0x    1    1 A    A */
  quiz_assert(KDColor::RGB24(0x123456) == 0x11AA);
}

void assert_colors_blend_to(KDColor c1, KDColor c2, uint8_t alpha,
                            KDColor res) {
  quiz_assert(KDColor::Blend(c1, c2, alpha) == res);
}

QUIZ_CASE(kandinsky_color_blend) {
  KDColor midGray = KDColor::RGB24(0x7F7F7F);
  KDColor midTurquoise = KDColor::RGB24(0x007F80);
  /* Note: midTurquoise is the blending of 0x7F/0xFF Green and Blue. In other
   * words, it's 0x7F/0xFF*Green + (0xFF-0x7F)/0xFF*Blue
   *           = 0x7F/0xFF*Green + 0x80/0xFF*Blue
   *           = 0x7F*(0x000100) + 0x80*(0x000001)
   *           = 0x007F80 */

  assert_colors_blend_to(KDColorWhite, KDColorBlack, 0x00, KDColorBlack);
  assert_colors_blend_to(KDColorWhite, KDColorBlack, 0xFF, KDColorWhite);
  assert_colors_blend_to(KDColorWhite, KDColorBlack, 0x7F, midGray);

  assert_colors_blend_to(KDColorGreen, KDColorBlue, 0x00, KDColorBlue);
  assert_colors_blend_to(KDColorGreen, KDColorBlue, 0xFF, KDColorGreen);
  assert_colors_blend_to(KDColorGreen, KDColorBlue, 0x7F, midTurquoise);

  // Assert that blending two identical colors does not produce strange colors.
  for (uint16_t col = 0; col < 0xFFFF; col++) {
    KDColor color = KDColor::RGB16(col);
    assert_colors_blend_to(color, color, col >> 8, color);
  }
}

QUIZ_CASE(kandinsky_color_hsv) {
  // Check if hsv conversion can be reverted to same color
  uint16_t c = 0;
  while (true) {
    KDColor color = KDColor::RGB16(static_cast<uint16_t>(c));
    quiz_assert(KDColor::ConvertHSVToRGB(color.convertToHSV()) == color);
    if (c == 0xffff) {
      break;
    }
    c++;
  }

  /* Check for some values if conversion is correct
   *
   * WARNING: The RGB value given in escher/palette.h is not the real
   * RGB value of the color since colors are coded on uint16_t in epsilon.
   * The following HSV conversions are the conversions of the real RGB values.
   * */
  constexpr static KDColor::HSVColor dataColorsConversion[] = {
      KDColor::HSVColor({357.0, 1.0, 255.0}),    // Red
      KDColor::HSVColor({226.0, 0.669, 242.0}),  // Blue
      KDColor::HSVColor({95.0, 0.99, 193.0}),    // Green
      KDColor::HSVColor({39.0, 0.808, 255.0}),   // YellowDark
      KDColor::HSVColor({327.0, 0.984, 255.0}),  // Magenta
      KDColor::HSVColor({198.0, 0.593, 236.0}),  // Turquoise
      KDColor::HSVColor({352.0, 0.329, 255.0}),  // Pink
      KDColor::HSVColor({28.0, 0.878, 254.0}),   // Orange
  };
  constexpr static int nRows = Escher::Palette::numberOfDataColors();
  static_assert(std::size(dataColorsConversion) == nRows);
  for (int i = 0; i < nRows; i++) {
    KDColor conversionColor = KDColor::ConvertHSVToRGB(dataColorsConversion[i]);
    KDColor dataColor = Escher::Palette::DataColor[i];
    quiz_assert(conversionColor == dataColor);
  }
}

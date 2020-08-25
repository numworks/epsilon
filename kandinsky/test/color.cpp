#include <quiz.h>
#include <kandinsky.h>
#include <assert.h>

QUIZ_CASE(kandinsky_color_rgb) {
  quiz_assert(sizeof(KDColor) == 2); // We really want KDColor to be packed

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

void assert_colors_blend_to(KDColor c1, KDColor c2, uint8_t alpha, KDColor res) {
  quiz_assert(KDColor::blend(c1, c2, alpha) == res );
}

QUIZ_CASE(kandinsky_color_blend) {
  KDColor midGray = KDColor::RGB24(0x7F7F7F);
  KDColor midTurquoise = KDColor::RGB24(0x007F7F);

  assert_colors_blend_to(KDColorWhite, KDColorBlack, 0x00, KDColorBlack);
  assert_colors_blend_to(KDColorWhite, KDColorBlack, 0xFF, KDColorWhite);
  assert_colors_blend_to(KDColorWhite, KDColorBlack, 0x7F, midGray);

  assert_colors_blend_to(KDColorGreen, KDColorBlue, 0x00, KDColorBlue);
  assert_colors_blend_to(KDColorGreen, KDColorBlue, 0xFF, KDColorGreen);
  assert_colors_blend_to(KDColorGreen, KDColorBlue, 0x7F, midTurquoise);

  // Assert that blending two identical colors does not produce strange colors.
  for (uint16_t col = 0; col < 0xFFFF; col++) {
    KDColor color = KDColor::RGB16(col);
    assert_colors_blend_to(color, color, col>>8, color);
  }
}

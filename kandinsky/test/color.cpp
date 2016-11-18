#include <quiz.h>
#include <kandinsky.h>
#include <assert.h>

QUIZ_CASE(kandinsky_color_rgb) {
  assert(sizeof(KDColor) == 2); // We really want KDColor to be packed

  assert(KDColor::RGB24(0xFF0000) == 0xF800);
  assert(KDColor::RGB24(0x00FF00) == 0x07E0);
  assert(KDColor::RGB24(0x0000FF) == 0x1F);
  /* R = 0x12 = 0b 0001 0010. 5 most important bits are 00010.
   * G = 0x34 = 0b 0011 0100. 6 most important bits are 001101.
   * B = 0x56 = 0b 0101 0110. 5 most important bits are 01010.
   * KDColor = 0b 00010 001101 01010
   *         = 0b 0001 0001 1010 1010
   *         = 0x    1    1 A    A */
  assert(KDColor::RGB24(0x123456) == 0x11AA);
}

QUIZ_CASE(kandinsky_color_blend) {
  KDColor midGray = KDColor::RGB24(0x7F7F7F);
  KDColor res = KDColor::blend(KDColorWhite, KDColorBlack, 0xFF);
  assert(res == KDColorWhite);
  assert(KDColor::blend(KDColorWhite, KDColorBlack, 0) == KDColorBlack);
  assert(KDColor::blend(KDColorWhite, KDColorBlack, 0x7F) == midGray);
}

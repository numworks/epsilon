#include <quiz.h>
#include <kandinsky.h>
#include <assert.h>

QUIZ_CASE(kandinsky_color_rgb) {
  assert(KDColorRGB(0xFF, 0, 0) == 0xF800);
  assert(KDColorRGB(0, 0xFF, 0) == 0x07E0);
  assert(KDColorRGB(0, 0, 0xFF) == 0x1F);
  /* R = 0x12 = 0b 0001 0010. 5 most important bits are 00010.
   * G = 0x34 = 0b 0011 0100. 6 most important bits are 001101.
   * B = 0x56 = 0b 0101 0110. 5 most important bits are 01010.
   * KDColor = 0b 00010 001101 01010
   *         = 0b 0001 0001 1010 1010
   *         = 0x    1    1 A    A */
  assert(KDColorRGB(0x12, 0x34, 0x56) == 0x11AA);
}


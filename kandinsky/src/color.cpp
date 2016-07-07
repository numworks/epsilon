#include <kandinsky/color.h>

KDColor KDColor::blend(KDColor other, uint8_t alpha) {
  return other; // FIXME
}

uint8_t KDColor::red() {
  uint8_t r5 = (m_value>>11)&0x1F;
  return r5 << 3;
}

uint8_t KDColor::green() {
  uint8_t g6 = (m_value>>5)&0x3F;
  return g6 << 2;
}

uint8_t KDColor::blue() {
  uint8_t b5 = m_value&0x1F;
  return b5 << 3;
}

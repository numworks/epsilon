#include <poincare/number.h>

Number::Number(int v) : m_value(v) {
}

void Number::layout() {
  m_frame.width = 20;
  m_frame.height = 10;
}

void Number::draw() {
  KDFillRect(m_frame, 0x7F);
}

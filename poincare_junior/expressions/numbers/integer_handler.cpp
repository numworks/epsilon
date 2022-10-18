#include "integer_handler.h"

namespace Poincare {

float IntegerHandler::approximate() {
  float sign = m_negative ? -1.0f : 1.0f;
  if (m_numberOfDigits == 1) {
    return sign * static_cast<float>(m_digitAccessor.m_digit);
  }
  float approximation = 0.0f;
  for (uint8_t i = 0; i < m_numberOfDigits; i++) {
    approximation += m_digitAccessor.m_digits[i];
  }
  return sign * approximation;
}

}

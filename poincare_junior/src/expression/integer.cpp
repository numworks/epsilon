#include "integer.h"

namespace Poincare {

template <typename T>
T IntegerHandler::to() {
  T sign = m_negative ? -1.0 : 1.0;
  if (m_numberOfDigits == 1) {
    return sign * static_cast<T>(m_digitAccessor.m_digit);
  }
  T approximation = 0.0f;
  for (uint8_t i = 0; i < m_numberOfDigits; i++) {
    approximation += m_digitAccessor.m_digits[i];
  }
  return sign * approximation;
}

}

template float Poincare::IntegerHandler::to<float>();
template double Poincare::IntegerHandler::to<double>();

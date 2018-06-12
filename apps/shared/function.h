#ifndef SHARED_FUNCTION_H
#define SHARED_FUNCTION_H

#include "expression_model.h"

namespace Shared {

class Function : public ExpressionModel {
public:
  Function(const char * name = nullptr, KDColor color = KDColorBlack);
  virtual uint32_t checksum();
  const char * name() const;
  KDColor color() const { return m_color; }
  bool isActive();
  void setActive(bool active);
  void setColor(KDColor m_color);
  virtual float evaluateAtAbscissa(float x, Poincare::Context * context) const {
    return templatedApproximateAtAbscissa(x, context);
  }
  virtual double evaluateAtAbscissa(double x, Poincare::Context * context) const {
    return templatedApproximateAtAbscissa(x, context);
  }
  virtual double sumBetweenBounds(double start, double end, Poincare::Context * context) const = 0;
private:
  constexpr static size_t k_dataLengthInBytes = (TextField::maxBufferSize()+2)*sizeof(char)+2;
  static_assert((k_dataLengthInBytes & 0x3) == 0, "The function data size is not a multiple of 4 bytes (cannot compute crc)"); // Assert that dataLengthInBytes is a multiple of 4
  template<typename T> T templatedApproximateAtAbscissa(T x, Poincare::Context * context) const;
  virtual char symbol() const = 0;
  const char * m_name;
  KDColor m_color;
  bool m_active;
};

}

#endif

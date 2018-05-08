#ifndef SHARED_FUNCTION_H
#define SHARED_FUNCTION_H

#include <poincare.h>
#include <kandinsky.h>
#include <escher.h>

namespace Shared {

class Function {
public:
  Function(const char * name = nullptr, KDColor color = KDColorBlack);
  virtual ~Function(); // Delete expression and layout, if needed
  Function& operator=(const Function& other);
  Function& operator=(Function&& other) = delete;
  Function(const Function& other) = delete;
  Function(Function&& other) = delete;
  virtual uint32_t checksum();
  const char * text() const;
  const char * name() const;
  KDColor color() const { return m_color; }
  Poincare::Expression * expression(Poincare::Context * context) const;
  Poincare::ExpressionLayout * layout();
  virtual bool isDefined();
  bool isActive();
  void setActive(bool active);
  virtual bool isEmpty();
  virtual void setContent(const char * c);
  void setColor(KDColor m_color);
  virtual float evaluateAtAbscissa(float x, Poincare::Context * context) const {
    return templatedApproximateAtAbscissa(x, context);
  }
  virtual double evaluateAtAbscissa(double x, Poincare::Context * context) const {
    return templatedApproximateAtAbscissa(x, context);
  }
  virtual double sumBetweenBounds(double start, double end, Poincare::Context * context) const = 0;
  virtual void tidy();
private:
  constexpr static size_t k_dataLengthInBytes = (TextField::maxBufferSize()+2)*sizeof(char)+2;
  static_assert((k_dataLengthInBytes & 0x3) == 0, "The function data size is not a multiple of 4 bytes (cannot compute crc)"); // Assert that dataLengthInBytes is a multiple of 4
  template<typename T> T templatedApproximateAtAbscissa(T x, Poincare::Context * context) const;
  virtual char symbol() const = 0;
  mutable Poincare::Expression * m_expression;
  char m_text[TextField::maxBufferSize()];
  const char * m_name;
  KDColor m_color;
  Poincare::ExpressionLayout * m_layout;
  bool m_active;
};

}

#endif

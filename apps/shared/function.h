#ifndef SHARED_FUNCTION_H
#define SHARED_FUNCTION_H

#include <poincare.h>
#include <kandinsky.h>

namespace Shared {

class Function {
public:
  Function(const char * text = nullptr, KDColor color = KDColorBlack);
  ~Function(); // Delete expression and layout, if needed
  const char * text();
  const char * name();
  KDColor color() const { return m_color; }
  Poincare::Expression * expression();
  Poincare::ExpressionLayout * layout();
  virtual bool isDefined();
  bool isActive();
  void setActive(bool active);
  virtual void setContent(const char * c);
  void setColor(KDColor m_color);
  virtual float evaluateAtAbscissa(float x, Poincare::Context * context) const;
protected:
  constexpr static int k_bodyLength = 255;
  Poincare::Expression * m_expression;
private:
  virtual char symbol() const = 0;
  char m_text[k_bodyLength];
  const char * m_name;
  KDColor m_color;
  Poincare::ExpressionLayout * m_layout;
  bool m_active;
};

}

#endif

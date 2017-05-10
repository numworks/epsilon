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
  const char * text() const;
  const char * name() const;
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
  Poincare::Expression * m_expression;
private:
  virtual char symbol() const = 0;
  char m_text[TextField::maxBufferSize()];
  const char * m_name;
  KDColor m_color;
  Poincare::ExpressionLayout * m_layout;
  bool m_active;
};

}

#endif

#ifndef GRAPH_FUNCTION_H
#define GRAPH_FUNCTION_H

#include <poincare.h>
#include <kandinsky.h>

namespace Graph {

class Function {
public:
  static constexpr const char * Parameter = "(x)";

  Function(const char * text = nullptr, KDColor color = KDColorBlack);
  ~Function(); // Delete expression and layout, if needed
  const char * text();
  const char * name();
  KDColor color() const { return m_color; }
  Poincare::Expression * expression();
  Poincare::ExpressionLayout * layout();
  bool isActive();
  void setActive(bool active);
  bool displayDerivative();
  void setDisplayDerivative(bool display);
  void setContent(const char * c);
  void setColor(KDColor m_color);
  float evaluateAtAbscissa(float x, Poincare::Context * context, Poincare::Expression::AngleUnit angleUnit) const;
  float approximateDerivative(float x, Poincare::Context * context, Poincare::Expression::AngleUnit angleUnit) const;
private:
  constexpr static int k_bodyLength = 255;
  char m_text[k_bodyLength];
  const char * m_name;
  KDColor m_color;
  Poincare::Expression * m_expression;
  Poincare::ExpressionLayout * m_layout;
  bool m_active;
  bool m_displayDerivative;
};

}

#endif

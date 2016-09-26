#ifndef GRAPH_FUNCTION_H
#define GRAPH_FUNCTION_H

#include <poincare.h>
#include <kandinsky.h>

namespace Graph {

class Function {
public:
  Function();
  Function(const char * text, KDColor m_color);
  ~Function(); // Delete expression and layout, if needed
  const char * text();
  const char * name();
  KDColor color() const { return m_color; }
  Expression * expression();
  ExpressionLayout * layout();
  bool isActive();
  void setActive(bool active);
  void setContent(const char * c);
  void setColor(KDColor m_color);
  void setName(const char * c);
private:
  constexpr static int k_bodyLength = 255;
  char m_text[k_bodyLength];
  const char * m_name;
  KDColor m_color;
  Expression * m_expression;
  ExpressionLayout * m_layout;
  bool m_active;
};

}

#endif

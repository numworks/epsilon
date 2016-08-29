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
  KDColor color() const { return m_color; }
  Expression * expression();
  ExpressionLayout * layout();
private:
  const char * m_text;
  KDColor m_color;
  Expression * m_expression;
  ExpressionLayout * m_layout;
};

}

#endif

#ifndef GRAPH_FUNCTION_H
#define GRAPH_FUNCTION_H

#include <poincare.h>

namespace Graph {

class Function {
public:
  Function();
  Function(const char * text);
  ~Function(); // Delete expression and layout, if needed
  const char * text();
  Expression * expression();
  ExpressionLayout * layout();
private:
  const char * m_text;
  Expression * m_expression;
  ExpressionLayout * m_layout;
};

}

#endif

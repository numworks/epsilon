#ifndef CALCUL_CALCUL_H
#define CALCUL_CALCUL_H

#include <poincare.h>

namespace Calcul {

class Calcul {
public:
  Calcul();
  ~Calcul(); // Delete expression and layout, if needed
  const char * text();
  Expression * expression();
  ExpressionLayout * layout();
  Float * evaluation();
  void setContent(const char * c, Context * context);
private:
  constexpr static int k_bodyLength = 255;
  char m_text[k_bodyLength];
  Expression * m_expression;
  ExpressionLayout * m_layout;
  Float m_evaluation;
};

}

#endif

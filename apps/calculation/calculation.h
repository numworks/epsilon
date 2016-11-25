#ifndef CALCULATION_CALCULATION_H
#define CALCULATION_CALCULATION_H

#include <poincare.h>

namespace Calculation {

class Calculation {
public:
  Calculation();
    ~Calculation(); // Delete expression and layout, if needed
  Calculation & operator= (const Calculation & other);
  const char * text();
  Expression * expression();
  ExpressionLayout * layout();
  Expression * evaluation();
  ExpressionLayout * evaluationLayout();
  void setContent(const char * c, Context * context);
  bool isEmpty();
private:
  constexpr static int k_bodyLength = 255;
  char m_text[k_bodyLength];
  Expression * m_expression;
  ExpressionLayout * m_layout;
  Expression * m_evaluation;
  ExpressionLayout * m_evaluationLayout;
};

}

#endif

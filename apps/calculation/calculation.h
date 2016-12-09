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
  Expression * input();
  ExpressionLayout * inputLayout();
  Expression * output();
  ExpressionLayout * outputLayout();
  void setContent(const char * c, Context * context);
  bool isEmpty();
  constexpr static int k_maximalExpressionTextLength = 255;
private:
  char m_text[k_maximalExpressionTextLength];
  Expression * m_input;
  ExpressionLayout * m_inputLayout;
  Expression * m_output;
  ExpressionLayout * m_outputLayout;
};

}

#endif

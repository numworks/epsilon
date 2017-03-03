#ifndef CALCULATION_CALCULATION_H
#define CALCULATION_CALCULATION_H

#include <poincare.h>

namespace Calculation {

class Calculation {
public:
  Calculation();
  ~Calculation(); // Delete expression and layout, if needed
  /* The copy assignment operator is deleted as its default implementation does
   * not create new expressions. The new object thus become obsolete as soon
   * as the copy is deleted (because of our implementation of deletion). To
   * avoid any use of obsolete object, we prevent to copy and assign. */
  Calculation & operator= (const Calculation & other) = delete;
  /* c.reset() is the equivalent of c = Calculation() without copy assingment. */
  void reset();
  const char * inputText();
  const char * outputText();
  Poincare::Expression * input();
  Poincare::ExpressionLayout * inputLayout();
  Poincare::Expression * output();
  Poincare::ExpressionLayout * outputLayout();
  void setContent(const char * c, Poincare::Context * context);
  bool isEmpty();
  constexpr static int k_maximalExpressionTextLength = 255;
private:
  char m_inputText[k_maximalExpressionTextLength];
  char m_outputText[k_maximalExpressionTextLength];
  Poincare::Expression * m_input;
  Poincare::ExpressionLayout * m_inputLayout;
  Poincare::Expression * m_output;
  Poincare::ExpressionLayout * m_outputLayout;
};

}

#endif

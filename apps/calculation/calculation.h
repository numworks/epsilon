#ifndef CALCULATION_CALCULATION_H
#define CALCULATION_CALCULATION_H

#include <poincare.h>
#include "../preferences.h"

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
  const char * text();
  Expression * input();
  ExpressionLayout * inputLayout();
  Expression * output();
  ExpressionLayout * outputLayout();
  void setContent(const char * c, Context * context, Preferences * preferences);
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

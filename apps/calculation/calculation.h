#ifndef CALCULATION_CALCULATION_H
#define CALCULATION_CALCULATION_H

#include <escher.h>
#include <poincare.h>

namespace Calculation {

class CalculationStore;

class Calculation {
public:
  Calculation();
  ~Calculation(); // Delete expression and layout, if needed
  Calculation& operator=(const Calculation& other);
  Calculation(const Calculation& other) = delete;
  Calculation(Calculation&& other) = delete;
  Calculation& operator=(Calculation&& other) = delete;
  /* c.reset() is the equivalent of c = Calculation() without copy assingment. */
  void reset();
  void setContent(const char * c, Poincare::Context * context, Poincare::Expression * ansExpression);
  const char * inputText();
  const char * exactOutputText();
  const char * approximateOutputText();
  Poincare::Expression * input();
  Poincare::ExpressionLayout * inputLayout();
  Poincare::Expression * approximateOutput(Poincare::Context * context);
  Poincare::Expression * exactOutput(Poincare::Context * context);
  Poincare::ExpressionLayout * exactOutputLayout(Poincare::Context * context);
  Poincare::ExpressionLayout * approximateOutputLayout(Poincare::Context * context);
  bool isEmpty();
  void tidy();
  bool shouldDisplayApproximateOutput(Poincare::Context * context);
  bool exactAndApproximateDisplayedOutputsAreEqual(Poincare::Context * context);
  constexpr static int k_printedExpressionSize = 2*::TextField::maxBufferSize();
private:
  /* Buffers holding text expressions have to be longer than the text written
   * by user (of maximum length TextField::maxBufferSize()) because when we
   * print an expression we add omitted signs (multiplications, parenthesis...) */
  char m_inputText[k_printedExpressionSize];
  char m_exactOutputText[k_printedExpressionSize];
  char m_approximateOutputText[k_printedExpressionSize];
  Poincare::Expression * m_input;
  Poincare::ExpressionLayout * m_inputLayout;
  Poincare::Expression * m_exactOutput;
  Poincare::ExpressionLayout * m_exactOutputLayout;
  Poincare::Expression * m_approximateOutput;
  Poincare::ExpressionLayout * m_approximateOutputLayout;
};

}

#endif

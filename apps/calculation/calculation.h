#ifndef CALCULATION_CALCULATION_H
#define CALCULATION_CALCULATION_H

#include <escher.h>
#include <poincare/context.h>
#include <poincare/expression.h>

namespace Calculation {

class CalculationStore;

class Calculation {
public:
  enum class EqualSign : uint8_t {
    Unknown,
    Approximation,
    Equal
  };
  Calculation();
  bool operator==(const Calculation& c);
  /* c.reset() is the equivalent of c = Calculation() without copy assingment. */
  void reset();
  void setContent(const char * c, Poincare::Context * context, Poincare::Expression ansExpression);
  KDCoordinate height(Poincare::Context * context);
  const char * inputText();
  const char * exactOutputText();
  const char * approximateOutputText();
  Poincare::Expression input();
  Poincare::Layout createInputLayout();
  Poincare::Expression approximateOutput(Poincare::Context * context);
  Poincare::Expression exactOutput(Poincare::Context * context);
  Poincare::Layout createExactOutputLayout(Poincare::Context * context);
  Poincare::Layout createApproximateOutputLayout(Poincare::Context * context);
  bool isEmpty();
  void tidy();
  bool shouldOnlyDisplayApproximateOutput(Poincare::Context * context);
  EqualSign exactAndApproximateDisplayedOutputsAreEqual(Poincare::Context * context);
  constexpr static int k_printedExpressionSize = 2*::TextField::maxBufferSize();
private:
  static constexpr KDCoordinate k_heightComputationFailureHeight = 50;
  /* Buffers holding text expressions have to be longer than the text written
   * by user (of maximum length TextField::maxBufferSize()) because when we
   * print an expression we add omitted signs (multiplications, parenthesis...) */
  char m_inputText[k_printedExpressionSize];
  char m_exactOutputText[k_printedExpressionSize];
  char m_approximateOutputText[k_printedExpressionSize];
  KDCoordinate m_height;
  EqualSign m_equalSign;
};

}

#endif

#ifndef CALCULATION_CALCULATION_H
#define CALCULATION_CALCULATION_H

#include <apps/constant.h>
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

  enum class DisplayOutput : uint8_t {
    ExactOnly,
    ApproximateOnly,
    ExactAndApproximate
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
  Poincare::Expression exactOutput();
  Poincare::Layout createExactOutputLayout();
  Poincare::Layout createApproximateOutputLayout(Poincare::Context * context);
  bool isEmpty();
  void tidy();
  DisplayOutput displayOutput(Poincare::Context * context);
  bool shouldOnlyDisplayExactOutput();
  EqualSign exactAndApproximateDisplayedOutputsAreEqual(Poincare::Context * context);
private:
  static constexpr KDCoordinate k_heightComputationFailureHeight = 50;
  /* Buffers holding text expressions have to be longer than the text written
   * by user (of maximum length TextField::maxBufferSize()) because when we
   * print an expression we add omitted signs (multiplications, parenthesis...) */
  char m_inputText[Constant::MaxSerializedExpressionSize];
  char m_exactOutputText[Constant::MaxSerializedExpressionSize];
  char m_approximateOutputText[Constant::MaxSerializedExpressionSize];
  KDCoordinate m_height;
  EqualSign m_equalSign;
};

}

#endif

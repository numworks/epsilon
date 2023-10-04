#ifndef CALCULATION_CALCULATION_H
#define CALCULATION_CALCULATION_H

#include <apps/calculation/additional_results/additional_results_type.h>
#include <apps/constant.h>
#include <apps/shared/poincare_helpers.h>
#include <poincare/context.h>
#include <poincare/expression.h>

namespace Calculation {

class CalculationStore;

// clang-format off
/* A calculation is:
 *  |     uint8_t   |  uint8_t  |KDCoordinate|  KDCoordinate  |   ...     |      ...        |          ...           |           ...          |
 *  |m_displayOutput|m_equalSign|  m_height  |m_expandedHeight|m_inputText|m_exactOutputText|m_approximateOutputText1|m_approximateOutputText2|
 *                                                                                                 with maximal            with displayed
 *                                                                                              significant digits       significant digits
 *
 * */
// clang-format on

class Calculation {
  friend CalculationStore;
  using HeightComputer = void (*)(Calculation*, Poincare::Context*);

 public:
  constexpr static int k_numberOfExpressions = 4;
  enum class EqualSign : uint8_t { Unknown, Approximation, Equal };

  enum class DisplayOutput : uint8_t {
    Unknown,
    ExactOnly,
    ApproximateOnly,
    ExactAndApproximate,
    ExactAndApproximateToggle
  };

  /* It is not really the minimal size, but it clears enough space for most
   * calculations instead of clearing less space, then fail to serialize, clear
   * more space, fail to serialize, clear more space, etc., until reaching
   * sufficient free space. */
  constexpr static int k_minimalSize =
      sizeof(uint8_t) + 2 * sizeof(KDCoordinate) + sizeof(uint8_t) +
      k_numberOfExpressions * Constant::MaxSerializedExpressionSize;

  Calculation()
      : m_displayOutput(DisplayOutput::Unknown),
        m_equalSign(EqualSign::Unknown),
        m_height(-1),
        m_expandedHeight(-1) {
    assert(sizeof(m_inputText) == 0);
  }
  bool operator==(const Calculation& c);
  Calculation* next() const;

  // Texts
  enum class NumberOfSignificantDigits { Maximal, UserDefined };
  const char* inputText() const { return m_inputText; }
  const char* exactOutputText() const {
    return m_inputText + strlen(m_inputText) + 1;
  }
  /* See comment in approximateOutput implementation explaining the need of two
   * approximateOutputTexts. */
  const char* approximateOutputText(
      NumberOfSignificantDigits numberOfSignificantDigits) const;

  // Expressions
  Poincare::Expression input();
  Poincare::Expression exactOutput();
  Poincare::Expression approximateOutput(
      NumberOfSignificantDigits numberOfSignificantDigits);

  // Layouts
  Poincare::Layout createInputLayout();
  Poincare::Layout createExactOutputLayout(bool* couldNotCreateExactLayout);
  Poincare::Layout createApproximateOutputLayout(
      bool* couldNotCreateApproximateLayout);

  // Heights
  KDCoordinate height(bool expanded);
  void setHeights(KDCoordinate height, KDCoordinate expandedHeight);
  void computeHeights(HeightComputer heightComputer,
                      Poincare::Context* context);

  // Displayed output
  DisplayOutput displayOutput(Poincare::Context* context);
  void createOutputLayouts(Poincare::Layout* exactOutput,
                           Poincare::Layout* approximateOutput,
                           Poincare::Context* context,
                           bool canChangeDisplayOutput,
                           KDCoordinate maxVisibleWidth, KDFont::Size font);
  EqualSign equalSign(Poincare::Context* context);

  void fillExpressionsForAdditionalResults(
      Poincare::Expression* input, Poincare::Expression* exactOutput,
      Poincare::Expression* approximateOutput);
  AdditionalResultsType additionalResultsType();

 private:
  constexpr static KDCoordinate k_heightComputationFailureHeight = 50;
  static bool DisplaysExact(DisplayOutput d) {
    return d != DisplayOutput::ApproximateOnly;
  }
  void forceDisplayOutput(DisplayOutput d) { m_displayOutput = d; }

  /* Buffers holding text expressions have to be longer than the text written
   * by user (of maximum length TextField::MaxBufferSize()) because when we
   * print an expression we add omitted signs (multiplications, parenthesis...)
   */
  DisplayOutput m_displayOutput;
  EqualSign m_equalSign;
  KDCoordinate m_height;
  KDCoordinate m_expandedHeight;
  char m_inputText[0];  // MUST be the last member variable
};

}  // namespace Calculation

#endif

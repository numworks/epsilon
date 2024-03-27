#ifndef CALCULATION_CALCULATION_H
#define CALCULATION_CALCULATION_H

#include <apps/calculation/additional_results/additional_results_type.h>
#include <apps/shared/poincare_helpers.h>
#include <poincare/context.h>
#include <poincare/expression.h>

#if __EMSCRIPTEN__
#include <emscripten.h>
#endif

namespace Calculation {

class CalculationStore;

// clang-format off
/* A calculation is:
 *  |     uint8_t   |  uint8_t  |KDCoordinate|  KDCoordinate  |   uint16_t    |    ...    |          ...    |           ...          |
 *  |m_displayOutput|m_equalSign|  m_height  |m_expandedHeight|m_inputTreeSize|m_inputTree|m_exactOutputText|m_approximateOutputText1|m_approximateOutputText2|
 *                                                                                                                with maximal            with displayed
 *                                                                                                             significant digits       significant digits
 *
 * */
// clang-format on

class Calculation {
  friend CalculationStore;

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

  Calculation(
      Poincare::Preferences::CalculationPreferences calculationPreferences)
      : m_displayOutput(DisplayOutput::Unknown),
        m_equalSign(EqualSign::Unknown),
        m_calculationPreferences(calculationPreferences),
        m_additionalResultsType(),
        m_height(-1),
        m_expandedHeight(-1) {
    assert(sizeof(m_inputText) == 0);
  }
  bool operator==(const Calculation& c);
  Calculation* next() const;

  // Reduction properties
  Poincare::Preferences::CalculationPreferences calculationPreferences() const {
    return m_calculationPreferences;
  }
  Poincare::Preferences::AngleUnit angleUnit() const {
    return m_calculationPreferences.angleUnit;
  }
  Poincare::Preferences::PrintFloatMode displayMode() const {
    return m_calculationPreferences.displayMode;
  }
  Poincare::Preferences::ComplexFormat complexFormat() const {
    return m_calculationPreferences.complexFormat;
  }
  uint8_t numberOfSignificantDigits() const {
    return m_calculationPreferences.numberOfSignificantDigits;
  }

  // Texts
  enum class NumberOfSignificantDigits { Maximal, UserDefined };
  const PoincareJ::Tree* inputTree() const {
    return (const PoincareJ::Tree*)(m_inputText + 2);
  }
  size_t inputTreeSize() const { return *(const uint16_t*)(m_inputText); }
  const char* exactOutputText() const {
    return 2 + m_inputText + inputTreeSize();
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
  /* Memoize the CalculationPreferences used for computing the outputs in case
   * they change later in the shared preferences and we need to compute
   * additional results. */
  Poincare::Preferences::CalculationPreferences m_calculationPreferences;
  AdditionalResultsType m_additionalResultsType;
#if __EMSCRIPTEN__
  // See comment about emscripten alignment in Function::RecordDataBuffer
  static_assert(
      sizeof(emscripten_align1_short) == sizeof(KDCoordinate),
      "emscripten_align1_short should have the same size as KDCoordinate");
  emscripten_align1_short m_height;
  emscripten_align1_short m_expandedHeight;
#else
  KDCoordinate m_height;
  KDCoordinate m_expandedHeight;
#endif
  char m_inputText[0];  // MUST be the last member variable
};

}  // namespace Calculation

#endif

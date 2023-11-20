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
 *  |     uint8_t   |  uint8_t  |KDCoordinate|  KDCoordinate  |   ...     |      ...        |          ...           |           ...          |
 *  |m_displayOutput|m_equalSign|  m_height  |m_expandedHeight|m_inputText|m_exactOutputText|m_approximateOutputText1|m_approximateOutputText2|
 *                                                                                                 with maximal            with displayed
 *                                                                                              significant digits       significant digits
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

  Calculation(Poincare::Preferences::ComplexFormat complexFormat,
              Poincare::Preferences::AngleUnit angleUnit)
      : m_displayOutput(DisplayOutput::Unknown),
        m_equalSign(EqualSign::Unknown),
        m_complexFormat(complexFormat),
        m_angleUnit(angleUnit),
        m_height(-1),
        m_expandedHeight(-1) {
    assert(sizeof(m_inputText) == 0);
  }
  bool operator==(const Calculation& c);
  Calculation* next() const;

  // Reduction properties
  Poincare::Preferences::ComplexFormat complexFormat() {
    return m_complexFormat;
  }
  Poincare::Preferences::AngleUnit angleUnit() { return m_angleUnit; }

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
  /* Memoize the parameters used for computing the outputs in case they change
   * later in the shared preferences and we need to compute additional
   * results. */
  Poincare::Preferences::ComplexFormat m_complexFormat;
  Poincare::Preferences::AngleUnit m_angleUnit;
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

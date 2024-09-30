#ifndef CALCULATION_CALCULATION_H
#define CALCULATION_CALCULATION_H

#include <apps/calculation/additional_results/additional_results_type.h>
#include <apps/shared/poincare_helpers.h>
#include <poincare/expression.h>
#include <poincare/old/context.h>

#if __EMSCRIPTEN__
#include <emscripten.h>
#endif

namespace Calculation {

class CalculationStore;

/* A calculation is:
 * struct {
 *   uint8_t m_displayOutput;
 *   uint8_t  m_equalSign;
 *   KDCoordinate m_height;
 *   KDCoordinate m_expandedHeight;
 *
 *   uint16_t m_inputTreeSize;
 *   uint16_t m_exactOutputTreeSize;
 *   uint16_t m_approximatedOutputTreeSize;
 *
 *   Poincare::Internal::Tree m_inputTree;
 *   Poincare::Internal::Tree m_exactOutputTree;
 *   Poincare::Internal::Tree m_approximatedOutputTree;
 * };
 *
 * Since the three last members have variable size, they are gathered in m_trees
 */

class Calculation {
  friend CalculationStore;

 public:
  constexpr static int k_numberOfExpressions = 3;
  enum class EqualSign : uint8_t { Unknown, Approximation, Equal };

  enum class DisplayOutput : uint8_t {
    Unknown,
    ExactOnly,
    ApproximateOnly,
    /* ApproximateIsIdenticalToExact has the same display behavior as
     * ApproximateOnly but exact output may be used by Ans or additional
     * results */
    ApproximateIsIdenticalToExact,
    ExactAndApproximate,
    ExactAndApproximateToggle
  };

  static bool CanDisplayExact(DisplayOutput d) {
    assert(d != DisplayOutput::Unknown);
    return d != DisplayOutput::ApproximateOnly &&
           d != DisplayOutput::ApproximateIsIdenticalToExact;
  }

  static bool CanDisplayApproximate(DisplayOutput d) {
    assert(d != DisplayOutput::Unknown);
    return d != DisplayOutput::ExactOnly;
  }

  Calculation(
      Poincare::Preferences::CalculationPreferences calculationPreferences)
      : m_displayOutput(DisplayOutput::Unknown),
        m_equalSign(EqualSign::Unknown),
        m_calculationPreferences(calculationPreferences),
        m_additionalResultsType(),
        m_height(-1),
        m_expandedHeight(-1) {
    static_assert(sizeof(m_trees) == 0);
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
  void setComplexFormat(Poincare::Preferences::ComplexFormat complexFormat) {
    m_calculationPreferences.complexFormat = complexFormat;
  }
  uint8_t numberOfSignificantDigits() const {
    return m_calculationPreferences.numberOfSignificantDigits;
  }

  // Expressions
  Poincare::UserExpression input();
  Poincare::UserExpression exactOutput();
  Poincare::UserExpression approximateOutput();

  // Layouts
  Poincare::Layout createInputLayout();
  Poincare::Layout createExactOutputLayout(bool* couldNotCreateExactLayout);
  Poincare::Layout createApproximateOutputLayout(
      bool* couldNotCreateApproximateLayout, bool forEditing = false);

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
      Poincare::UserExpression* input, Poincare::UserExpression* exactOutput,
      Poincare::UserExpression* approximateOutput);
  AdditionalResultsType additionalResultsType();

 private:
  constexpr static KDCoordinate k_heightComputationFailureHeight = 50;
  void forceDisplayOutput(DisplayOutput d) { m_displayOutput = d; }

  const Poincare::Internal::Tree* inputTree() const {
    return reinterpret_cast<const Poincare::Internal::Tree*>(m_trees);
  }
  const Poincare::Internal::Tree* exactOutputTree() const {
    return reinterpret_cast<const Poincare::Internal::Tree*>(m_trees +
                                                             m_inputTreeSize);
  }
  const Poincare::Internal::Tree* approximatedOutputTree() const {
    return reinterpret_cast<const Poincare::Internal::Tree*>(
        m_trees + m_inputTreeSize + m_exactOutputTreeSize);
  }
  bool exactAndApproximatedAreEqual() const {
    return m_exactOutputTreeSize == m_approximatedOutputTreeSize &&
           memcmp(exactOutputTree(), approximatedOutputTree(),
                  m_exactOutputTreeSize) == 0;
  }

  size_t cumulatedTreeSizes() const {
    return m_inputTreeSize + m_exactOutputTreeSize +
           m_approximatedOutputTreeSize;
  }

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
  emscripten_align1_short m_inputTreeSize;
  emscripten_align1_short m_exactOutputTreeSize;
  emscripten_align1_short m_approximatedOutputTreeSize;
#else
  KDCoordinate m_height;
  KDCoordinate m_expandedHeight;
  uint16_t m_inputTreeSize;
  uint16_t m_exactOutputTreeSize;
  uint16_t m_approximatedOutputTreeSize;  // used only by ==
#endif
  char m_trees[0];  // MUST be the last member variable
};

}  // namespace Calculation

#endif

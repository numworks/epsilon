#ifndef CALCULATION_CALCULATION_H
#define CALCULATION_CALCULATION_H

#include <apps/calculation/additional_results/additional_results_type.h>
#include <apps/shared/poincare_helpers.h>
#include <poincare/expression.h>
#include <poincare/k_tree.h>
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
  enum class EqualSign : uint8_t { Unknown, Undefined, Approximation, Equal };

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
        m_expandedHeight(-1),
        m_inputTreeSize(0),
        m_exactOutputTreeSize(0),
        m_approximatedOutputTreeSize(0) {
    static_assert(sizeof(m_trees) == 0);
  }
  bool operator==(const Calculation& c);
  Calculation* next() const;

  // Reduction properties
  Poincare::Preferences::CalculationPreferences calculationPreferences() const {
    return m_calculationPreferences;
  }

  void setComplexFormat(Poincare::Preferences::ComplexFormat complexFormat) {
    m_calculationPreferences.complexFormat = complexFormat;
  }

  // Expressions
  Poincare::UserExpression input();
  Poincare::UserExpression exactOutput();
  Poincare::UserExpression approximateOutput();

  // Layouts
  Poincare::Layout createInputLayout(Poincare::Context* context);
  Poincare::Layout createExactOutputLayout(Poincare::Context* context,
                                           bool* couldNotCreateExactLayout);
  Poincare::Layout createApproximateOutputLayout(
      Poincare::Context* context, bool* couldNotCreateApproximateLayout,
      bool forEditing = false);

  // Heights
  KDCoordinate height(bool expanded);
  void setHeights(KDCoordinate height, KDCoordinate expandedHeight);

  // Displayed output
  DisplayOutput displayOutput() const {
    assert(m_displayOutput != DisplayOutput::Unknown);
    return m_displayOutput;
  }

  EqualSign equalSign() const;

  struct OutputLayouts {
    Poincare::Layout exact;
    Poincare::Layout approximate;
  };

  OutputLayouts layoutCalculation(KDFont::Size font,
                                  KDCoordinate maxVisibleWidth,
                                  Poincare::Context* context,
                                  bool canChangeDisplayOutput);

  void fillExpressionsForAdditionalResults(
      Poincare::UserExpression* input, Poincare::UserExpression* exactOutput,
      Poincare::UserExpression* approximateOutput, Poincare::Context* context);
  AdditionalResultsType additionalResultsType(Poincare::Context* context);

 private:
  constexpr static KDCoordinate k_heightComputationFailureHeight = 50;

  static DisplayOutput ComputeDisplayOutput(
      Poincare::UserExpression input, Poincare::UserExpression exactOutput,
      Poincare::UserExpression approximateOutput, Poincare::Context* context);

  static EqualSign ComputeEqualSignFromOutputs(
      const OutputLayouts& outputLayouts,
      Poincare::Internal::ComplexFormat complexFormat,
      Poincare::Internal::AngleUnit angleUnit, Poincare::Context* context);

  void forceDisplayOutput(DisplayOutput d) { m_displayOutput = d; }

  const Poincare::Internal::Tree* inputTree() const {
    return m_inputTreeSize == 0 ? KUndef
                                : Poincare::Internal::Tree::FromBlocks(m_trees);
  }
  const Poincare::Internal::Tree* exactOutputTree() const {
    return m_exactOutputTreeSize == 0 ? KUndef
                                      : Poincare::Internal::Tree::FromBlocks(
                                            m_trees + m_inputTreeSize);
  }
  const Poincare::Internal::Tree* approximatedOutputTree() const {
    return m_approximatedOutputTreeSize == 0
               ? KUndef
               : Poincare::Internal::Tree::FromBlocks(
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

  void computeDisplayOutput(Poincare::Context* context);

  OutputLayouts createOutputLayouts(Poincare::Context* context,
                                    bool canChangeDisplayOutput,
                                    KDCoordinate maxVisibleWidth,
                                    KDFont::Size font);

  /* Compute the sign to be displayed for this expression by comparing the exact
   * output layout and the approximate output layout. */
  void computeEqualSign(const OutputLayouts& outputLayouts,
                        Poincare::Context* context);

  /* Buffers holding text expressions have to be longer than the text written
   * by user (of maximum length TextField::MaxBufferSize()) because when we
   * print an expression we add omitted signs (multiplications, parenthesis...)
   */
  DisplayOutput m_displayOutput;
  EqualSign m_equalSign;
  /* Memoize the CalculationPreferences used for computing the outputs in
   * case they change later in the shared preferences and we need to compute
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
  Poincare::Internal::Block m_trees[0];  // MUST be the last member variable
};

}  // namespace Calculation

#endif

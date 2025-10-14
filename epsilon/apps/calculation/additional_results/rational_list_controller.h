#pragma once

#include "chainable_expressions_list_controller.h"

namespace Calculation {

class RationalListController : public ChainableExpressionsListController {
 public:
  RationalListController(EditExpressionController* editExpressionController)
      : ChainableExpressionsListController(editExpressionController, true) {}

  void computeAdditionalResults(
      const Poincare::UserExpression input,
      const Poincare::UserExpression exactOutput,
      const Poincare::UserExpression approximateOutput) override;

 private:
  I18n::Message messageAtIndex(int index) override { return m_message[index]; }
  Poincare::Layout layoutAtIndex(Escher::HighlightCell* cell,
                                 int index) override;
  bool hasRationalApproximation() const {
    /* Rational approximation is optional, k_maxNumberOfOutputRows is attained
     * if and only if it is displayed. */
    return numberOfRows() == k_maxNumberOfOutputRows;
  }
  // Return -1 if rational approximation is not displayed
  int indexOfRationalApproximationCell() const {
    return hasRationalApproximation() ? 0 : -1;
  }
  int indexOfEuclidianDivisionCell() const {
    /* Rational approximation is displayed first, so euclidean division shifts
     * one row when rational approximation is present. */
    return hasRationalApproximation() ? 2 : 1;
  }
  constexpr static int k_maxNumberOfOutputRows = 3;
  I18n::Message m_message[k_maxNumberOfOutputRows];
};

}  // namespace Calculation

#ifndef CALCULATION_UNIT_LIST_CONTROLLER_H
#define CALCULATION_UNIT_LIST_CONTROLLER_H

#include <apps/i18n.h>
#include <escher/buffer_text_view.h>
#include <escher/menu_cell.h>
#include <escher/message_text_view.h>
#include <poincare/expression.h>

#include "expressions_list_controller.h"
#include "unit_comparison_helper.h"

namespace Calculation {

class UnitListController : public ExpressionsListController {
 public:
  UnitListController(EditExpressionController* editExpressionController);

  void viewDidDisappear() override;

  int typeAtRow(int row) const override {
    return row < m_numberOfExpressionCells ? k_expressionCellType
                                           : k_bufferCellType;
  }
  int reusableCellCount(int type) const override;
  Escher::HighlightCell* reusableCell(int index, int type) override;
  KDCoordinate nonMemoizedRowHeight(int row) override;
  void fillCellForRow(Escher::HighlightCell* cell, int row) override;

  int numberOfRows() const override;

  void computeAdditionalResults(
      const Poincare::UserExpression input,
      const Poincare::UserExpression exactOutput,
      const Poincare::UserExpression approximateOutput) override;

 private:
  constexpr static int k_bufferCellType = 1;
  constexpr static int k_maxNumberOfExpressionCells = 5;
  static_assert(k_maxNumberOfExpressionCells ==
                ExpressionsListController::k_maxNumberOfRows);
  constexpr static int k_maxNumberOfBufferCells = 2;

  using BufferCell =
      Escher::MenuCell<Escher::OneLineBufferTextView<KDFont::Size::Large>,
                       Escher::MessageTextView>;

  I18n::Message messageAtIndex(int index) override;
  void fillBufferCellAtIndex(BufferCell* bufferCell, int index);
  Poincare::Layout layoutAtIndex(Escher::HighlightCell* cell,
                                 int index) override;

  int m_numberOfExpressionCells;
  int m_numberOfBufferCells;
  BufferCell m_bufferCells[k_maxNumberOfBufferCells];
  const UnitComparison::ReferenceValue*
      m_referenceValues[k_maxNumberOfBufferCells];
  double m_SIValue;
  int m_tableIndexForComparison;
};

}  // namespace Calculation

#endif

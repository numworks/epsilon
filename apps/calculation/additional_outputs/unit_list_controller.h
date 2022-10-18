#ifndef CALCULATION_ADDITIONAL_OUTPUTS_UNIT_LIST_CONTROLLER_H
#define CALCULATION_ADDITIONAL_OUTPUTS_UNIT_LIST_CONTROLLER_H

#include <apps/i18n.h>
#include <poincare/expression.h>
#include <escher/buffer_table_cell_with_message.h>
#include "list_controller.h"
#include "unit_comparison_helper.h"

namespace Calculation {

class UnitListController : public ListController {
public:
  UnitListController(EditExpressionController * editExpressionController);

  void viewDidDisappear() override;
  void didEnterResponderChain(Responder * previousFirstResponder) override;

  int typeAtIndex(int index) const override { return index < m_numberOfExpressionCells ? k_expressionCellType : k_bufferCellType; }
  int reusableCellCount(int type) override;
  Escher::HighlightCell * reusableCell(int index, int type) override;
  KDCoordinate nonMemoizedRowHeight(int j) override;
  void willDisplayCellForIndex(Escher::HighlightCell * cell, int index) override;

  int numberOfRows() const override;

  void setExpression(Poincare::Expression e) override;

private:
  constexpr static int k_expressionCellType = 0;
  constexpr static int k_bufferCellType = 1;
  constexpr static int k_maxNumberOfExpressionCells = 5;
  constexpr static int k_maxNumberOfBufferCells = 2;

  I18n::Message messageAtIndex(int index);
  Poincare::Layout layoutAtIndex(int index);
  void fillBufferCellAtIndex(Escher::BufferTableCellWithMessage * bufferCell, int index);
  int textAtIndex(char * buffer, size_t bufferSize, int index) override;

  Poincare::Expression m_expression;
  int m_numberOfExpressionCells;
  int m_numberOfBufferCells;
  Escher::ExpressionTableCellWithMessage m_expressionCells[k_maxNumberOfExpressionCells];
  Escher::BufferTableCellWithMessage m_bufferCells[k_maxNumberOfBufferCells];
  Poincare::Layout m_layouts[k_maxNumberOfExpressionCells];
  const UnitComparison::ReferenceValue * m_referenceValues[k_maxNumberOfBufferCells];
  double m_SIValue;
  int m_tableIndexForComparison;
};

}

#endif

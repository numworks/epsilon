#ifndef CALCULATION_ADDITIONAL_OUTPUTS_BUFFER_AND_EXPRESSIONS_LIST_CONTROLLER_H
#define CALCULATION_ADDITIONAL_OUTPUTS_BUFFER_AND_EXPRESSIONS_LIST_CONTROLLER_H

#include <poincare/expression.h>
#include <escher/buffer_table_cell_with_message.h>
#include <apps/i18n.h>
#include "list_controller.h"

namespace Calculation {

class BuffersAndExpressionsListController : public ListController {
public:
  BuffersAndExpressionsListController(EditExpressionController * editExpressionController);

  // Responder
  void viewDidDisappear() override;
  void didEnterResponderChain(Responder * previousFirstResponder) override;

  // MemoizedListViewDataSource
  int typeAtIndex(int index) override { return index < m_numberOfExpressionCells ? k_expressionCellType : k_bufferCellType; }
  int reusableCellCount(int type) override;
  Escher::HighlightCell * reusableCell(int index, int type) override;
  KDCoordinate nonMemoizedRowHeight(int j) override;
  void willDisplayCellForIndex(Escher::HighlightCell * cell, int index) override;
  int numberOfRows() const override;

  void setExpression(Poincare::Expression e) override;

protected:
  constexpr static int k_expressionCellType = 0;
  constexpr static int k_bufferCellType = 1;
  constexpr static int k_maxNumberOfExpressionCells = 3;
  constexpr static int k_maxNumberOfBufferCells = 2;
  constexpr static int k_maxNumberOfRows = k_maxNumberOfExpressionCells + k_maxNumberOfBufferCells;
  int textAtIndex(char * buffer, size_t bufferSize, int index) override;
  Poincare::Expression m_expression;
  // Memoization of layouts
  mutable Poincare::Layout m_layouts[k_maxNumberOfRows];
  mutable int m_numberOfExpressionCells;

private:
  Poincare::Layout layoutAtIndex(int index);
  virtual I18n::Message messageAtIndex(int index) = 0;
  virtual void fillBufferCellAtIndex(Escher::BufferTableCellWithMessage * bufferCell, int index) = 0;
  Escher::ExpressionTableCellWithMessage m_expressionCells[k_maxNumberOfRows];
  Escher::BufferTableCellWithMessage m_bufferCells[k_maxNumberOfBufferCells];


};


}

#endif

#ifndef CALCULATION_ADDITIONAL_OUTPUTS_ILLUSTRATED_LIST_CONTROLLER_H
#define CALCULATION_ADDITIONAL_OUTPUTS_ILLUSTRATED_LIST_CONTROLLER_H

#include "scrollable_three_expressions_cell.h"
#include "list_controller.h"
#include "../calculation_store.h"
#include <poincare/variable_context.h>
#include <apps/i18n.h>

namespace Calculation {

class IllustratedListController : public ListController, public Escher::SelectableTableViewDelegate {
public:
  IllustratedListController(EditExpressionController * editExpressionController);

  // Responder
  void viewDidDisappear() override;
  void didEnterResponderChain(Responder * previousFirstResponder) override;

  // MemoizedListViewDataSource
  int numberOfRows() const override;
  int reusableCellCount(int type) override;
  Escher::HighlightCell * reusableCell(int index, int type) override;
  KDCoordinate nonMemoizedRowHeight(int j) override;
  int typeAtIndex(int index) const override { return index == 0 ? k_illustrationCellType : k_additionalCalculationCellType; }
  void willDisplayCellForIndex(Escher::HighlightCell * cell, int index) override;

  // SelectableTableViewDelegate
  void tableViewDidChangeSelection(Escher::SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY, bool withinTemporarySelection) override;
  void tableViewDidChangeSelectionAndDidScroll(Escher::SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY, bool withinTemporarySelection) override;

  // IllustratedListController
  void setExpression(Poincare::Expression e) override;

  constexpr static KDCoordinate k_illustrationHeight = 120;
protected:
  constexpr static uint8_t k_illustrationCellType = 0;
  constexpr static uint8_t k_additionalCalculationCellType = 1;
  static KDCoordinate CalculationHeight(Calculation * c, Poincare::Context * context, bool expanded) { return ScrollableThreeExpressionsCell::Height(c, context); }
  Poincare::VariableContext illustratedListContext();
  Poincare::Expression m_expression;
  CalculationStore m_calculationStore;
private:
  int textAtIndex(char * buffer, size_t bufferSize, int index) override;
  virtual const char * symbol() const = 0;
  // Set the size of the buffer needed to store the additional calculation
  constexpr static int k_maxNumberOfAdditionalCalculations = 4;
  constexpr static int k_calculationStoreBufferSize = k_maxNumberOfAdditionalCalculations * (Calculation::k_minimalSize + sizeof(Calculation *));
  char m_calculationStoreBuffer[k_calculationStoreBufferSize];
  // Cells
  virtual Escher::HighlightCell * illustrationCell() = 0;
  ScrollableThreeExpressionsCell m_additionalCalculationCells[k_maxNumberOfAdditionalCalculations];
};

}

#endif


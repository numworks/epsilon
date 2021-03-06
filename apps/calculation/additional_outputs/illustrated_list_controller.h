#ifndef CALCULATION_ADDITIONAL_OUTPUTS_ILLUSTRATED_LIST_CONTROLLER_H
#define CALCULATION_ADDITIONAL_OUTPUTS_ILLUSTRATED_LIST_CONTROLLER_H

#include <escher.h>
#include "scrollable_three_expressions_cell.h"
#include "list_controller.h"
#include "../calculation_store.h"
#include <apps/i18n.h>

namespace Calculation {

class IllustratedListController : public ListController, public SelectableTableViewDelegate {
public:
  IllustratedListController(EditExpressionController * editExpressionController);

  // Responder
  void viewDidDisappear() override;
  void didEnterResponderChain(Responder * previousFirstResponder) override;

  //ListViewDataSource
  int numberOfRows() const override;
  int reusableCellCount(int type) override;
  HighlightCell * reusableCell(int index, int type) override;
  KDCoordinate rowHeight(int j) override;
  int typeAtLocation(int i, int j) override;
  void willDisplayCellForIndex(HighlightCell * cell, int index) override;

  // SelectableTableViewDelegate
  void tableViewDidChangeSelection(SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY, bool withinTemporarySelection) override;

  // IllustratedListController
  void setExpression(Poincare::Expression e) override;

  constexpr static KDCoordinate k_illustrationHeight = 120;
protected:
  static KDCoordinate CalculationHeight(Calculation * c, bool expanded) { return ScrollableThreeExpressionsCell::Height(c); }
  Poincare::Expression m_savedExpression;
  CalculationStore m_calculationStore;
private:
  int textAtIndex(char * buffer, size_t bufferSize, int index) override;
  virtual CodePoint expressionSymbol() const = 0;
  // Set the size of the buffer needed to store the additional calculation
  constexpr static int k_maxNumberOfAdditionalCalculations = 4;
  constexpr static int k_calculationStoreBufferSize = k_maxNumberOfAdditionalCalculations * (sizeof(Calculation) + Calculation::k_numberOfExpressions * Constant::MaxSerializedExpressionSize + sizeof(Calculation *));
  char m_calculationStoreBuffer[k_calculationStoreBufferSize];
  // Cells
  virtual HighlightCell * illustrationCell() = 0;
  ScrollableThreeExpressionsCell m_additionalCalculationCells[k_maxNumberOfAdditionalCalculations];
};

}

#endif


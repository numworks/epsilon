#ifndef CALCULATION_ADDITIONAL_OUTPUTS_ILLUSTRATED_EXPRESSIONS_LIST_CONTROLLER_H
#define CALCULATION_ADDITIONAL_OUTPUTS_ILLUSTRATED_EXPRESSIONS_LIST_CONTROLLER_H

#include "expressions_list_controller.h"
#include "../calculation_store.h"
#include <poincare/variable_context.h>
#include <apps/i18n.h>

namespace Calculation {

class IllustratedExpressionsListController : public ExpressionsListController, public Escher::SelectableTableViewDelegate {
public:
  IllustratedExpressionsListController(EditExpressionController * editExpressionController);

  // Responder
  void didEnterResponderChain(Responder * previousFirstResponder) override;

  // MemoizedListViewDataSource
  int numberOfRows() const override;
  int reusableCellCount(int type) override;
  Escher::HighlightCell * reusableCell(int index, int type) override;
  KDCoordinate nonMemoizedRowHeight(int j) override;
  int typeAtIndex(int index) override { return index == 0 && showIllustration() ? k_illustrationCellType : k_expressionCellType; }
  void willDisplayCellForIndex(Escher::HighlightCell * cell, int index) override;

  // SelectableTableViewDelegate
  void tableViewDidChangeSelection(Escher::SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY, bool withinTemporarySelection) override;

  constexpr static KDCoordinate k_illustrationHeight = 105;
protected:
  constexpr static uint8_t k_illustrationCellType = 0;
  constexpr static uint8_t k_expressionCellType = 1;
  Poincare::VariableContext illustratedExpressionsListContext();
  bool showIllustration() const { return m_showIllustration; }
  void setShowIllustration(bool showIllustration) { m_showIllustration = showIllustration; }
private:
  int textAtIndex(char * buffer, size_t bufferSize, int index) override;
  virtual const char * symbol() const = 0;
  // Cells
  virtual Escher::HighlightCell * illustrationCell() = 0;
  bool m_showIllustration;
};

}

#endif


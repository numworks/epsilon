#ifndef CALCULATION_ILLUSTRATED_EXPRESSIONS_LIST_CONTROLLER_H
#define CALCULATION_ILLUSTRATED_EXPRESSIONS_LIST_CONTROLLER_H

#include <apps/i18n.h>

#include "../calculation_store.h"
#include "chained_expressions_list_controller.h"
#include "illustration_cell.h"

namespace Calculation {

class IllustratedExpressionsListController
    : public ChainedExpressionsListController,
      public Escher::SelectableListViewDelegate {
 public:
  IllustratedExpressionsListController(
      EditExpressionController* editExpressionController,
      bool highlightWholeCells = false)
      : ChainedExpressionsListController(editExpressionController,
                                         highlightWholeCells, this) {}

  // Responder
  void didBecomeFirstResponder() override;
  // ViewController
  void viewWillAppear() override;

  // MemoizedListViewDataSource
  int numberOfRows() const override;
  int reusableCellCount(int type) const override;
  Escher::HighlightCell* reusableCell(int index, int type) override;
  KDCoordinate nonMemoizedRowHeight(int row) override;
  int typeAtRow(int row) const override {
    return row == 0 ? k_illustrationCellType : k_expressionCellType;
  }
  void fillCellForRow(Escher::HighlightCell* cell, int row) override;

  /* Since we can't focus the illustration, we need to keep its height small
   * enough for a sufficient part of the first expressionCell to be visible when
   * the focus is there and the table view scrolled at the top. */
  constexpr static KDCoordinate k_illustrationHeight = 120;

 protected:
  constexpr static uint8_t k_illustrationCellType = 0;
  constexpr static uint8_t k_expressionCellType = 1;
  void setShowIllustration(bool showIllustration);
  /* Helper to add a line in the format func(arg) = exact ~ approx */
  void setLineAtIndex(int index, const Poincare::UserExpression formula,
                      const Poincare::UserExpression expression,
                      const Poincare::Internal::ProjectionContext* ctx);

 private:
  Poincare::Layout layoutAtIndex(Escher::HighlightCell* cell,
                                 int index) override;
  virtual KDCoordinate illustrationHeight() { return k_illustrationHeight; }
  virtual IllustrationCell* illustrationCell() = 0;
};

}  // namespace Calculation

#endif

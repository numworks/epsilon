#ifndef APPS_MATH_TOOLBOX_H
#define APPS_MATH_TOOLBOX_H

#include <apps/i18n.h>
#include <escher/container.h>
#include <escher/menu_cell.h>
#include <escher/message_text_view.h>
#include <escher/scrollable_expression_view.h>
#include <escher/toolbox.h>

class MathToolbox : public Escher::Toolbox {
 public:
  MathToolbox();
  // View Controller
  void viewDidDisappear() override;
  // MemoizedListViewDataSource
  KDCoordinate nonMemoizedRowHeight(int j) override;
  void willDisplayCellForIndex(Escher::HighlightCell* cell, int index) override;

 protected:
  /* These could have a ScrollableExpressionView as label, but it's not
   * necessary for now since no leaf of toolbox is longer than the cell width.
   */
  using LeafCell =
      Escher::MenuCell<Escher::ExpressionView, Escher::MessageTextView>;
  bool selectSubMenu(int selectedRow) override;
  bool selectLeaf(int selectedRow) override;
  const Escher::ToolboxMessageTree* rootModel() const override;
  LeafCell* leafCellAtIndex(int index) override;
  Escher::NestedMenuController::NodeCell* nodeCellAtIndex(int index) override;
  int maxNumberOfDisplayedRows() override;
  int controlChecksum() const override;
  constexpr static int k_maxNumberOfDisplayedRows =
      Escher::Metric::MinimalNumberOfScrollableRowsToFillDisplayHeight(
          Escher::AbstractMenuCell::k_minimalLargeFontCellHeight,
          Escher::Metric::PopUpTopMargin + Escher::Metric::StackTitleHeight);

 private:
  bool isMessageTreeDisabled(
      const Escher::ToolboxMessageTree* messageTree) const;
  bool displayMessageTreeDisabledPopUp(
      const Escher::ToolboxMessageTree* messageTree);
  int indexAfterFork(
      const Escher::ToolboxMessageTree* forkMessageTree) const override;

  LeafCell m_leafCells[k_maxNumberOfDisplayedRows];
  Escher::NestedMenuController::NodeCell
      m_nodeCells[k_maxNumberOfDisplayedRows];
};

#endif

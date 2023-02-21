#ifndef APPS_MATH_TOOLBOX_H
#define APPS_MATH_TOOLBOX_H

#include <apps/i18n.h>
#include <escher/container.h>
#include <escher/expression_table_cell_with_message.h>
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
  bool selectSubMenu(int selectedRow) override;
  bool selectLeaf(int selectedRow) override;
  const Escher::ToolboxMessageTree* rootModel() const override;
  Escher::ExpressionTableCellWithMessage* leafCellAtIndex(int index) override;
  Escher::MessageTableCellWithChevron* nodeCellAtIndex(int index) override;
  int maxNumberOfDisplayedRows() override;
  int controlChecksum() const override;
  constexpr static int k_maxNumberOfDisplayedRows =
      Escher::Metric::MinimalNumberOfScrollableRowsToFillDisplayHeight(
          Escher::TableCell::k_minimalLargeFontCellHeight,
          Escher::Metric::PopUpTopMargin + Escher::Metric::StackTitleHeight);

 private:
  bool isMessageTreeDisabled(
      const Escher::ToolboxMessageTree* messageTree) const;
  bool displayMessageTreeDisabledPopUp(
      const Escher::ToolboxMessageTree* messageTree);
  int indexAfterFork(
      const Escher::ToolboxMessageTree* forkMessageTree) const override;

  Escher::ExpressionTableCellWithMessage
      m_leafCells[k_maxNumberOfDisplayedRows];
  Escher::MessageTableCellWithChevron m_nodeCells[k_maxNumberOfDisplayedRows];
};

#endif

#ifndef APPS_MATH_TOOLBOX_CONTROLLER_H
#define APPS_MATH_TOOLBOX_CONTROLLER_H

#include <apps/i18n.h>
#include <escher/container.h>
#include <escher/menu_cell.h>
#include <escher/message_text_view.h>
#include <escher/scrollable_layout_view.h>
#include <escher/toolbox.h>

namespace Shared {

class MathToolboxExtraCellsDataSource {
 public:
  virtual int numberOfExtraCells() = 0;
  virtual Poincare::Layout extraCellLayoutAtRow(int row) = 0;
};

class MathToolboxController : public Escher::Toolbox {
 public:
  MathToolboxController();
  bool handleEvent(Ion::Events::Event event) override;
  // View Controller
  void viewDidDisappear() override;
  // MemoizedListViewDataSource
  int numberOfRows() const override;
  int typeAtRow(int row) const override;
  KDCoordinate nonMemoizedRowHeight(int row) override;
  void fillCellForRow(Escher::HighlightCell* cell, int row) override;

  void setExtraCellsDataSource(MathToolboxExtraCellsDataSource* dataSource) {
    m_extraCellsDataSource = dataSource;
  }

 protected:
  /* These could have a ScrollableLayoutView as label, but it's not
   * necessary for now since no leaf of toolbox is longer than the cell width.
   */
  using LeafCell =
      Escher::MenuCell<Escher::LayoutView, Escher::MessageTextView>;
  bool selectSubMenu(int selectedRow) override;
  bool selectLeaf(int selectedRow) override;
  bool selectExtraCell(int selectedRow);
  const Escher::ToolboxMessageTree* rootModel() const override;
  LeafCell* leafCellAtIndex(int index) override;
  Escher::NestedMenuController::NodeCell* nodeCellAtIndex(int index) override;
  const Escher::ToolboxMessageTree* messageTreeModelAtIndex(
      int index) const override;
  int maxNumberOfDisplayedRows() override;
  int controlChecksum() const override;
  constexpr static int k_maxNumberOfDisplayedRows =
      Escher::Metric::MinimalNumberOfScrollableRowsToFillDisplayHeight(
          Escher::AbstractMenuCell::k_minimalLargeFontCellHeight,
          Escher::Metric::PopUpMargins.top() +
              Escher::Metric::StackTitleHeight);

 private:
  constexpr static size_t k_maxSizeOfExtraCellExpression = 10;
  bool isMessageTreeDisabled(
      const Escher::ToolboxMessageTree* messageTree) const;
  bool displayMessageTreeDisabledPopUp(
      const Escher::ToolboxMessageTree* messageTree);
  int indexAfterFork(
      const Escher::ToolboxMessageTree* forkMessageTree) const override;
  int numberOfExtraCellsInCurrentMenu() const;

  LeafCell m_leafCells[k_maxNumberOfDisplayedRows];
  Escher::NestedMenuController::NodeCell
      m_nodeCells[k_maxNumberOfDisplayedRows];
  // These cells are at the top of the root menu
  MathToolboxExtraCellsDataSource* m_extraCellsDataSource;
};

}  // namespace Shared

#endif

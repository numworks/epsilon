#ifndef CODE_PYTHON_TOOLBOX_H
#define CODE_PYTHON_TOOLBOX_H

#include <apps/i18n.h>
#include <escher/container.h>
#include <escher/menu_cell.h>
#include <escher/toolbox.h>
#include <ion/events.h>
#include <kandinsky/font.h>

namespace Code {

using ToolboxLeafCell =
    Escher::MenuCell<Escher::MessageTextView, Escher::MessageTextView>;

class PythonToolbox : public Escher::Toolbox {
 public:
  // PythonToolbox
  PythonToolbox();
  const Escher::ToolboxMessageTree* moduleChildren(const char* name,
                                                   int* numberOfNodes) const;

  // Toolbox
  bool handleEvent(Ion::Events::Event event) override;

  // MemoizedListViewDataSource
  KDCoordinate nonMemoizedRowHeight(int j) override;
  void willDisplayCellForIndex(Escher::HighlightCell* cell, int index) override;

 protected:
  bool selectLeaf(int selectedRow) override;
  const Escher::ToolboxMessageTree* rootModel() const override;
  ToolboxLeafCell* leafCellAtIndex(int index) override;
  Escher::MessageTableCellWithChevron* nodeCellAtIndex(int index) override;
  int maxNumberOfDisplayedRows() override;
  constexpr static int k_maxNumberOfDisplayedRows =
      Escher::Metric::MinimalNumberOfScrollableRowsToFillDisplayHeight(
          Escher::TableCell::k_minimalSmallFontCellHeight,
          Escher::Metric::PopUpTopMargin + Escher::Metric::StackTitleHeight);

 private:
  void scrollToLetter(char letter);
  void scrollToAndSelectChild(int i);
  ToolboxLeafCell m_leafCells[k_maxNumberOfDisplayedRows];
  Escher::MessageTableCellWithChevron m_nodeCells[k_maxNumberOfDisplayedRows];
};

}  // namespace Code

#endif

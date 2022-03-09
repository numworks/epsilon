#ifndef GRAPH_FUNCTION_TOOLBOX_H
#define GRAPH_FUNCTION_TOOLBOX_H

#include <escher/expression_table_cell.h>
#include "../../math_toolbox.h"
#include <poincare/layout.h>

namespace Graph {

class FunctionToolbox : public MathToolbox {
public:
  FunctionToolbox();
  bool handleEvent(Ion::Events::Event event) override;
  int numberOfRows() const override;
  KDCoordinate nonMemoizedRowHeight(int index) override;
  int reusableCellCount(int type) override;
  Escher::HighlightCell * reusableCell(int index, int type) override;
  void willDisplayCellForIndex(Escher::HighlightCell * cell, int index) override;
  int typeAtIndex(int index) override;
protected:
  const Escher::ToolboxMessageTree * messageTreeModelAtIndex(int index) const override {
    assert(index >= addedCellsAtRoot());
    return MathToolbox::messageTreeModelAtIndex(index - addedCellsAtRoot());
  }
private:
  static constexpr int k_addedCellType = 2;
  static constexpr int k_numberOfAddedCells = 2;
  // At root depth, there are additional rows to display.
  int addedCellsAtRoot() const { return m_messageTreeModel == rootModel() ? k_numberOfAddedCells : 0; }
  bool selectAddedCell(int selectedRow);
  Escher::ExpressionTableCell m_addedCells[k_numberOfAddedCells];
  Poincare::Layout m_addedCellLayout[k_numberOfAddedCells];
};

}

#endif

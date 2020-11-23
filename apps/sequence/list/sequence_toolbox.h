#ifndef SEQUENCE_SEQUENCE_TOOLBOX_H
#define SEQUENCE_SEQUENCE_TOOLBOX_H

#include "../../math_toolbox.h"

#include <poincare/layout.h>

namespace Sequence {

class SequenceToolbox : public MathToolbox {
public:
  SequenceToolbox();
  bool handleEvent(Ion::Events::Event event) override;
  int numberOfRows() const override;
  HighlightCell * reusableCell(int index, int type) override;
  void willDisplayCellForIndex(HighlightCell * cell, int index) override;
  int typeAtLocation(int i, int j) override;
  void buildExtraCellsLayouts(const char * sequenceName, int recurrenceDepth);
private:
  int stackRowIndex(int selectedRow) override {
    /* At 0 depth, mathToolboxIndex() offset must be removed when calling
     * NestedMenuController::push() so that the pushed row is correct when
     * popped in NestedMenuController::returnToPreviousMenu(). */
    return stackDepth() == 0 ? selectedRow + m_numberOfAddedCells : selectedRow;
  };
  bool selectAddedCell(int selectedRow);
  int mathToolboxIndex(int index);
  ExpressionTableCell m_addedCells[k_maxNumberOfDisplayedRows];
  Poincare::Layout m_addedCellLayout[k_maxNumberOfDisplayedRows];
  int m_numberOfAddedCells;
};

}

#endif

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
  /* At 0 depth, there are additional rows to display. With the exception of
   * NestedMenuController::returnToPreviousMenu(), it must be ignored in
   * parent's classes. */
  int stackRowOffset() const override { return stackDepth() == 0 ? m_numberOfAddedCells : 0; }
  bool selectAddedCell(int selectedRow);
  ExpressionTableCell m_addedCells[k_maxNumberOfDisplayedRows];
  Poincare::Layout m_addedCellLayout[k_maxNumberOfDisplayedRows];
  int m_numberOfAddedCells;
};

}

#endif

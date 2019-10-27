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
  bool selectAddedCell(int selectedRow);
  int mathToolboxIndex(int index);
  ExpressionTableCell m_addedCells[k_maxNumberOfDisplayedRows];
  Poincare::Layout m_addedCellLayout[k_maxNumberOfDisplayedRows];
  int m_numberOfAddedCells;
};

}

#endif

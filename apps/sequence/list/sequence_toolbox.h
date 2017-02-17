#ifndef SEQUENCE_SEQUENCE_TOOLBOX_H
#define SEQUENCE_SEQUENCE_TOOLBOX_H

#include "../../math_toolbox.h"
#include "../sequence_store.h"

namespace Sequence {

class SequenceToolbox : public MathToolbox {
public:
  SequenceToolbox(SequenceStore * sequenceStore);
  bool handleEvent(Ion::Events::Event event) override;
  int numberOfRows() override;
  HighlightCell * reusableCell(int index, int type) override;
  void willDisplayCellForIndex(HighlightCell * cell, int index) override;
  KDCoordinate rowHeight(int j) override;
  int typeAtLocation(int i, int j) override;
  void addCells(int recurrenceDepth);
private:
  bool selectAddedCell(int selectedRow);
  constexpr static KDCoordinate k_addedRowHeight = 20;
  ExpressionTableCell m_addedCells[k_maxNumberOfDisplayedRows];
  Poincare::ExpressionLayout * m_addedCellLayout[k_maxNumberOfDisplayedRows];
  int m_numberOfAddedCells;
  SequenceStore * m_sequenceStore;
};

}

#endif

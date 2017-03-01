#ifndef SEQUENCE_VALUES_CONTROLLER_H
#define SEQUENCE_VALUES_CONTROLLER_H

#include "../sequence_store.h"
#include "../sequence_title_cell.h"
#include "../../shared/values_controller.h"

namespace Sequence {

class ValuesController : public Shared::ValuesController {
public:
  ValuesController(Responder * parentResponder, SequenceStore * sequenceStore, ButtonRowController * header);
  int numberOfColumns() override;
  void willDisplayCellAtLocation(HighlightCell * cell, int i, int j) override;
  const char * emptyMessage() override;
private:
  int maxNumberOfCells() override;
  int maxNumberOfFunctions() override;
  constexpr static int k_maxNumberOfCells = 30;
  constexpr static int k_maxNumberOfSequences = 3;
  SequenceTitleCell m_sequenceTitleCells[k_maxNumberOfSequences];
  SequenceTitleCell * functionTitleCells(int j) override;
  EvenOddBufferTextCell m_floatCells[k_maxNumberOfCells];
  EvenOddBufferTextCell * floatCells(int j) override;
  SequenceStore * m_sequenceStore;
  SequenceStore * functionStore() const override;
  Shared::ValuesFunctionParameterController m_sequenceParameterController;
  Shared::ValuesFunctionParameterController * functionParameterController() override;
};

}

#endif

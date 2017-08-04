#ifndef SEQUENCE_VALUES_CONTROLLER_H
#define SEQUENCE_VALUES_CONTROLLER_H

#include "../sequence_store.h"
#include "../sequence_title_cell.h"
#include "../../shared/values_controller.h"
#include "interval_parameter_controller.h"

namespace Sequence {

class ValuesController : public Shared::ValuesController {
public:
  ValuesController(Responder * parentResponder, SequenceStore * sequenceStore, Shared::Interval * interval, ButtonRowController * header);
  void willDisplayCellAtLocation(HighlightCell * cell, int i, int j) override;
  I18n::Message emptyMessage() override;
  IntervalParameterController * intervalParameterController() override;
private:
  bool setDataAtLocation(double floatBody, int columnIndex, int rowIndex) override;
  int maxNumberOfCells() override;
  int maxNumberOfFunctions() override;
  constexpr static int k_maxNumberOfCells = 30;
  constexpr static int k_maxNumberOfSequences = 3;
  SequenceTitleCell * m_sequenceTitleCells[k_maxNumberOfSequences];
  SequenceTitleCell * functionTitleCells(int j) override;
  EvenOddBufferTextCell * m_floatCells[k_maxNumberOfCells];
  EvenOddBufferTextCell * floatCells(int j) override;
  SequenceStore * m_sequenceStore;
  SequenceStore * functionStore() const override;
  View * loadView() override;
  void unloadView(View * view) override;
#if COPY_COLUMN
  Shared::ValuesFunctionParameterController m_sequenceParameterController;
#endif
  Shared::ValuesFunctionParameterController * functionParameterController() override;
  IntervalParameterController m_intervalParameterController;
};

}

#endif

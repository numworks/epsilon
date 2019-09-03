#ifndef SEQUENCE_VALUES_CONTROLLER_H
#define SEQUENCE_VALUES_CONTROLLER_H

#include "../sequence_store.h"
#include "../sequence_title_cell.h"
#include "../../shared/values_controller.h"
#include "interval_parameter_controller.h"

namespace Sequence {

class ValuesController : public Shared::ValuesController {
public:
  ValuesController(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate, Shared::Interval * interval, ButtonRowController * header);
  Button * buttonAtIndex(int index, ButtonRowController::Position position) const override {
    return const_cast<Button *>(&m_setIntervalButton);
  }
  void willDisplayCellAtLocation(HighlightCell * cell, int i, int j) override;
  I18n::Message emptyMessage() override;
  IntervalParameterController * intervalParameterController() override {
    return &m_intervalParameterController;
  }
private:
  bool setDataAtLocation(double floatBody, int columnIndex, int rowIndex) override;
  int maxNumberOfCells() override { return k_maxNumberOfCells; }
  int maxNumberOfFunctions() override { return k_maxNumberOfSequences; }
  constexpr static int k_maxNumberOfSequences = 3;
  constexpr static int k_maxNumberOfCells = k_maxNumberOfSequences * k_maxNumberOfRows;

  SequenceStore * functionStore() const override { return static_cast<SequenceStore *>(Shared::ValuesController::functionStore()); }
  ViewController * functionParameterController() override;
  I18n::Message valuesParameterControllerPageTitle() const override;
  int abscissaCellsCount() const override { return k_maxNumberOfRows; }
  EvenOddEditableTextCell * abscissaCells(int j) override {
    assert (j >= 0 && j < k_maxNumberOfRows);
    return &m_abscissaCells[j];
  }
  int abscissaTitleCellsCount() const override { return 1; }
  EvenOddMessageTextCell * abscissaTitleCells(int j) override {
    assert (j >= 0 && j < abscissaTitleCellsCount());
    return &m_abscissaTitleCell;
  }
  SequenceTitleCell * functionTitleCells(int j) override {
    assert(j >= 0 && j < k_maxNumberOfSequences);
    return &m_sequenceTitleCells[j];
  }
  EvenOddBufferTextCell * floatCells(int j) override {
    assert(j >= 0 && j < k_maxNumberOfCells);
    return &m_floatCells[j];
  }
  SequenceTitleCell m_sequenceTitleCells[k_maxNumberOfSequences];
  EvenOddBufferTextCell m_floatCells[k_maxNumberOfCells];
  EvenOddMessageTextCell m_abscissaTitleCell;
  EvenOddEditableTextCell m_abscissaCells[k_maxNumberOfRows];
#if COPY_COLUMN
  Shared::ValuesFunctionParameterController m_sequenceParameterController;
#endif
  IntervalParameterController m_intervalParameterController;
  Button m_setIntervalButton;
};

}

#endif

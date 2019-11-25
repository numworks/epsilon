#ifndef SEQUENCE_VALUES_CONTROLLER_H
#define SEQUENCE_VALUES_CONTROLLER_H

#include "../sequence_store.h"
#include "../sequence_title_cell.h"
#include "../../shared/values_controller.h"
#include "interval_parameter_controller.h"

namespace Sequence {

class ValuesController : public Shared::ValuesController {
public:
  ValuesController(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate, ButtonRowController * header);

  // TableViewDataSource
  KDCoordinate columnWidth(int i) override;
  void willDisplayCellAtLocation(HighlightCell * cell, int i, int j) override;

  // ButtonRowDelegate
  Button * buttonAtIndex(int index, ButtonRowController::Position position) const override {
    return const_cast<Button *>(&m_setIntervalButton);
  }

  // AlternateEmptyViewDelegate
  I18n::Message emptyMessage() override;

  // Parameters controllers getters
  IntervalParameterController * intervalParameterController() override {
    return &m_intervalParameterController;
  }
private:
  constexpr static int k_maxNumberOfDisplayableSequences = 3;
  constexpr static int k_maxNumberOfDisplayableCells = k_maxNumberOfDisplayableSequences * k_maxNumberOfDisplayableRows;

  // ValuesController
  void setStartEndMessages(Shared::IntervalParameterController * controller, int column) override {
    setDefaultStartEndMessages();
  }

  void setDefaultStartEndMessages();
  I18n::Message valuesParameterMessageAtColumn(int columnIndex) const override;
  int maxNumberOfCells() override { return k_maxNumberOfDisplayableCells; }
  int maxNumberOfFunctions() override { return k_maxNumberOfDisplayableSequences; }

  // EditableCellViewController
  bool setDataAtLocation(double floatBody, int columnIndex, int rowIndex) override;

  // Model getters
  SequenceStore * functionStore() const override { return static_cast<SequenceStore *>(Shared::ValuesController::functionStore()); }
  Shared::Interval * intervalAtColumn(int columnIndex) override;

  // Function evaluation memoization
  static constexpr int k_valuesCellBufferSize = Poincare::PrintFloat::charSizeForFloatsWithPrecision(Poincare::Preferences::LargeNumberOfSignificantDigits);
  char * memoizedBufferAtIndex(int i) override {
    assert(i >= 0 && i < k_maxNumberOfDisplayableCells);
    return m_memoizedBuffer[i];
  }
  int valuesCellBufferSize() const override{ return k_valuesCellBufferSize; }
  int numberOfMemoizedColumn() override { return k_maxNumberOfDisplayableSequences; }
  void fillMemoizedBuffer(int i, int j, int index) override;


  // Parameters controllers getter
  ViewController * functionParameterController() override;

  // Cells & view
  SelectableTableView * selectableTableView() override { return &m_selectableTableView; }
  int abscissaCellsCount() const override { return k_maxNumberOfDisplayableRows; }
  EvenOddEditableTextCell * abscissaCells(int j) override {
    assert (j >= 0 && j < k_maxNumberOfDisplayableRows);
    return &m_abscissaCells[j];
  }
  int abscissaTitleCellsCount() const override { return 1; }
  EvenOddMessageTextCell * abscissaTitleCells(int j) override {
    assert (j >= 0 && j < abscissaTitleCellsCount());
    return &m_abscissaTitleCell;
  }
  SequenceTitleCell * functionTitleCells(int j) override {
    assert(j >= 0 && j < k_maxNumberOfDisplayableSequences);
    return &m_sequenceTitleCells[j];
  }
  EvenOddBufferTextCell * floatCells(int j) override {
    assert(j >= 0 && j < k_maxNumberOfDisplayableCells);
    return &m_floatCells[j];
  }

  SelectableTableView m_selectableTableView;
  SequenceTitleCell m_sequenceTitleCells[k_maxNumberOfDisplayableSequences];
  EvenOddBufferTextCell m_floatCells[k_maxNumberOfDisplayableCells];
  EvenOddMessageTextCell m_abscissaTitleCell;
  EvenOddEditableTextCell m_abscissaCells[k_maxNumberOfDisplayableRows];
#if COPY_COLUMN
  Shared::ValuesFunctionParameterController m_sequenceParameterController;
#endif
  IntervalParameterController m_intervalParameterController;
  Button m_setIntervalButton;
  mutable char m_memoizedBuffer[k_maxNumberOfDisplayableCells][k_valuesCellBufferSize];
};

}

#endif

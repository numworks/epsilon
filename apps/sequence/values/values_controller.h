#ifndef SEQUENCE_VALUES_CONTROLLER_H
#define SEQUENCE_VALUES_CONTROLLER_H

#include "../../shared/sequence_store.h"
#include "../../shared/sequence_title_cell.h"
#include "../../shared/values_controller.h"
#include "interval_parameter_controller.h"

namespace Sequence {

class ValuesController : public Shared::ValuesController {
public:
  ValuesController(Escher::Responder * parentResponder, Escher::InputEventHandlerDelegate * inputEventHandlerDelegate, Escher::ButtonRowController * header);

  // TableViewDataSource
  KDCoordinate columnWidth(int i) override;

  // ButtonRowDelegate
  Escher::Button * buttonAtIndex(int index, Escher::ButtonRowController::Position position) const override {
    return const_cast<Escher::Button *>(&m_setIntervalButton);
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
  Shared::PrefacedTableView * prefacedView() override { return &m_prefacedView; }

  void setDefaultStartEndMessages();
  I18n::Message valuesParameterMessageAtColumn(int columnIndex) const override;
  int maxNumberOfCells() override { return k_maxNumberOfDisplayableCells; }
  int maxNumberOfFunctions() override { return k_maxNumberOfDisplayableSequences; }

  // EditableCellViewController
  bool checkDataAtLocation(double floatBody, int columnIndex, int rowIndex) const override { return floatBody >= 0.0; }
  bool setDataAtLocation(double floatBody, int columnIndex, int rowIndex) override;

  // Model getters
  Shared::SequenceStore * functionStore() const override { return static_cast<Shared::SequenceStore *>(Shared::ValuesController::functionStore()); }
  Shared::Interval * intervalAtColumn(int columnIndex) override;

  // Function evaluation memoization
  static constexpr int k_valuesCellBufferSize = Poincare::PrintFloat::charSizeForFloatsWithPrecision(Poincare::Preferences::VeryLargeNumberOfSignificantDigits);
  char * memoizedBufferAtIndex(int i) override {
    assert(i >= 0 && i < k_maxNumberOfDisplayableCells);
    return m_memoizedBuffer[i];
  }
  int valuesCellBufferSize() const override{ return k_valuesCellBufferSize; }
  int numberOfMemoizedColumn() override { return k_maxNumberOfDisplayableSequences; }
  void fillMemoizedBuffer(int i, int j, int index) override;


  // Parameters controllers getter
  Shared::ColumnParameterController * functionParameterController() override { return nullptr; }

  // Cells & view
  Escher::SelectableTableView * selectableTableView() override { return &m_selectableTableView; }
  int abscissaCellsCount() const override { return k_maxNumberOfDisplayableRows; }
  Escher::EvenOddEditableTextCell * abscissaCells(int j) override {
    assert (j >= 0 && j < k_maxNumberOfDisplayableRows);
    return &m_abscissaCells[j];
  }
  int abscissaTitleCellsCount() const override { return 1; }
  Escher::EvenOddMessageTextCell * abscissaTitleCells(int j) override {
    assert (j >= 0 && j < abscissaTitleCellsCount());
    return &m_abscissaTitleCell;
  }
  Shared::SequenceTitleCell * functionTitleCells(int j) override {
    assert(j >= 0 && j < k_maxNumberOfDisplayableSequences);
    return &m_sequenceTitleCells[j];
  }
  Escher::EvenOddBufferTextCell * floatCells(int j) override {
    assert(j >= 0 && j < k_maxNumberOfDisplayableCells);
    return &m_floatCells[j];
  }

  int fillColumnName(int columnIndex, char * buffer) override;
  void setTitleCellText(Escher::HighlightCell * titleCell, int columnIndex) override;

  Escher::SelectableTableView m_selectableTableView;
  Shared::PrefacedTableView m_prefacedView;
  Shared::SequenceTitleCell m_sequenceTitleCells[k_maxNumberOfDisplayableSequences];
  Escher::EvenOddBufferTextCell m_floatCells[k_maxNumberOfDisplayableCells];
  Escher::EvenOddMessageTextCell m_abscissaTitleCell;
  Escher::EvenOddEditableTextCell m_abscissaCells[k_maxNumberOfDisplayableRows];

  IntervalParameterController m_intervalParameterController;
  Escher::Button m_setIntervalButton;
  mutable char m_memoizedBuffer[k_maxNumberOfDisplayableCells][k_valuesCellBufferSize];
};

}

#endif

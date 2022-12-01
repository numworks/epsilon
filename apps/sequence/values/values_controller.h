#ifndef SEQUENCE_VALUES_CONTROLLER_H
#define SEQUENCE_VALUES_CONTROLLER_H

#include <apps/shared/editable_cell_selectable_table_view.h>
#include <apps/shared/expression_function_title_cell.h>
#include <apps/shared/prefaced_twice_table_view.h>
#include <apps/shared/sequence_store.h>
#include <apps/shared/values_controller.h>
#include "interval_parameter_controller.h"

namespace Sequence {

class ValuesController : public Shared::ValuesController {
public:
  ValuesController(Escher::Responder * parentResponder, Escher::InputEventHandlerDelegate * inputEventHandlerDelegate, Escher::ButtonRowController * header);

  // ViewController
  Escher::View * view() override { return &m_prefacedTwiceTableView; }

  // ButtonRowDelegate
  Escher::AbstractButtonCell * buttonAtIndex(int index, Escher::ButtonRowController::Position position) const override { return const_cast<Escher::AbstractButtonCell *>(&m_setIntervalButton); }

  // AlternateEmptyViewDelegate
  bool isEmpty() const override { return functionStore()->numberOfActiveFunctions() == 0; }
  I18n::Message emptyMessage() override { return functionStore()->numberOfDefinedModels() == 0 ? I18n::Message::NoSequence : I18n::Message::NoActivatedSequence; }

  // Shared::ValuesController
  IntervalParameterController * intervalParameterController() override { return &m_intervalParameterController; }

private:
  constexpr static int k_maxNumberOfDisplayableSequences = 3;
  constexpr static int k_maxNumberOfDisplayableCells = k_maxNumberOfDisplayableSequences * k_maxNumberOfDisplayableRows;
  constexpr static int k_valuesCellBufferSize = Poincare::PrintFloat::charSizeForFloatsWithPrecision(Poincare::Preferences::VeryLargeNumberOfSignificantDigits);

  // TabTableController
  Escher::SelectableTableView * selectableTableView() override { return &m_selectableTableView; }

  // TableViewDataSource
  Escher::TableSize1DManager * columnWidthManager() override { return &m_widthManager; }
  Escher::TableSize1DManager * rowHeightManager() override { return &m_heightManager; }

  // ColumnHelper
  int fillColumnName(int columnIndex, char * buffer) override;

  // EditableCellTableViewController
  void setTitleCellText(Escher::HighlightCell * titleCell, int columnIndex) override; 
  bool checkDataAtLocation(double floatBody, int columnIndex, int rowIndex) const override { return floatBody >= 0.0; }
  bool setDataAtLocation(double floatBody, int columnIndex, int rowIndex) override;

  // Shared::ValuesController
  Shared::SequenceStore * functionStore() const override { return static_cast<Shared::SequenceStore *>(Shared::ValuesController::functionStore()); }
  int numberOfMemoizedColumn() override { return k_maxNumberOfDisplayableSequences; }
  Poincare::Layout * memoizedLayoutAtIndex(int i) override;
  Shared::PrefacedTableView * prefacedView() override { return &m_prefacedTwiceTableView; }
  void setStartEndMessages(Shared::IntervalParameterController * controller, int column) override { setDefaultStartEndMessages(); }
  void createMemoizedLayout(int i, int j, int index) override;
  Shared::Interval * intervalAtColumn(int columnIndex) override;
  I18n::Message valuesParameterMessageAtColumn(int columnIndex) const override { return I18n::Message::N; }
  int maxNumberOfCells() override { return k_maxNumberOfDisplayableCells; }
  int maxNumberOfFunctions() override { return k_maxNumberOfDisplayableSequences; }
  Shared::ExpressionFunctionTitleCell * functionTitleCells(int j) override;
  Escher::EvenOddExpressionCell * valueCells(int j) override;
  int abscissaCellsCount() const override { return k_maxNumberOfDisplayableRows; }
  Escher::EvenOddEditableTextCell * abscissaCells(int j) override;
  int abscissaTitleCellsCount() const override { return 1; }
  Escher::EvenOddMessageTextCell * abscissaTitleCells(int j) override;
  Shared::ColumnParameterController * functionParameterController() override { return nullptr; }
  Shared::ColumnParameters * functionParameters() override { return nullptr; }

  void setDefaultStartEndMessages();

  Shared::PrefacedTwiceTableView m_prefacedTwiceTableView;
  Shared::EditableCellSelectableTableView m_selectableTableView;

  Shared::ExpressionFunctionTitleCell m_sequenceTitleCells[k_maxNumberOfDisplayableSequences];
  Escher::EvenOddExpressionCell m_valueCells[k_maxNumberOfDisplayableCells];
  Escher::EvenOddMessageTextCell m_abscissaTitleCell;
  Escher::EvenOddEditableTextCell m_abscissaCells[k_maxNumberOfDisplayableRows];

  IntervalParameterController m_intervalParameterController;
  Escher::AbstractButtonCell m_setIntervalButton;
  mutable Poincare::Layout m_memoizedLayouts[k_maxNumberOfDisplayableCells];

  Escher::RegularTableSize1DManager m_widthManager;
  Escher::RegularTableSize1DManager m_heightManager;
};

}

#endif

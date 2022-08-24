#ifndef GRAPH_VALUES_CONTROLLER_H
#define GRAPH_VALUES_CONTROLLER_H

#include "../continuous_function_store.h"
#include "../../shared/buffer_function_title_cell.h"
#include "../../shared/hideable_even_odd_buffer_text_cell.h"
#include "../../shared/interval_parameter_controller.h"
#include "../../shared/scrollable_two_expressions_cell.h"
#include "../../shared/store_cell.h"
#include "../../shared/values_controller.h"
#include "abscissa_title_cell.h"
#include "derivative_parameter_controller.h"
#include "function_parameter_controller.h"
#include "interval_parameter_selector_controller.h"

namespace Graph {

class ValuesController : public Shared::ValuesController, public Escher::SelectableTableViewDelegate {
public:
  ValuesController(Escher::Responder * parentResponder, Escher::InputEventHandlerDelegate * inputEventHandlerDelegate, Escher::ButtonRowController * header);

  // View controller
  void viewDidDisappear() override;

  // TableViewDataSource
  KDCoordinate columnWidth(int i) override;
  KDCoordinate cumulatedWidthFromIndex(int i) override;
  int indexFromCumulatedWidth(KDCoordinate offsetX) override;
  KDCoordinate rowHeight(int j) override;
  KDCoordinate cumulatedHeightFromIndex(int j) override;
  int indexFromCumulatedHeight(KDCoordinate offsetY) override;

  void willDisplayCellAtLocation(Escher::HighlightCell * cell, int i, int j) override;
  Escher::HighlightCell * reusableCell(int index, int type) override { return type == k_exactValueCellType ? &m_exactValueCell : Shared::ValuesController::reusableCell(index, type); }
  int typeAtLocation(int i, int j) override;
  int reusableCellCount(int type) override { return type == k_exactValueCellType ? 1 : Shared::ValuesController::reusableCellCount(type); }

  // SelectableTableViewDelegate
  void tableViewDidChangeSelection(Escher::SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY, bool withinTemporarySelection = false) override;

  // ButtonRowDelegate
  Escher::Button * buttonAtIndex(int index, Escher::ButtonRowController::Position position) const override {
    return const_cast<Escher::Button *>(&m_setIntervalButton);
  }

  // AlternateEmptyViewDelegate
  bool isEmpty() const override;
  I18n::Message emptyMessage() override;

  // Parameters controllers getters
  Shared::IntervalParameterController * intervalParameterController() override {
    return &m_intervalParameterController;
  }
  IntervalParameterSelectorController * intervalParameterSelectorController() {
    return &m_intervalParameterSelectorController;
  }
private:
  constexpr static int k_exactValueCellType = k_notEditableValueCellType + 1;
  constexpr static KDCoordinate k_abscissaCellWidth = k_cellWidth + Escher::Metric::TableSeparatorThickness;
  constexpr static KDCoordinate k_parametricCellWidth = (2*Poincare::PrintFloat::glyphLengthForFloatWithPrecision(Poincare::Preferences::VeryLargeNumberOfSignificantDigits)+3) * KDFont::GlyphWidth(KDFont::Size::Small) + 2*Escher::Metric::SmallCellMargin; // The largest cell is holding "(-1.234567E-123;-1.234567E-123)"
  constexpr static size_t k_maxNumberOfSymbolTypes = Shared::ContinuousFunction::k_numberOfSymbolTypes;
  constexpr static int k_maxNumberOfDisplayableFunctions = 4;
  constexpr static int k_maxNumberOfDisplayableSymbolTypes = 2;
  constexpr static int k_maxNumberOfDisplayableAbscissaCells = k_maxNumberOfDisplayableSymbolTypes * k_maxNumberOfDisplayableRows;
  constexpr static int k_maxNumberOfDisplayableCells = k_maxNumberOfDisplayableFunctions * k_maxNumberOfDisplayableRows;

  // Values controller
  void setStartEndMessages(Shared::IntervalParameterController * controller, int column) override;
  int maxNumberOfCells() override { return k_maxNumberOfDisplayableCells; }
  int maxNumberOfFunctions() override { return k_maxNumberOfDisplayableFunctions; }
  Shared::PrefacedTableView * prefacedView() override { return &m_prefacedView; }

  // Number of columns memoization
  void updateNumberOfColumns() const override;

  // Model getters
  Ion::Storage::Record recordAtColumn(int i) override;
  Ion::Storage::Record recordAtColumn(int i, bool * isDerivative);
  ContinuousFunctionStore * functionStore() const override { return static_cast<ContinuousFunctionStore *>(Shared::ValuesController::functionStore()); }
  Shared::Interval * intervalAtColumn(int columnIndex) override;
  Shared::ExpiringPointer<Shared::ContinuousFunction> functionAtIndex(int column, int row, double * abscissa, bool * isDerivative);

  // Number of columns
  int numberOfColumnsForAbscissaColumn(int column) override;
  int numberOfColumnsForRecord(Ion::Storage::Record record) const;
  int numberOfColumnsForSymbolType(int symbolTypeIndex) const;
  int numberOfAbscissaColumnsBeforeColumn(int column);
  int numberOfValuesColumns() override;
  Shared::ContinuousFunction::SymbolType symbolTypeAtColumn(int * i) const;

  // Function evaluation memoization
  constexpr static int k_valuesCellBufferSize = 2*Poincare::PrintFloat::charSizeForFloatsWithPrecision(Poincare::Preferences::VeryLargeNumberOfSignificantDigits)+3; // The largest buffer holds (-1.234567E-123;-1.234567E-123)
  char * memoizedBufferAtIndex(int i) override {
    assert(i >= 0 && i < k_maxNumberOfDisplayableCells);
    return m_memoizedBuffer[i];
  }
  int valuesCellBufferSize() const override { return k_valuesCellBufferSize; }
  int numberOfMemoizedColumn() override { return k_maxNumberOfDisplayableFunctions; }
  /* The conversion of column coordinates from the absolute table to the table
   * on only values cell depends on the number of abscissa columns which depends
   * on the number of different plot types in the table. */
  int valuesColumnForAbsoluteColumn(int column) override;
  int absoluteColumnForValuesColumn(int column) override;
  void fillMemoizedBuffer(int column, int row, int index) override;
  Poincare::Layout exactValueLayout(int column, int row);

  // Parameter controllers
  Shared::ColumnParameterController * functionParameterController() override;
  I18n::Message valuesParameterMessageAtColumn(int columnIndex) const override;
  /* The paramater i should be the column index and symbolTypeAtColumn changes
   * it to be the relative column index within the sub table. */

  //Column name and title cells
  int fillColumnName(int columnIndex, char * buffer) override;
  void setTitleCellText(Escher::HighlightCell * titleCell, int columnIndex) override;
  void setTitleCellStyle(Escher::HighlightCell * titleCell, int columnIndex) override;

  // Cells & View
  Shared::Hideable * hideableCellFromType(Escher::HighlightCell * cell, int type);
  Shared::BufferFunctionTitleCell * functionTitleCells(int j) override;
  Escher::EvenOddBufferTextCell * floatCells(int j) override;
  int abscissaCellsCount() const override { return k_maxNumberOfDisplayableAbscissaCells; }
  Escher::EvenOddEditableTextCell * abscissaCells(int j) override { assert (j >= 0 && j < k_maxNumberOfDisplayableAbscissaCells); return &m_abscissaCells[j]; }
  int abscissaTitleCellsCount() const override { return k_maxNumberOfDisplayableSymbolTypes; }
  Escher::EvenOddMessageTextCell * abscissaTitleCells(int j) override { assert (j >= 0 && j < abscissaTitleCellsCount()); return &m_abscissaTitleCells[j]; }
  Escher::SelectableTableView * selectableTableView() override { return &m_selectableTableView; }


  /* For parametric function, we display the evaluation with the form "(1;2)".
   * This form is not parsable so when we store it into the clipboard, we want
   * to turn it into a parsable matrix "[[1][2]]". To do so, we use a child
   * class of SelectableTableView to override the behaviour of the responder
   *  when encountering a cut/copy events. */
  class ValuesSelectableTableView : public Escher::SelectableTableView {
  public:
    ValuesSelectableTableView(ValuesController * vc) : Escher::SelectableTableView(vc, vc, vc) {}
    bool handleEvent(Ion::Events::Event event) override;
  };

  ValuesSelectableTableView m_selectableTableView;
  Shared::PrefacedTableView m_prefacedView;
  mutable int m_numberOfValuesColumnsForType[k_maxNumberOfSymbolTypes];
  Shared::BufferFunctionTitleCell m_functionTitleCells[k_maxNumberOfDisplayableFunctions];
  Shared::HideableEvenOddBufferTextCell m_floatCells[k_maxNumberOfDisplayableCells];
  AbscissaTitleCell m_abscissaTitleCells[k_maxNumberOfDisplayableSymbolTypes];
  Shared::StoreCell m_abscissaCells[k_maxNumberOfDisplayableAbscissaCells];
  Shared::ScrollableTwoExpressionsCell m_exactValueCell;
  FunctionParameterController m_functionParameterController;
  Shared::IntervalParameterController m_intervalParameterController;
  IntervalParameterSelectorController m_intervalParameterSelectorController;
  DerivativeParameterController m_derivativeParameterController;
  Escher::Button m_setIntervalButton;
  // TODO specialize buffer size as well
  mutable char m_memoizedBuffer[k_maxNumberOfDisplayableCells][k_valuesCellBufferSize];
};

}

#endif

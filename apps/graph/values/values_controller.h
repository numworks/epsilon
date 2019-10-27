#ifndef GRAPH_VALUES_CONTROLLER_H
#define GRAPH_VALUES_CONTROLLER_H

#include "../continuous_function_store.h"
#include "../../shared/buffer_function_title_cell.h"
#include "../../shared/hideable_even_odd_buffer_text_cell.h"
#include "../../shared/values_controller.h"
#include "../../shared/interval_parameter_controller.h"
#include "../../shared/store_cell.h"
#include "abscissa_title_cell.h"
#include "interval_parameter_selector_controller.h"
#include "derivative_parameter_controller.h"
#include "function_parameter_controller.h"

namespace Graph {

class ValuesController : public Shared::ValuesController, public SelectableTableViewDelegate {
public:
  ValuesController(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate, ButtonRowController * header);

  // TableViewDataSource
  KDCoordinate columnWidth(int i) override;
  KDCoordinate cumulatedWidthFromIndex(int i) override;
  int indexFromCumulatedWidth(KDCoordinate offsetX) override;
  void willDisplayCellAtLocation(HighlightCell * cell, int i, int j) override;
  int typeAtLocation(int i, int j) override;

  // SelectableTableViewDelegate
  void tableViewDidChangeSelection(SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY, bool withinTemporarySelection = false) override;

  // ButtonRowDelegate
  Button * buttonAtIndex(int index, ButtonRowController::Position position) const override {
    return const_cast<Button *>(&m_setIntervalButton);
  }

  // AlternateEmptyViewDelegate
  I18n::Message emptyMessage() override;

  // Parameters controllers getters
  Shared::IntervalParameterController * intervalParameterController() override {
    return &m_intervalParameterController;
  }
  IntervalParameterSelectorController * intervalParameterSelectorController() {
    return &m_intervalParameterSelectorController;
  }
private:
  static constexpr KDCoordinate k_abscissaCellWidth = k_cellWidth + Metric::TableSeparatorThickness;
  static constexpr KDCoordinate k_parametricCellWidth = (2*Poincare::PrintFloat::glyphLengthForFloatWithPrecision(Poincare::Preferences::LargeNumberOfSignificantDigits)+3) * 7 + 2*Metric::CellMargin; // The largest cell is holding "(-1.234567E-123;-1.234567E-123)" and KDFont::SmallFont->glyphSize().width() = 7
  static constexpr int k_maxNumberOfDisplayableFunctions = 4;
  static constexpr int k_maxNumberOfDisplayableAbscissaCells = Shared::ContinuousFunction::k_numberOfPlotTypes * k_maxNumberOfDisplayableRows;
  static constexpr int k_maxNumberOfDisplayableCells = k_maxNumberOfDisplayableFunctions * k_maxNumberOfDisplayableRows;

  // Values controller
  void setStartEndMessages(Shared::IntervalParameterController * controller, int column) override;
  int maxNumberOfCells() override { return k_maxNumberOfDisplayableCells; }
  int maxNumberOfFunctions() override { return k_maxNumberOfDisplayableFunctions; }

  // Number of columns memoization
  void updateNumberOfColumns() const override;

  // Model getters
  Ion::Storage::Record recordAtColumn(int i) override;
  Ion::Storage::Record recordAtColumn(int i, bool * isDerivative);
  ContinuousFunctionStore * functionStore() const override { return static_cast<ContinuousFunctionStore *>(Shared::ValuesController::functionStore()); }
  Shared::Interval * intervalAtColumn(int columnIndex) override;

  // Number of columns
  int numberOfColumnsForAbscissaColumn(int column) override;
  int numberOfColumnsForRecord(Ion::Storage::Record record) const;
  int numberOfColumnsForPlotType(int plotTypeIndex) const;
  int numberOfAbscissaColumnsBeforeColumn(int column);
  int numberOfValuesColumns() override;
  Shared::ContinuousFunction::PlotType plotTypeAtColumn(int * i) const;

  // Function evaluation memoization
  static constexpr int k_valuesCellBufferSize = 2*Poincare::PrintFloat::charSizeForFloatsWithPrecision(Poincare::Preferences::LargeNumberOfSignificantDigits)+3; // The largest buffer holds (-1.234567E-123;-1.234567E-123)
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
  void fillMemoizedBuffer(int i, int j, int index) override;

  // Parameter controllers
  ViewController * functionParameterController() override;
  I18n::Message valuesParameterMessageAtColumn(int columnIndex) const override;
  /* The paramater i should be the column index and plotTypeAtColumn changes it
   * to be the relative column index within the sub table. */

  // Cells & View
  Shared::Hideable * hideableCellFromType(HighlightCell * cell, int type);
  Shared::BufferFunctionTitleCell * functionTitleCells(int j) override;
  EvenOddBufferTextCell * floatCells(int j) override;
  int abscissaCellsCount() const override { return k_maxNumberOfDisplayableAbscissaCells; }
  EvenOddEditableTextCell * abscissaCells(int j) override { assert (j >= 0 && j < k_maxNumberOfDisplayableAbscissaCells); return &m_abscissaCells[j]; }
  int abscissaTitleCellsCount() const override { return Shared::ContinuousFunction::k_numberOfPlotTypes; }
  EvenOddMessageTextCell * abscissaTitleCells(int j) override { assert (j >= 0 && j < abscissaTitleCellsCount()); return &m_abscissaTitleCells[j]; }
  SelectableTableView * selectableTableView() override { return &m_selectableTableView; }


  /* For parametric function, we display the evaluation with the form "(1;2)".
   * This form is not parsable so when we store it into the clipboard, we want
   * to turn it into a parsable matrix "[[1][2]]". To do so, we use a child
   * class of SelectableTableView to override the behaviour of the responder
   *  when encountering a cut/copy events. */
  class ValuesSelectableTableView : public SelectableTableView {
  public:
    ValuesSelectableTableView(ValuesController * vc) : SelectableTableView(vc, vc, vc) {}
    bool handleEvent(Ion::Events::Event event) override;
  };

  ValuesSelectableTableView m_selectableTableView;
  mutable int m_numberOfValuesColumnsForType[Shared::ContinuousFunction::k_numberOfPlotTypes];
  Shared::BufferFunctionTitleCell m_functionTitleCells[k_maxNumberOfDisplayableFunctions];
  Shared::HideableEvenOddBufferTextCell m_floatCells[k_maxNumberOfDisplayableCells];
  AbscissaTitleCell m_abscissaTitleCells[Shared::ContinuousFunction::k_numberOfPlotTypes];
  Shared::StoreCell m_abscissaCells[k_maxNumberOfDisplayableAbscissaCells];
  FunctionParameterController m_functionParameterController;
  Shared::IntervalParameterController m_intervalParameterController;
  IntervalParameterSelectorController m_intervalParameterSelectorController;
  DerivativeParameterController m_derivativeParameterController;
  Button m_setIntervalButton;
  // TODO specialize buffer size as well
  mutable char m_memoizedBuffer[k_maxNumberOfDisplayableCells][k_valuesCellBufferSize];
};

}

#endif

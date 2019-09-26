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
  Button * buttonAtIndex(int index, ButtonRowController::Position position) const override {
    return const_cast<Button *>(&m_setIntervalButton);
  }
  KDCoordinate columnWidth(int i) override;
  KDCoordinate cumulatedWidthFromIndex(int i) override;
  int indexFromCumulatedWidth(KDCoordinate offsetX) override;
  void willDisplayCellAtLocation(HighlightCell * cell, int i, int j) override;
  int typeAtLocation(int i, int j) override;
  I18n::Message emptyMessage() override;
  Shared::IntervalParameterController * intervalParameterController() override {
    return &m_intervalParameterController;
  }
  IntervalParameterSelectorController * intervalParameterSelectorController() {
    return &m_intervalParameterSelectorController;
  }
  void tableViewDidChangeSelection(SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY, bool withinTemporarySelection = false) override;
private:
  static constexpr KDCoordinate k_abscissaCellWidth = k_cellWidth + Metric::TableSeparatorThickness;
  static constexpr KDCoordinate k_parametricCellWidth = (2*Poincare::PrintFloat::glyphLengthForFloatWithPrecision(Poincare::Preferences::LargeNumberOfSignificantDigits)+3) * 7 + 2*Metric::CellMargin; // The largest cell is holding "(-1.234567E-123;-1.234567E-123)" and KDFont::SmallFont->glyphSize().width() = 7

  constexpr static int k_maxNumberOfFunctions = 5;
  constexpr static int k_maxNumberOfAbscissaCells = Shared::ContinuousFunction::k_numberOfPlotTypes * k_maxNumberOfRows;
  constexpr static int k_maxNumberOfCells = k_maxNumberOfFunctions * k_maxNumberOfRows;

  void setStartEndMessages(Shared::IntervalParameterController * controller, int column) override;
  void updateNumberOfColumns() const override;
  Ion::Storage::Record recordAtColumn(int i) override;
  Ion::Storage::Record recordAtColumn(int i, bool * isDerivative);
  int numberOfColumnsForRecord(Ion::Storage::Record record) const;
  Shared::Interval * intervalAtColumn(int columnIndex) override;
  I18n::Message valuesParameterMessageAtColumn(int columnIndex) const override;
  /* The paramater i should be the column index and plotTypeAtColumn changes it
   * to be the relative column index within the sub table. */
  Shared::ContinuousFunction::PlotType plotTypeAtColumn(int * i) const;
  int maxNumberOfCells() override;
  int maxNumberOfFunctions() override;
  Shared::Hideable * hideableCellFromType(HighlightCell * cell, int type);
  void printEvaluationOfAbscissaAtColumn(double abscissa, int columnIndex, char * buffer, const int bufferSize) override;
  ContinuousFunctionStore * functionStore() const override { return static_cast<ContinuousFunctionStore *>(Shared::ValuesController::functionStore()); }
  Shared::BufferFunctionTitleCell * functionTitleCells(int j) override;
  EvenOddBufferTextCell * floatCells(int j) override;
  int abscissaCellsCount() const override { return k_maxNumberOfAbscissaCells; }
  EvenOddEditableTextCell * abscissaCells(int j) override { assert (j >= 0 && j < k_maxNumberOfAbscissaCells); return &m_abscissaCells[j]; }
  int abscissaTitleCellsCount() const override { return Shared::ContinuousFunction::k_numberOfPlotTypes; }
  EvenOddMessageTextCell * abscissaTitleCells(int j) override { assert (j >= 0 && j < abscissaTitleCellsCount()); return &m_abscissaTitleCells[j]; }
  ViewController * functionParameterController() override;
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
  mutable int m_numberOfColumnsForType[Shared::ContinuousFunction::k_numberOfPlotTypes];
  Shared::BufferFunctionTitleCell m_functionTitleCells[k_maxNumberOfFunctions];
  Shared::HideableEvenOddBufferTextCell m_floatCells[k_maxNumberOfCells];
  AbscissaTitleCell m_abscissaTitleCells[Shared::ContinuousFunction::k_numberOfPlotTypes];
  Shared::StoreCell m_abscissaCells[k_maxNumberOfAbscissaCells];
  FunctionParameterController m_functionParameterController;
  Shared::IntervalParameterController m_intervalParameterController;
  IntervalParameterSelectorController m_intervalParameterSelectorController;
  DerivativeParameterController m_derivativeParameterController;
  Button m_setIntervalButton;
};

}

#endif

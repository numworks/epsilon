#ifndef GRAPH_VALUES_CONTROLLER_H
#define GRAPH_VALUES_CONTROLLER_H

#include "../cartesian_function_store.h"
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
  constexpr static int k_maxNumberOfFunctions = 5;
  constexpr static int k_maxNumberOfAbscissaCells = Shared::CartesianFunction::k_numberOfPlotTypes * k_maxNumberOfRows;
  constexpr static int k_maxNumberOfCells = k_maxNumberOfFunctions * k_maxNumberOfRows;

  void setStartEndMessages(Shared::IntervalParameterController * controller, int column) override;
  void updateNumberOfColumns() override;
  Ion::Storage::Record recordAtColumn(int i) override;
  Ion::Storage::Record recordAtColumn(int i, bool * isDerivative);
  int numberOfColumnsForRecord(Ion::Storage::Record record) const;
  Shared::Interval * intervalAtColumn(int columnIndex) override;
  I18n::Message valuesParameterMessageAtColumn(int columnIndex) const override;
  Shared::CartesianFunction::PlotType plotTypeAtColumn(int * i) const;
  int maxNumberOfCells() override;
  int maxNumberOfFunctions() override;
  double evaluationOfAbscissaAtColumn(double abscissa, int columnIndex) override;
  CartesianFunctionStore * functionStore() const override { return static_cast<CartesianFunctionStore *>(Shared::ValuesController::functionStore()); }
  Shared::BufferFunctionTitleCell * functionTitleCells(int j) override;
  EvenOddBufferTextCell * floatCells(int j) override;
  int abscissaCellsCount() const override { return k_maxNumberOfAbscissaCells; }
  EvenOddEditableTextCell * abscissaCells(int j) override { assert (j >= 0 && j < k_maxNumberOfAbscissaCells); return &m_abscissaCells[j]; }
  int abscissaTitleCellsCount() const override { return Shared::CartesianFunction::k_numberOfPlotTypes; }
  EvenOddMessageTextCell * abscissaTitleCells(int j) override { assert (j >= 0 && j < abscissaTitleCellsCount()); return &m_abscissaTitleCells[j]; }
  ViewController * functionParameterController() override;

  int m_numberOfColumnsForType[Shared::CartesianFunction::k_numberOfPlotTypes];
  Shared::BufferFunctionTitleCell m_functionTitleCells[k_maxNumberOfFunctions];
  Shared::HideableEvenOddBufferTextCell m_floatCells[k_maxNumberOfCells];
  AbscissaTitleCell m_abscissaTitleCells[Shared::CartesianFunction::k_numberOfPlotTypes];
  Shared::StoreCell m_abscissaCells[k_maxNumberOfAbscissaCells];
  FunctionParameterController m_functionParameterController;
  Shared::IntervalParameterController m_intervalParameterController;
  IntervalParameterSelectorController m_intervalParameterSelectorController;
  DerivativeParameterController m_derivativeParameterController;
  Button m_setIntervalButton;
};

}

#endif

#ifndef GRAPH_VALUES_CONTROLLER_H
#define GRAPH_VALUES_CONTROLLER_H

#include "../cartesian_function_store.h"
#include "../../shared/buffer_function_title_cell.h"
#include "../../shared/values_controller.h"
#include "../../shared/interval_parameter_controller.h"
#include "interval_parameter_selector_controller.h"
#include "derivative_parameter_controller.h"
#include "function_parameter_controller.h"

namespace Graph {

class ValuesController : public Shared::ValuesController {
public:
  ValuesController(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate, Shared::Interval * interval, ButtonRowController * header);
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
private:
  constexpr static int k_maxNumberOfCells = 50;
  constexpr static int k_maxNumberOfFunctions = 5;
  void updateNumberOfColumns() override;
  Ion::Storage::Record recordAtColumn(int i) override;
  Ion::Storage::Record recordAtColumn(int i, bool * isDerivative);
  int numberOfColumnsForRecord(Ion::Storage::Record record) const;
  int maxNumberOfCells() override;
  int maxNumberOfFunctions() override;
  double evaluationOfAbscissaAtColumn(double abscissa, int columnIndex) override;
  CartesianFunctionStore * functionStore() const override { return static_cast<CartesianFunctionStore *>(Shared::ValuesController::functionStore()); }
  Shared::BufferFunctionTitleCell * functionTitleCells(int j) override;
  EvenOddBufferTextCell * floatCells(int j) override;
  ViewController * functionParameterController() override;
  I18n::Message valuesParameterControllerPageTitle() const override;

  int m_numberOfColumnsForType[Shared::CartesianFunction::k_numberOfPlotTypes];
  Shared::BufferFunctionTitleCell m_functionTitleCells[k_maxNumberOfFunctions];
  EvenOddBufferTextCell m_floatCells[k_maxNumberOfCells];
  FunctionParameterController m_functionParameterController;
  Shared::IntervalParameterController m_intervalParameterController;
  IntervalParameterSelectorController m_intervalParameterSelectorController;
  DerivativeParameterController m_derivativeParameterController;
  Button m_setIntervalButton;
};

}

#endif

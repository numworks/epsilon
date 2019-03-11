#ifndef GRAPH_VALUES_CONTROLLER_H
#define GRAPH_VALUES_CONTROLLER_H

#include "../storage_cartesian_function_store.h"
#include "../../shared/buffer_function_title_cell.h"
#include "../../shared/storage_values_controller.h"
#include "../../shared/interval_parameter_controller.h"
#include "storage_derivative_parameter_controller.h"
#include "storage_function_parameter_controller.h"

namespace Graph {

class StorageValuesController : public Shared::StorageValuesController {
public:
  StorageValuesController(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate, Shared::Interval * interval, ButtonRowController * header);
  bool handleEvent(Ion::Events::Event event) override;
  void willDisplayCellAtLocation(HighlightCell * cell, int i, int j) override;
  I18n::Message emptyMessage() override;
  Shared::IntervalParameterController * intervalParameterController() override;
  void updateNumberOfColumns() override;
private:
  constexpr static int k_maxNumberOfCells = 50;
  constexpr static int k_maxNumberOfFunctions = 5;
  Ion::Storage::Record recordAtColumn(int i) override;
  bool isDerivativeColumn(int i);
  void configureDerivativeFunction();
  int maxNumberOfCells() override;
  int maxNumberOfFunctions() override;
  double evaluationOfAbscissaAtColumn(double abscissa, int columnIndex) override;
  StorageCartesianFunctionStore * functionStore() const override { return static_cast<StorageCartesianFunctionStore *>(Shared::StorageValuesController::functionStore()); }
  Shared::BufferFunctionTitleCell * functionTitleCells(int j) override;
  EvenOddBufferTextCell * floatCells(int j) override;
  FunctionParameterController * functionParameterController() override;

  Shared::BufferFunctionTitleCell m_functionTitleCells[k_maxNumberOfFunctions];
  EvenOddBufferTextCell m_floatCells[k_maxNumberOfCells];
  FunctionParameterController m_functionParameterController;
  Shared::IntervalParameterController m_intervalParameterController;
  StorageDerivativeParameterController m_derivativeParameterController;
};

}

#endif

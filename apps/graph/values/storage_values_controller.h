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
  StorageValuesController(Responder * parentResponder, StorageCartesianFunctionStore * functionStore, Shared::Interval * interval, ButtonRowController * header);
  bool handleEvent(Ion::Events::Event event) override;
  void willDisplayCellAtLocation(HighlightCell * cell, int i, int j) override;
  I18n::Message emptyMessage() override;
  Shared::IntervalParameterController * intervalParameterController() override;
  void updateNumberOfColumns() override;
private:
  constexpr static int k_maxNumberOfCells = 50;
  constexpr static int k_maxNumberOfFunctions = 5;
  Shared::StorageCartesianFunction * functionAtColumn(int i) override;
  bool isDerivativeColumn(int i);
  void configureDerivativeFunction();
  int maxNumberOfCells() override;
  int maxNumberOfFunctions() override;
  double evaluationOfAbscissaAtColumn(double abscissa, int columnIndex) override;
  Shared::BufferFunctionTitleCell * functionTitleCells(int j) override;
  EvenOddBufferTextCell * floatCells(int j) override;
  StorageCartesianFunctionStore * functionStore() const override;
  StorageFunctionParameterController * functionParameterController() override;

  Shared::BufferFunctionTitleCell m_functionTitleCells[k_maxNumberOfFunctions];
  EvenOddBufferTextCell m_floatCells[k_maxNumberOfCells];
  StorageCartesianFunctionStore * m_functionStore;
  StorageFunctionParameterController m_functionParameterController;
  Shared::IntervalParameterController m_intervalParameterController;
  StorageDerivativeParameterController m_derivativeParameterController;
};

}

#endif

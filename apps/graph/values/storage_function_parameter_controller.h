#ifndef GRAPH_STORAGE_FUNCTION_PARAM_CONTROLLER_H
#define GRAPH_STORAGE_FUNCTION_PARAM_CONTROLLER_H

#include "../../shared/expiring_pointer.h"
#include "../../shared/storage_cartesian_function.h"
#include "../../shared/storage_values_function_parameter_controller.h"

namespace Graph {

class StorageValuesController;

class StorageFunctionParameterController : public Shared::StorageValuesFunctionParameterController {
public:
  StorageFunctionParameterController(StorageValuesController * valuesController);
  bool handleEvent(Ion::Events::Event event) override;
  int numberOfRows() override;
  HighlightCell * reusableCell(int index) override;
  int reusableCellCount() override;
  void willDisplayCellForIndex(HighlightCell * cell, int index) override;
  void viewWillAppear() override;
private:
  Shared::ExpiringPointer<Shared::StorageCartesianFunction> function();
#if COPY_COLUMN
  constexpr static int k_totalNumberOfCell = 2;
#else
  constexpr static int k_totalNumberOfCell = 1;
#endif
  MessageTableCellWithSwitch m_displayDerivativeColumn;
  StorageValuesController * m_valuesController;
};

}

#endif

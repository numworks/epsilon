#ifndef GRAPH_FUNCTION_PARAM_CONTROLLER_H
#define GRAPH_FUNCTION_PARAM_CONTROLLER_H

#include "../../shared/expiring_pointer.h"
#include "../../shared/continuous_function.h"
#include "../../shared/values_function_parameter_controller.h"

namespace Graph {

class ValuesController;

class FunctionParameterController : public Shared::ValuesFunctionParameterController {
public:
  FunctionParameterController(ValuesController * valuesController);
  bool handleEvent(Ion::Events::Event event) override;
  int numberOfRows() const override;
  HighlightCell * reusableCell(int index) override;
  int reusableCellCount() const override;
  void willDisplayCellForIndex(HighlightCell * cell, int index) override;
  void viewWillAppear() override;
private:
  Shared::ExpiringPointer<Shared::ContinuousFunction> function();
#if COPY_COLUMN
  constexpr static int k_totalNumberOfCell = 2;
#else
  constexpr static int k_totalNumberOfCell = 1;
#endif
  MessageTableCellWithSwitch m_displayDerivativeColumn;
  ValuesController * m_valuesController;
  // Index of the column corresponding to the function in the values controller
  int m_selectedFunctionColumn;
};

}

#endif

#ifndef GRAPH_FUNCTION_PARAM_CONTROLLER_H
#define GRAPH_FUNCTION_PARAM_CONTROLLER_H

#include "../cartesian_function.h"
#include "../../shared/values_function_parameter_controller.h"

namespace Graph {

class ValuesController;

class FunctionParameterController : public Shared::ValuesFunctionParameterController {
public:
  FunctionParameterController(ValuesController * valuesController);
  bool handleEvent(Ion::Events::Event event) override;
  int numberOfRows() override;
  HighlightCell * reusableCell(int index) override;
  int reusableCellCount() override;
  void willDisplayCellForIndex(HighlightCell * cell, int index) override;
  void setFunction(Shared::Function * function) override;
  void viewWillAppear() override;
private:
#if COPY_COLUMN
  constexpr static int k_totalNumberOfCell = 2;
#else
  constexpr static int k_totalNumberOfCell = 1;
#endif
  MessageTableCellWithSwitch m_displayDerivativeColumn;
  CartesianFunction * m_cartesianFunction;
  ValuesController * m_valuesController;
};

}

#endif

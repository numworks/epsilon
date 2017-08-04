#ifndef GRAPH_VALUES_CONTROLLER_H
#define GRAPH_VALUES_CONTROLLER_H

#include "../cartesian_function_store.h"
#include "../function_title_cell.h"
#include "../../shared/values_controller.h"
#include "../../shared/interval_parameter_controller.h"
#include "derivative_parameter_controller.h"
#include "function_parameter_controller.h"

namespace Graph {

class ValuesController : public Shared::ValuesController {
public:
  ValuesController(Responder * parentResponder, CartesianFunctionStore * functionStore, Shared::Interval * interval, ButtonRowController * header);
  bool handleEvent(Ion::Events::Event event) override;
  void willDisplayCellAtLocation(HighlightCell * cell, int i, int j) override;
  I18n::Message emptyMessage() override;
  Shared::IntervalParameterController * intervalParameterController() override;
  void updateNumberOfColumns() override;
private:
  CartesianFunction * functionAtColumn(int i) override;
  bool isDerivativeColumn(int i);
  void configureDerivativeFunction();
  int maxNumberOfCells() override;
  int maxNumberOfFunctions() override;
  double evaluationOfAbscissaAtColumn(double abscissa, int columnIndex) override;
  constexpr static int k_maxNumberOfCells = 50;
  constexpr static int k_maxNumberOfFunctions = 5;
  FunctionTitleCell * m_functionTitleCells[k_maxNumberOfFunctions];
  FunctionTitleCell * functionTitleCells(int j) override;
  EvenOddBufferTextCell * m_floatCells[k_maxNumberOfCells];
  EvenOddBufferTextCell * floatCells(int j) override;
  CartesianFunctionStore * m_functionStore;
  CartesianFunctionStore * functionStore() const override;
  FunctionParameterController m_functionParameterController;
  FunctionParameterController * functionParameterController() override;
  View * loadView() override;
  void unloadView(View * view) override;
  Shared::IntervalParameterController m_intervalParameterController;
  DerivativeParameterController m_derivativeParameterController;
};

}

#endif

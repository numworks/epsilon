#ifndef GRAPH_LIST_CONTROLLER_H
#define GRAPH_LIST_CONTROLLER_H

#include <escher.h>
#include "../function_title_cell.h"
#include "../../shared/function_expression_cell.h"
#include "../cartesian_function_store.h"
#include "../../shared/new_function_cell.h"
#include "../../shared/list_controller.h"
#include "../../shared/list_parameter_controller.h"

namespace Graph {

class ListController : public Shared::ListController {
public:
  ListController(Responder * parentResponder, CartesianFunctionStore * functionStore, ButtonRowController * header, ButtonRowController * footer);
  const char * title() override;
  int numberOfRows() override;
  KDCoordinate rowHeight(int j) override;
private:
  void editExpression(Shared::Function * function, Ion::Events::Event event) override;
  Shared::ListParameterController * parameterController() override;
  int maxNumberOfRows() override;
  HighlightCell * titleCells(int index) override;
  HighlightCell * expressionCells(int index) override;
  void willDisplayTitleCellAtIndex(HighlightCell * cell, int j) override;
  void willDisplayExpressionCellAtIndex(HighlightCell * cell, int j) override;
  bool removeFunctionRow(Shared::Function * function) override;
  View * loadView() override;
  void unloadView(View * view) override;
  constexpr static int k_maxNumberOfRows = 5;
  FunctionTitleCell * m_functionTitleCells[k_maxNumberOfRows];
  Shared::FunctionExpressionCell * m_expressionCells[k_maxNumberOfRows];
  Shared::ListParameterController m_parameterController;
};

}

#endif

#ifndef GRAPH_LIST_CONTROLLER_H
#define GRAPH_LIST_CONTROLLER_H

#include <escher.h>
#include "../function_title_cell.h"
#include "function_expression_cell.h"
#include "../cartesian_function_store.h"
#include "../../shared/new_function_cell.h"
#include "../../shared/list_controller.h"
#include "../../shared/list_parameter_controller.h"

namespace Graph {

class ListController : public Shared::ListController {
public:
  ListController(Responder * parentResponder, CartesianFunctionStore * functionStore, HeaderViewController * header);
  const char * title() const override;
  bool handleEvent(Ion::Events::Event event) override;
private:
  bool handleEnter();
  void editExpression(FunctionExpressionCell * functionCell, Ion::Events::Event event);
  Shared::ListParameterController * parameterController() override;
  int maxNumberOfRows() override;
  TableViewCell * titleCells(int index) override;
  TableViewCell * expressionCells(int index) override;
  void configureFunction(Shared::Function * function);
  void willDisplayTitleCellAtIndex(TableViewCell * cell, int j) override;
  void willDisplayExpressionCellAtIndex(TableViewCell * cell, int j) override;
  constexpr static int k_maxNumberOfRows = 5;
  FunctionTitleCell m_functionTitleCells[k_maxNumberOfRows];
  FunctionExpressionCell m_expressionCells[k_maxNumberOfRows];
  Shared::ListParameterController m_parameterController;
};

}

#endif

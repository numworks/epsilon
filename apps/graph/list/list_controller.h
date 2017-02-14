#ifndef GRAPH_LIST_CONTROLLER_H
#define GRAPH_LIST_CONTROLLER_H

#include <escher.h>
#include "../../shared/function_title_cell.h"
#include "../../shared/function_expression_cell.h"
#include "../../shared/new_function_cell.h"
#include "../../shared/list_controller.h"
#include "../../shared/list_parameter_controller.h"

namespace Graph {

class ListController : public Shared::ListController {
public:
  ListController(Responder * parentResponder, Shared::FunctionStore * functionStore, HeaderViewController * header);
  const char * title() const override;
  bool handleEvent(Ion::Events::Event event) override;
private:
  bool handleEnter();
  void editExpression(Shared::FunctionExpressionCell * functionCell, Ion::Events::Event event);
  Shared::ListParameterController * parameterController() override;
  int maxNumberOfRows() override;
  TableViewCell * titleCells(int index) override;
  TableViewCell * expressionCells(int index) override;
  constexpr static int k_maxNumberOfRows = 5;
  Shared::FunctionTitleCell m_functionTitleCells[k_maxNumberOfRows];
  Shared::FunctionExpressionCell m_expressionCells[k_maxNumberOfRows];
  Shared::ListParameterController m_parameterController;
};

}

#endif

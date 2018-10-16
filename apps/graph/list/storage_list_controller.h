#ifndef GRAPH_STORAGE_LIST_CONTROLLER_H
#define GRAPH_STORAGE_LIST_CONTROLLER_H

#include <escher.h>
#include "list_parameter_controller.h"
#include "text_field_function_title_cell.h"
#include "../storage_cartesian_function_store.h"
#include <apps/shared/storage_function_list_controller.h>
#include <apps/shared/buffer_function_title_cell.h>
#include <apps/shared/function_expression_cell.h>

namespace Graph {

class StorageListController : public Shared::StorageFunctionListController {
public:
  StorageListController(Responder * parentResponder, StorageCartesianFunctionStore * functionStore, ButtonRowController * header, ButtonRowController * footer);
  const char * title() override;
private:
  Shared::StorageListParameterController * parameterController() override;
  int maxNumberOfDisplayableRows() override;
  Shared::FunctionTitleCell * titleCells(int index) override;
  HighlightCell * expressionCells(int index) override;
  void willDisplayTitleCellAtIndex(HighlightCell * cell, int j) override;
  void willDisplayExpressionCellAtIndex(HighlightCell * cell, int j) override;
  constexpr static int k_maxNumberOfDisplayableRows = 5;
  TextFieldFunctionTitleCell m_functionTitleCells[k_maxNumberOfDisplayableRows];
  Shared::FunctionExpressionCell m_expressionCells[k_maxNumberOfDisplayableRows];
  ListParameterController m_parameterController;
};

}

#endif

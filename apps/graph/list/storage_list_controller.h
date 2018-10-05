#ifndef GRAPH_STORAGE_LIST_CONTROLLER_H
#define GRAPH_STORAGE_LIST_CONTROLLER_H

#include <escher.h>
#include "../../shared/storage_function_list_controller.h"
#include "../storage_cartesian_function_store.h"
#include "../../shared/buffer_function_title_cell.h"
#include "../../shared/function_expression_cell.h"
#include <apps/shared/storage_list_parameter_controller.h>

namespace Graph {

class StorageListController : public Shared::StorageFunctionListController<StorageCartesianFunction> {
public:
  StorageListController(Responder * parentResponder, StorageCartesianFunctionStore * functionStore, ButtonRowController * header, ButtonRowController * footer);
  const char * title() override;
private:
  Shared::StorageListParameterController<StorageCartesianFunction> * parameterController() override;
  int maxNumberOfRows() override;
  Shared::FunctionTitleCell * titleCells(int index) override;
  HighlightCell * expressionCells(int index) override;
  void willDisplayTitleCellAtIndex(HighlightCell * cell, int j) override;
  void willDisplayExpressionCellAtIndex(HighlightCell * cell, int j) override;
  bool removeModelRow(StorageCartesianFunction * function) override;
  constexpr static int k_maxNumberOfRows = 5;
  Shared::BufferFunctionTitleCell m_functionTitleCells[k_maxNumberOfRows];
  Shared::FunctionExpressionCell m_expressionCells[k_maxNumberOfRows];
  Shared::StorageListParameterController<StorageCartesianFunction> m_parameterController;
};

}

#endif

#ifndef GRAPH_LIST_CONTROLLER_H
#define GRAPH_LIST_CONTROLLER_H

#include "function_toolbox.h"
#include "../shared/function_parameter_controller.h"
#include <apps/shared/continuous_function_store.h>
#include <apps/shared/function_list_controller.h>
#include "function_models_parameter_controller.h"
#include "../graph/graph_controller.h"
#include "function_cell.h"
#include "editable_function_cell.h"

namespace Graph {

class ListController : public Shared::FunctionListController,  public Escher::MemoizedListViewDataSource {
public:
  ListController(Escher::Responder * parentResponder, Escher::ButtonRowController * header, Escher::ButtonRowController * footer, GraphController * graphController, FunctionParameterController * functionParameterController);
  // ListViewDataSource
  int numberOfRows() const override { return this->numberOfExpressionRows(); }
  int typeAtIndex(int index) const override;
  Escher::HighlightCell * reusableCell(int index, int type) override;
  int reusableCellCount(int type) override;
  // ViewController
  const char * title() override;
  Escher::View * view() override { return &m_selectableTableView; }
  void viewWillAppear() override;
  // LayoutFieldDelegate
  bool layoutFieldDidReceiveEvent(Escher::LayoutField * layoutField, Ion::Events::Event event) override;
  bool layoutFieldDidFinishEditing(Escher::LayoutField * layoutField, Poincare::Layout layout, Ion::Events::Event event) override;
  void layoutFieldDidChangeSize(Escher::LayoutField * layoutField) override;
  bool layoutFieldDidAbortEditing(Escher::LayoutField * layoutField) override;
  // Responder
  bool handleEvent(Ion::Events::Event event) override;
  // ExpressionModelListController
  void editExpression(Ion::Events::Event event) override;
  KDCoordinate expressionRowHeight(int j) override;
  Escher::SelectableTableView * selectableTableView() override { return &m_selectableTableView; }
  FunctionToolbox * toolbox() override;
  Shared::ListParameterController * parameterController() override;
  // SelectableTableViewDelegate
  bool canStoreContentOfCellAtLocation(Escher::SelectableTableView * t, int col, int row) const override { return false; }
private:
  constexpr static int k_functionCellType = 0;
  constexpr static int k_addNewModelType = 1;
  constexpr static int k_editableCellType = 2;
  // 6 rows of undefined empty functions
  constexpr static int k_maxNumberOfDisplayableRows = 6;

  // Responder
  int initialColumnToSelect() const override { return 0; }

  // ExpressionModelListController
  void resetSizesMemoization() override { resetMemoization(); }
  // ListViewDataSource
  KDCoordinate nonMemoizedRowHeight(int j) override { return expressionRowHeight(j); }

  void fillWithDefaultFunctionEquation(char * buffer, size_t bufferSize, FunctionModelsParameterController * modelsParameterController, CodePoint Symbol) const;
  bool layoutRepresentsPolarFunction(Poincare::Layout l) const;
  bool layoutRepresentsParametricFunction(Poincare::Layout l) const;
  // Complete the equationField with a valid left equation side
  bool completeEquation(Escher::InputEventHandler * equationField, CodePoint symbol);
  void addModel() override;
  int maxNumberOfDisplayableRows() override;
  Escher::HighlightCell * functionCells(int index) override;
  void willDisplayCellForIndex(Escher::HighlightCell * cell, int j) override;
  Shared::ContinuousFunctionStore * modelStore() const override;
  Escher::SelectableTableView m_selectableTableView;
  FunctionCell m_expressionCells[k_maxNumberOfDisplayableRows];
  EditableFunctionCell m_editableCell;
  FunctionParameterController * m_parameterController;
  FunctionModelsParameterController m_modelsParameterController;
  Escher::StackViewController m_modelsStackController;
  FunctionToolbox m_functionToolbox;
  int m_editedCellIndex;
  bool m_parameterColumnSelected;
};

}

#endif

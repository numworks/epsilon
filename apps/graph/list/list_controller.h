#ifndef GRAPH_LIST_CONTROLLER_H
#define GRAPH_LIST_CONTROLLER_H

#include <apps/shared/continuous_function_store.h>
#include <apps/shared/function_list_controller.h>
#include <apps/shared/math_toolbox_controller.h>

#include "../graph/graph_controller.h"
#include "../shared/function_parameter_controller.h"
#include "editable_function_cell.h"
#include "function_cell.h"
#include "function_models_parameter_controller.h"

namespace Graph {

class ListController : public Shared::FunctionListController,
                       public Shared::MathToolboxExtraCellsDataSource {
 public:
  ListController(Escher::Responder* parentResponder,
                 Escher::ButtonRowController* header,
                 Escher::ButtonRowController* footer,
                 FunctionParameterController* functionParameterController);
  Escher::HighlightCell* reusableCell(int index, int type) override;
  int reusableCellCount(int type) override;
  // ViewController
  void viewWillAppear() override;
  void viewDidDisappear() override;
  // MathLayoutFieldDelegate
  bool layoutFieldDidReceiveEvent(Escher::LayoutField* layoutField,
                                  Ion::Events::Event event) override;
  CodePoint defaultXNT() override;
  // Responder
  bool handleEvent(Ion::Events::Event event) override;
  // ExpressionModelListController
  void editExpression(Ion::Events::Event event) override;
  KDCoordinate expressionRowHeight(int row) override;
  KDCoordinate editableRowHeight() override;
  Shared::ListParameterController* parameterController() override;
  bool canStoreCellAtRow(int row) override { return false; }

  // MathToolboxExtraCellsDataSource
  int numberOfExtraCells() override { return k_numberOfToolboxExtraCells; }
  Poincare::Layout extraCellLayoutAtRow(int row) override;

 private:
  // 6 rows of undefined empty functions
  constexpr static int k_maxNumberOfDisplayableRows = 6;
  constexpr static int k_numberOfToolboxExtraCells = 2;

  Escher::EditableExpressionModelCell* editableExpressionModelCell() override {
    return m_editableCell.expressionCell();
  }
  void fillWithDefaultFunctionEquation(
      char* buffer, size_t bufferSize,
      FunctionModelsParameterController* modelsParameterController,
      CodePoint Symbol) const;
  bool layoutRepresentsPolarFunction(Poincare::Layout l) const;
  bool layoutRepresentsParametricFunction(Poincare::Layout l) const;
  bool shouldCompleteEquation(Poincare::Expression expression) override;
  bool completeEquation(Escher::LayoutField* equationField) override;
  void addModel() override;
  int maxNumberOfDisplayableRows() override;
  Escher::HighlightCell* functionCells(int index) override;
  void fillCellForRow(Escher::HighlightCell* cell, int row) override;
  Shared::ContinuousFunctionStore* modelStore() const override;
  Escher::LayoutField* layoutField() override {
    return m_editableCell.expressionCell()->layoutField();
  }

  FunctionCell m_expressionCells[k_maxNumberOfDisplayableRows];
  EditableFunctionCell m_editableCell;
  FunctionParameterController* m_parameterController;
  FunctionModelsParameterController m_modelsParameterController;
  Escher::StackViewController m_modelsStackController;
  bool m_parameterColumnSelected;
};

}  // namespace Graph

#endif

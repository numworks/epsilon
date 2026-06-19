#pragma once

#include <apps/shared/continuous_function_store.h>
#include <apps/shared/function_list_controller.h>
#include <apps/shared/math_toolbox_controller.h>

#include "../graph/graph_controller.h"
#include "../shared/derivative_column_parameter_controller.h"
#include "../shared/function_parameter_controller.h"
#include "editable_function_cell.h"
#include "function_cell.h"
#include "function_models_parameter_controller.h"

namespace Graph {

class ListController : public Shared::FunctionListController,
                       public Shared::MathToolboxExtraCellsDataSource,
                       public ParameterDelegate {
 public:
  ListController(
      Escher::Responder* parentResponder, Escher::ButtonRowController* header,
      Escher::ButtonRowController* footer,
      FunctionParameterController* functionParameterController,
      DerivativeColumnParameterController* derivativeColumnParameterController);

  Escher::HighlightCell* reusableCell(int index, int type) override;
  // ViewController
  void viewWillAppear() override;
  void viewDidDisappear() override;
  // MathLayoutFieldDelegate
  void layoutFieldDidHandleEvent(Escher::LayoutField* layoutField) override;
  bool layoutFieldDidReceiveEvent(Escher::LayoutField* layoutField,
                                  Ion::Events::Event event) override;
  void layoutFieldDidAbortEditing(Escher::LayoutField* layoutField) override;
  CodePoint defaultXNT() override;
  // Responder
  bool handleEvent(Ion::Events::Event event) override;
  // ExpressionModelListController
  void editExpression(Ion::Events::Event event) override;
  bool setSelectedRecordContent(const Poincare::Layout& layout) override;
  KDCoordinate expressionRowHeight(int row) override;
  KDCoordinate editableRowHeight() override;
  Shared::ListParameterController* parameterController() override;
  bool isRecordShownInList(Ion::Storage::Record record) const override;

  // MathToolboxExtraCellsDataSource
  int numberOfExtraCells() const override {
    return k_numberOfToolboxExtraCells;
  }
  Poincare::Layout extraCellLayoutAtRow(int row) override;

  // ParameterDelegate
  DerivativeType type() const override { return DerivativeType::Plot; }
  void hideDerivative(Ion::Storage::Record record) override;

 private:
  // 6 rows of undefined empty functions
  constexpr static int k_maxNumberOfDisplayableRows = 6;
  constexpr static int k_numberOfToolboxExtraCells = 2;

  Escher::EditableExpressionModelCell* editableExpressionModelCell() override {
    return m_editableCell.expressionCell();
  }
  void fillWithDefaultFunctionEquation(char* buffer, size_t bufferSize,
                                       CodePoint Symbol) const;
  bool shouldCompleteEquation(const Poincare::UserExpression& expression,
                              CodePoint symbol) override;
  bool completeEquation(Escher::LayoutField* equationField,
                        CodePoint symbol) override;
  int maxNumberOfDisplayableRows() const override;
  void addNewModelAction() override;
  void fillCellForRow(Escher::HighlightCell* cell, int row) override;
  Shared::ContinuousFunctionStore* modelStore() const override;
  Escher::LayoutField* layoutField() override {
    return m_editableCell.expressionCell()->layoutField();
  }
  bool isValidExpressionModel(
      const Poincare::UserExpression& expression) override;
  OMG::HorizontalDirection parameterColumnPosition() const override {
    return OMG::Direction::Right();
  }

  FunctionCell m_expressionCells[k_maxNumberOfDisplayableRows];
  EditableFunctionCell m_editableCell;
  FunctionParameterController* m_functionParameterController;
  DerivativeColumnParameterController* m_derivativeColumnParameterController;
  FunctionModelsParameterController m_modelsParameterController;
  Escher::StackViewController::Default m_modelsStackController;
};

}  // namespace Graph

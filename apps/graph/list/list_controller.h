#ifndef GRAPH_LIST_CONTROLLER_H
#define GRAPH_LIST_CONTROLLER_H

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
  bool editSelectedRecordWithText(const char* text) override;
  KDCoordinate expressionRowHeight(int row) override;
  KDCoordinate editableRowHeight() override;
  Shared::ListParameterController* parameterController() override;
  bool canStoreCellAtRow(int row) override { return false; }
  bool removeModelRow(Ion::Storage::Record record) override;

  // MathToolboxExtraCellsDataSource
  int numberOfExtraCells() override { return k_numberOfToolboxExtraCells; }
  Poincare::Layout extraCellLayoutAtRow(int row) override;

  // ParameterDelegate
  bool usePersonalizedTitle() const override { return false; }
  bool showColorCell() const override { return true; }
  I18n::Message hideDerivativeMessage() const override {
    return I18n::Message::HideDerivative;
  }
  void hideDerivative(Ion::Storage::Record record,
                      int derivationOrder) override;

 private:
  // 6 rows of undefined empty functions
  constexpr static int k_maxNumberOfDisplayableRows = 6;
  constexpr static int k_numberOfToolboxExtraCells = 2;

  Escher::EditableExpressionModelCell* editableExpressionModelCell() override {
    return m_editableCell.expressionCell();
  }
  void fillWithDefaultFunctionEquation(char* buffer, size_t bufferSize,
                                       CodePoint Symbol) const;
  bool shouldCompleteEquation(Poincare::UserExpression expression,
                              CodePoint symbol) override;
  bool completeEquation(Escher::LayoutField* equationField,
                        CodePoint symbol) override;
  void addNewModelAction() override;
  int maxNumberOfDisplayableRows() const override;
  Escher::HighlightCell* functionCells(int index) override;
  void fillCellForRow(Escher::HighlightCell* cell, int row) override;
  Shared::ContinuousFunctionStore* modelStore() const override;
  Escher::LayoutField* layoutField() override {
    return m_editableCell.expressionCell()->layoutField();
  }
  bool isValidExpressionModel(Poincare::UserExpression expression) override;
  OMG::HorizontalDirection parameterColumnPosition() const override {
    return OMG::Direction::Right();
  }
  int numberOfRowsForRecord(Ion::Storage::Record record) const override;
  int derivationOrderFromRelativeRow(Shared::ContinuousFunction* f,
                                     int relativeRow) const;

  FunctionCell m_expressionCells[k_maxNumberOfDisplayableRows];
  EditableFunctionCell m_editableCell;
  FunctionParameterController* m_functionParameterController;
  DerivativeColumnParameterController* m_derivativeColumnParameterController;
  FunctionModelsParameterController m_modelsParameterController;
  Escher::StackViewController::Default m_modelsStackController;
};

}  // namespace Graph

#endif

#ifndef GRAPH_LIST_CONTROLLER_H
#define GRAPH_LIST_CONTROLLER_H

#include "list_parameter_controller.h"
#include "text_field_function_title_cell.h"
#include "../continuous_function_store.h"
#include <apps/shared/function_expression_cell.h>
#include <apps/shared/function_list_controller.h>
#include <apps/shared/text_field_delegate.h>
#include <apps/shared/layout_field_delegate.h>
#include "../function_models_parameter_controller.h"
#include <escher/even_odd_cell_with_ellipsis.h>

namespace Graph {

class ListController : public Shared::FunctionListController, public Shared::TextFieldDelegate, public Shared::LayoutFieldDelegate {
public:
  ListController(Escher::Responder * parentResponder, Escher::ButtonRowController * header, Escher::ButtonRowController * footer, Escher::InputEventHandlerDelegate * inputEventHandlerDelegate);
  const char * title() override;
  void renameSelectedFunction();
  // TextFieldDelegate
  bool textFieldDidFinishEditing(Escher::TextField * textField, const char * text, Ion::Events::Event event) override;
  bool textFieldDidAbortEditing(Escher::TextField * textField) override;
  bool textFieldShouldFinishEditing(Escher::TextField * textField, Ion::Events::Event event) override;
  bool textFieldDidReceiveEvent(Escher::TextField * textField, Ion::Events::Event event) override;
  bool layoutFieldDidReceiveEvent(Escher::LayoutField * layoutField, Ion::Events::Event event) override;
  bool layoutFieldDidFinishEditing(Escher::LayoutField * layoutField, Poincare::Layout layout, Ion::Events::Event event) override;
  // Make methods public
  void editExpression(Ion::Events::Event event) override { return Shared::FunctionListController::editExpression(event); }
  bool editSelectedRecordWithText(const char * text) override { return Shared::FunctionListController::editSelectedRecordWithText(text); }
  // Cannot select firs column (TODO Hugo)
  void selectColumn(int j) override { if (j != 0) { SelectableTableViewDataSource::selectColumn(j); } }
private:
  constexpr static int k_maxNumberOfDisplayableRows = 5;
  void addModel() override;
  Shared::ListParameterController * parameterController() override;
  int maxNumberOfDisplayableRows() override;
  // TODO Hugo : use new type of expression cell with left accessory for color, and giveAccessoryAllWidth with some tweaks
  Shared::FunctionTitleCell * titleCells(int index) override;
  Escher::HighlightCell * expressionCells(int index) override;
  Escher::HighlightCell * parameterCells(int index) override;
  void willDisplayTitleCellAtIndex(Escher::HighlightCell * cell, int j) override;
  void willDisplayExpressionCellAtIndex(Escher::HighlightCell * cell, int j) override;
  void willDisplayParameterCellAtIndex(Escher::HighlightCell * cell, int j) override;
  void setFunctionNameInTextField(Shared::ExpiringPointer<NewFunction> function, Escher::TextField * textField);
  ContinuousFunctionStore * modelStore() override;
  TextFieldFunctionTitleCell m_functionTitleCells[k_maxNumberOfDisplayableRows];
  Shared::FunctionExpressionCell m_expressionCells[k_maxNumberOfDisplayableRows];
  Escher::EvenOddCellWithEllipsis m_expressionParameterCells[k_maxNumberOfDisplayableRows];
  ListParameterController m_parameterController;
  FunctionModelsParameterController m_modelsParameterController;
  Escher::StackViewController m_modelsStackController;
};

}

#endif

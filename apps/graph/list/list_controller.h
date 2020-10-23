#ifndef GRAPH_LIST_CONTROLLER_H
#define GRAPH_LIST_CONTROLLER_H

#include "list_parameter_controller.h"
#include "text_field_function_title_cell.h"
#include "../continuous_function_store.h"
#include <apps/shared/function_expression_cell.h>
#include <apps/shared/function_list_controller.h>
#include <apps/shared/text_field_delegate.h>

namespace Graph {

class ListController : public Shared::FunctionListController, public Shared::TextFieldDelegate {
public:
  ListController(Escher::Responder * parentResponder, Escher::ButtonRowController * header, Escher::ButtonRowController * footer, Escher::InputEventHandlerDelegate * inputEventHandlerDelegate);
  const char * title() override;
  void renameSelectedFunction();
  // TextFieldDelegate
  bool textFieldDidFinishEditing(Escher::TextField * textField, const char * text, Ion::Events::Event event) override;
  bool textFieldDidAbortEditing(Escher::TextField * textField) override;
  bool textFieldShouldFinishEditing(Escher::TextField * textField, Ion::Events::Event event) override;
  bool textFieldDidReceiveEvent(Escher::TextField * textField, Ion::Events::Event event) override;
private:
  constexpr static int k_maxNumberOfDisplayableRows = 5;
  Shared::ListParameterController * parameterController() override;
  int maxNumberOfDisplayableRows() override;
  Shared::FunctionTitleCell * titleCells(int index) override;
  Escher::HighlightCell * expressionCells(int index) override;
  void willDisplayTitleCellAtIndex(Escher::HighlightCell * cell, int j) override;
  void willDisplayExpressionCellAtIndex(Escher::HighlightCell * cell, int j) override;
  void setFunctionNameInTextField(Shared::ExpiringPointer<Shared::ContinuousFunction> function, Escher::TextField * textField);
  ContinuousFunctionStore * modelStore() override;
  TextFieldFunctionTitleCell m_functionTitleCells[k_maxNumberOfDisplayableRows];
  Shared::FunctionExpressionCell m_expressionCells[k_maxNumberOfDisplayableRows];
  ListParameterController m_parameterController;
};

}

#endif

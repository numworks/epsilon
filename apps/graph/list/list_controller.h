#ifndef GRAPH_LIST_CONTROLLER_H
#define GRAPH_LIST_CONTROLLER_H

#include <escher.h>
#include "list_parameter_controller.h"
#include "text_field_function_title_cell.h"
#include "../continuous_function_store.h"
#include <apps/shared/function_expression_cell.h>
#include <apps/shared/function_list_controller.h>
#include <apps/shared/text_field_delegate.h>

namespace Graph {

class ListController : public Shared::FunctionListController, public Shared::TextFieldDelegate {
public:
  ListController(Responder * parentResponder, ButtonRowController * header, ButtonRowController * footer, InputEventHandlerDelegate * inputEventHandlerDelegate);
  const char * title() override;
  void renameSelectedFunction();
  // TextFieldDelegate
  bool textFieldDidFinishEditing(TextField * textField, const char * text, Ion::Events::Event event) override;
  bool textFieldDidAbortEditing(TextField * textField) override;
  bool textFieldShouldFinishEditing(TextField * textField, Ion::Events::Event event) override;
  bool textFieldDidReceiveEvent(TextField * textField, Ion::Events::Event event) override;
protected:
  virtual const char * recordExtension() const override { return Ion::Storage::funcExtension; }
private:
  constexpr static int k_maxNumberOfDisplayableRows = 5;
  Shared::ListParameterController * parameterController() override;
  int maxNumberOfDisplayableRows() override;
  Shared::FunctionTitleCell * titleCells(int index) override;
  HighlightCell * expressionCells(int index) override;
  void willDisplayTitleCellAtIndex(HighlightCell * cell, int j) override;
  void willDisplayExpressionCellAtIndex(HighlightCell * cell, int j) override;
  void setFunctionNameInTextField(Shared::ExpiringPointer<Shared::ContinuousFunction> function, TextField * textField);
  ContinuousFunctionStore * modelStore() override;
  TextFieldFunctionTitleCell m_functionTitleCells[k_maxNumberOfDisplayableRows];
  Shared::FunctionExpressionCell m_expressionCells[k_maxNumberOfDisplayableRows];
  ListParameterController m_parameterController;
};

}

#endif

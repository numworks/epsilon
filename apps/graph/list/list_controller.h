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

namespace Graph {

class ListController : public Shared::FunctionListController, public Shared::TextFieldDelegate, public Shared::LayoutFieldDelegate {
public:
  ListController(Escher::Responder * parentResponder, Escher::ButtonRowController * header, Escher::ButtonRowController * footer, Escher::InputEventHandlerDelegate * inputEventHandlerDelegate);
  const char * title() override;
  // LayoutFieldDelegate
  bool layoutFieldDidReceiveEvent(Escher::LayoutField * layoutField, Ion::Events::Event event) override;
  bool layoutFieldDidFinishEditing(Escher::LayoutField * layoutField, Poincare::Layout layout, Ion::Events::Event event) override;
  // Make methods public
  void editExpression(Ion::Events::Event event) override { return Shared::FunctionListController::editExpression(event); }
  bool editSelectedRecordWithText(const char * text) override { return Shared::FunctionListController::editSelectedRecordWithText(text); }
private:
  constexpr static int k_maxNumberOfDisplayableRows = 5;
  void addModel() override;
  Shared::ListParameterController * parameterController() override;
  int maxNumberOfDisplayableRows() override;
  Escher::HighlightCell * functionCells(int index) override;
  void willDisplayCellForIndex(Escher::HighlightCell * cell, int j) override;
  ContinuousFunctionStore * modelStore() override;
  // TODO create new kind of cell
  Shared::FunctionExpressionCell m_expressionCells[k_maxNumberOfDisplayableRows];
  ListParameterController m_parameterController;
  FunctionModelsParameterController m_modelsParameterController;
  Escher::StackViewController m_modelsStackController;
};

}

#endif

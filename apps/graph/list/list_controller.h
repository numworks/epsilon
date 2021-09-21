#ifndef GRAPH_LIST_CONTROLLER_H
#define GRAPH_LIST_CONTROLLER_H

#include "function_toolbox.h"
#include "list_parameter_controller.h"
#include "text_field_function_title_cell.h"
#include "../continuous_function_store.h"
#include <apps/shared/function_expression_cell.h>
#include <apps/shared/function_list_controller.h>
#include <apps/shared/text_field_delegate.h>
#include <apps/shared/layout_field_delegate.h>
#include <apps/shared/input_event_handler_delegate.h>
#include "../function_models_parameter_controller.h"
#include "function_cell.h"

namespace Graph {

class ListController : public Shared::FunctionListController, public Shared::TextFieldDelegate, public Shared::LayoutFieldDelegate, public Shared::InputEventHandlerDelegate {
public:
  ListController(Escher::Responder * parentResponder, Escher::ButtonRowController * header, Escher::ButtonRowController * footer);
  const char * title() override;
  // LayoutFieldDelegate
  bool layoutFieldDidReceiveEvent(Escher::LayoutField * layoutField, Ion::Events::Event event) override;
  // Responder
  bool handleEvent(Ion::Events::Event event) override;
  // Make methods public
  void editExpression(Ion::Events::Event event) override { return Shared::FunctionListController::editExpression(event); }
  bool editSelectedRecordWithText(const char * text) override { return Shared::FunctionListController::editSelectedRecordWithText(text); }
  Escher::Toolbox * toolboxForInputEventHandler(Escher::InputEventHandler * handler) override;
private:
  constexpr static int k_maxNumberOfDisplayableRows = 5;
  void addModel() override;
  Shared::ListParameterController * parameterController() override;
  int maxNumberOfDisplayableRows() override;
  Escher::HighlightCell * functionCells(int index) override;
  void willDisplayCellForIndex(Escher::HighlightCell * cell, int j) override;
  ContinuousFunctionStore * modelStore() override;
  FunctionCell m_expressionCells[k_maxNumberOfDisplayableRows];
  ListParameterController m_parameterController;
  FunctionModelsParameterController m_modelsParameterController;
  Escher::StackViewController m_modelsStackController;
  FunctionToolbox m_functionToolbox;
  bool m_parameterColumnSelected;
};

}

#endif

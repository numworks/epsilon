#ifndef SHARED_FUNCTION_LIST_CONTROLLER_H
#define SHARED_FUNCTION_LIST_CONTROLLER_H

#include <escher/button_row_controller.h>
#include <escher/stack_view_controller.h>
#include <escher/tab_view_controller.h>
#include <escher/highlight_cell.h>
#include "list_parameter_controller.h"
#include "expression_model_list_controller.h"
#include "input_event_handler_delegate.h"
#include "layout_field_delegate.h"
#include "text_field_delegate.h"

namespace Shared {

class FunctionListController : public ExpressionModelListController, public Escher::ButtonRowDelegate, public Shared::InputEventHandlerDelegate, public Shared::TextFieldDelegate, public Shared::LayoutFieldDelegate {
public:
  FunctionListController(Escher::Responder * parentResponder, Escher::ButtonRowController * header, Escher::ButtonRowController * footer, I18n::Message text);

  TELEMETRY_ID("List");

  /* ButtonRowDelegate */
  int numberOfButtons(Escher::ButtonRowController::Position position) const override;
  Escher::AbstractButtonCell * buttonAtIndex(int index, Escher::ButtonRowController::Position position) const override;

  /* Responder */
  void willExitResponderChain(Escher::Responder * nextFirstResponder) override;
  void didBecomeFirstResponder() override;

  /* ViewController */
  ViewController::TitlesDisplay titlesDisplay() override { return TitlesDisplay::NeverDisplayOwnTitle; }
  void viewWillAppear() override;
protected:
  Escher::StackViewController * stackController() const;
  void configureFunction(Ion::Storage::Record record);
  Escher::TabViewController * tabController() const;
private:
  Escher::InputViewController * inputController() override;
  virtual int initialColumnToSelect() const = 0;
  virtual ListParameterController * parameterController() = 0;
  virtual int maxNumberOfDisplayableRows() = 0;
  virtual Escher::HighlightCell * functionCells(int index) = 0;
  Escher::AbstractButtonCell m_plotButton;
  Escher::AbstractButtonCell m_valuesButton;
};

}

#endif

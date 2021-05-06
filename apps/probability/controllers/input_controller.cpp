#include "input_controller.h"

#include <apps/i18n.h>
#include <escher/highlight_cell.h>
#include <escher/input_event_handler_delegate.h>
#include <escher/invocation.h>
#include <escher/selectable_list_view_controller.h>
#include <escher/text_field_delegate.h>

InputController::InputController(Escher::Responder * parent, Escher::InputEventHandlerDelegate * handler,
                                 Escher::TextFieldDelegate * textFieldDelegate)
    : Escher::SelectableListViewController(parent),
      m_next(&m_selectableTableView, I18n::Message::Ok,
             Escher::Invocation(
                 [](void * ctx, void * sender) -> bool {
                   reinterpret_cast<InputController *>(ctx)->buttonAction();
                   return true;
                 },
                 this)),
      // syntax to initialize list of objects
      m_parameters{{&m_selectableTableView, handler, textFieldDelegate}, {&m_selectableTableView, handler, textFieldDelegate}} {
  m_parameters[0].setMessage(I18n::Message::X);
  m_parameters[0].setAccessoryText(I18n::translate(I18n::Message::NumberOfSuccesses));
  m_parameters[1].setMessage(I18n::Message::N);
  m_parameters[1].setAccessoryText(I18n::translate(I18n::Message::SampleSize));
}

Escher::HighlightCell * InputController::reusableCell(int i, int type) {
  if (i < k_numberOfParameters) {
    return &m_parameters[i];
  }
  assert(i == k_numberOfParameters);
  return &m_next;
}

void InputController::buttonAction() {}

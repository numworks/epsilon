#include "results_controller.h"

#include <apps/i18n.h>
#include <escher/input_event_handler_delegate.h>
#include <escher/responder.h>
#include <escher/selectable_list_view_controller.h>
#include <escher/stack_view_controller.h>
#include <escher/text_field_delegate.h>

template <int numberOfResults>
ResultsController<numberOfResults>::ResultsController(Escher::StackViewController * parent,
                                                      Escher::InputEventHandlerDelegate * handler,
                                                      Escher::TextFieldDelegate * textFieldDelegate)
    : SelectableListViewPage(parent),
      m_next(&m_selectableTableView, I18n::Message::Ok, buttonActionInvocation()) {}

template <int numberOfResults>
Escher::HighlightCell * ResultsController<numberOfResults>::reusableCell(int i, int type) {
  if (i < numberOfResults) {
    return &m_cells[i];
  }
  assert(i == numberOfResults);
  return &m_next;
}

template <int numberOfResults>
void ResultsController<numberOfResults>::buttonAction() {}

TestResults::TestResults(Escher::StackViewController * parent, Escher::InputEventHandlerDelegate * handler,
                         Escher::TextFieldDelegate * textFieldDelegate)
    : ResultsController(parent, handler, textFieldDelegate) {
  m_cells[k_indexOfZ].setMessage(I18n::Message::Z);
  m_cells[k_indexOfPVal].setMessage(I18n::Message::PValue);
}

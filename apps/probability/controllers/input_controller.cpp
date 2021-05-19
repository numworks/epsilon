#include "input_controller.h"

#include <escher/stack_view_controller.h>

#include "results_controller.h"

using namespace Probability;

template <int n>
inline InputController<n>::InputController(Escher::StackViewController * parent, TestResults * resultsController,
                                           Escher::InputEventHandlerDelegate * handler,
                                           Escher::TextFieldDelegate * textFieldDelegate)
    : SelectableListViewPage(parent),
      m_resultsController(resultsController),
      m_parameters{{&m_selectableTableView, handler, textFieldDelegate},
                   {&m_selectableTableView, handler, textFieldDelegate}},
      m_next(&m_selectableTableView, I18n::Message::Ok, buttonActionInvocation()) {
  // Initialize parameter cells
  for (int i = 0; i < k_numberOfParameters; i++) {
    m_parameters[i].setParentResponder(&m_selectableTableView);
    m_parameters[i].textField()->setDelegates(handler, textFieldDelegate);
  }
}

template <int n>
inline Escher::HighlightCell * InputController<n>::reusableCell(int i, int type) {
  if (i < k_numberOfParameters) {
    return &m_parameters[i];
  }
  assert(i == k_numberOfParameters);
  return &m_next;
}

template <int n>
inline void InputController<n>::buttonAction() {
  openPage(m_resultsController, true);
}

NormalInputController::NormalInputController(Escher::StackViewController * parent, TestResults * resultsController,
                                             Escher::InputEventHandlerDelegate * handler,
                                             Escher::TextFieldDelegate * textFieldDelegate)
    : InputController(parent, resultsController, handler, textFieldDelegate) {
  m_parameters[k_indexOfX].setMessage(I18n::Message::X);
  m_parameters[k_indexOfX].setAccessoryText(I18n::translate(I18n::Message::NumberOfSuccesses));
  m_parameters[k_indexOfN].setMessage(I18n::Message::N);
  m_parameters[k_indexOfN].setAccessoryText(I18n::translate(I18n::Message::SampleSize));
}

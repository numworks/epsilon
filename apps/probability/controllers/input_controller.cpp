#include "input_controller.h"

#include <escher/stack_view_controller.h>
#include <poincare/print_float.h>
#include <string.h>

#include "probability/app.h"
#include "probability/data.h"
#include "probability/helpers.h"
#include "results_controller.h"

using namespace Probability;

template <int n>
InputController<n>::InputController(Escher::StackViewController * parent,
                                    TestResults * resultsController,
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
const char * InputController<n>::title() {
  // H0:<first symbol>=<firstParam> Ha:<first symbol><operator symbol><firstParams>
  m_titleBuffer[0] = 0;
  size_t bufferSize = sizeof(m_titleBuffer);
  const char * H0 = "H0:";
  const char * Ha = "  Ha:";
  const char * eq = "=";  // TODO Must be somewhere else
  const char * symbol = testToTextSymbol(App::app()->snapshot()->data()->test());
  char op[2] = {0, 0};
  op[0] = static_cast<const char>(App::app()->snapshot()->data()->hypothesisParams()->op());
  char paramBuffer[10];
  Poincare::PrintFloat::ConvertFloatToText(
      App::app()->snapshot()->data()->hypothesisParams()->firstParam(), paramBuffer,
      sizeof(paramBuffer), 5, 5, Poincare::Preferences::PrintFloatMode::Decimal);

  strlcat(m_titleBuffer, H0, bufferSize);
  strlcat(m_titleBuffer, symbol, bufferSize);
  strlcat(m_titleBuffer, eq, bufferSize);
  strlcat(m_titleBuffer, paramBuffer, bufferSize);
  strlcat(m_titleBuffer, Ha, bufferSize);
  strlcat(m_titleBuffer, symbol, bufferSize);
  strlcat(m_titleBuffer, op, bufferSize);
  strlcat(m_titleBuffer, paramBuffer, bufferSize);

  return m_titleBuffer;
}

template <int n>
Escher::HighlightCell * InputController<n>::reusableCell(int i, int type) {
  if (i < k_numberOfParameters) {
    return &m_parameters[i];
  }
  assert(i == k_numberOfParameters);
  return &m_next;
}

template <int n>
void InputController<n>::didBecomeFirstResponder() {
  Probability::App::app()->snapshot()->navigation()->setPage(Data::Page::Input);
  // TODO factor out
  if (selectedRow() == -1) {
    selectCellAtLocation(0, 0);
  } else {
    selectCellAtLocation(selectedColumn(), selectedRow());
  }
  Escher::Container::activeApp()->setFirstResponder(&m_selectableTableView);
}

template <int n>
void InputController<n>::buttonAction() {
  openPage(m_resultsController);
}

NormalInputController::NormalInputController(Escher::StackViewController * parent,
                                             TestResults * resultsController,
                                             Escher::InputEventHandlerDelegate * handler,
                                             Escher::TextFieldDelegate * textFieldDelegate)
    : InputController(parent, resultsController, handler, textFieldDelegate) {
  m_parameters[k_indexOfX].setMessage(I18n::Message::X);
  m_parameters[k_indexOfX].setAccessoryText(I18n::translate(I18n::Message::NumberOfSuccesses));
  m_parameters[k_indexOfN].setMessage(I18n::Message::N);
  m_parameters[k_indexOfN].setAccessoryText(I18n::translate(I18n::Message::SampleSize));
}
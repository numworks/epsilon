#include "interval_parameter_selector_controller.h"
#include "../../shared/interval_parameter_controller.h"
#include "../app.h"
#include <apps/i18n.h>
#include <assert.h>

namespace Graph {

IntervalParameterSelectorController::IntervalParameterSelectorController() :
  ViewController(nullptr),
  m_selectableTableView(this, this, this)
{
}

const char * IntervalParameterSelectorController::title() {
  return I18n::translate(I18n::Message::IntervalSet);
}

void IntervalParameterSelectorController::didBecomeFirstResponder() {
  if (selectedRow() < 0) {
    selectCellAtLocation(0, 0);
  }
  Container::activeApp()->setFirstResponder(&m_selectableTableView);
}

bool IntervalParameterSelectorController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE || event == Ion::Events::Right) {
    StackViewController * stack = (StackViewController *)parentResponder();
    Shared::IntervalParameterController * controller = App::app()->valuesController()->intervalParameterController();
    controller->setTitle(messageAtIndex(selectedRow()));
    controller->setInterval(App::app()->interval());
    stack->push(controller);
    return true;
  }
  return false;
}

int IntervalParameterSelectorController::numberOfRows() {
  return MaxNumberOfRows;
};

HighlightCell * IntervalParameterSelectorController::reusableCell(int index) {
  assert(0 <= index && index < reusableCellCount());
  return m_intervalParameterCell + index;
}

int IntervalParameterSelectorController::reusableCellCount() {
  return MaxNumberOfRows;
}

void IntervalParameterSelectorController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  assert(0 <= index && index < MaxNumberOfRows);
  static_cast<MessageTableCellWithChevron *>(cell)->setMessage(messageAtIndex(index));
}

I18n::Message IntervalParameterSelectorController::messageAtIndex(int index) {
  assert(0 <= index && index < MaxNumberOfRows);
  constexpr I18n::Message message[MaxNumberOfRows] = {
    I18n::Message::IntervalX
  };
  return message[index];
}

}

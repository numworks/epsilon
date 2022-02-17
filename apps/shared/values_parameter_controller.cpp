#include "values_parameter_controller.h"
#include "function_app.h"
#include <assert.h>

using namespace Escher;

namespace Shared {

ValuesParameterController::ValuesParameterController(Responder * parentResponder) :
  SelectableListViewController(parentResponder),
  m_deleteColumn(I18n::Message::Default),
#if COPY_COLUMN
  m_copyColumn(I18n::Message::Default),
#endif
  m_setInterval(I18n::Message::Default)
{
}

const char * ValuesParameterController::title() {
  return I18n::translate(m_pageTitle);
}

void ValuesParameterController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  MessageTableCell * myCell = static_cast<MessageTableCell *>(cell);
#if COPY_COLUMN
  I18n::Message labels[k_totalNumberOfCell] = {I18n::Message::ClearColumn, I18n::Message::CopyColumnInList, I18n::Message::IntervalSet};
#else
  I18n::Message labels[k_totalNumberOfCell] = {I18n::Message::ClearColumn, I18n::Message::IntervalSet};
#endif
  myCell->setMessage(labels[index]);
}

void ValuesParameterController::didBecomeFirstResponder() {
  if (selectedRow() < 0) {
    selectCellAtLocation(0, 0);
  }
  Container::activeApp()->setFirstResponder(&m_selectableTableView);
}

bool ValuesParameterController::handleEvent(Ion::Events::Event event) {
#if COPY_COLUMN
  if (event == Ion::Events::OK || event == Ion::Events::EXE || (event == Ion::Events::Right && selectedRow() == 2)) {
#else
  if (event == Ion::Events::OK || event == Ion::Events::EXE || (event == Ion::Events::Right && selectedRow() == 1)) {
#endif
    ValuesController * valuesController = FunctionApp::app()->valuesController();
    switch (selectedRow()) {
      case 0:
      {
        StackViewController * stack = static_cast<StackViewController *>(parentResponder());
        stack->pop();
        valuesController->tryToDeleteColumn();
        return true;
      }
#if COPY_COLUMN
      case 1:
      /* TODO: implement function copy column */
      return true;
      case 2:
#else
      case 1:
#endif
      {
        StackViewController * stack = ((StackViewController *)parentResponder());
        IntervalParameterController * intervalParameterController = valuesController->intervalParameterController();
        intervalParameterController->setTitle(I18n::Message::IntervalSet);
        stack->push(intervalParameterController);
        return true;
      }
      default:
        assert(false);
        return false;
    }
  }
  return false;
}

int ValuesParameterController::numberOfRows() const {
  return k_totalNumberOfCell;
};

HighlightCell * ValuesParameterController::reusableCell(int index, int type) {
  assert(index >= 0);
  assert(index < k_totalNumberOfCell);
  HighlightCell * cells[] = {&m_deleteColumn, &m_setInterval}; //{&m_deleteColumn, &m_copyColumn, &m_setInterval};
  return cells[index];
}

}

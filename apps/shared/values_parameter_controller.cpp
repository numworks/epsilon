#include "values_parameter_controller.h"
#include "function_app.h"
#include <assert.h>

namespace Shared {

ValuesParameterController::ValuesParameterController(Responder * parentResponder) :
  ViewController(parentResponder),
  m_deleteColumn(I18n::Message::Default),
#if COPY_COLUMN
  m_copyColumn(I18n::Message::Default),
#endif
  m_setInterval(I18n::Message::Default),
  m_selectableTableView(this, this, this)
{
}

const char * ValuesParameterController::title() {
  return I18n::translate(m_pageTitle);
}

View * ValuesParameterController::view() {
  return &m_selectableTableView;
}

void ValuesParameterController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  MessageTableCell<> * myCell = (MessageTableCell<> *)cell;
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
    IntervalParameterController * intervalParameterController = FunctionApp::app()->valuesController()->intervalParameterController();
    switch (selectedRow()) {
      case 0:
      {
        Interval * interval = intervalParameterController->interval();
        interval->clear();
        StackViewController * stack = ((StackViewController *)parentResponder());
        stack->pop();
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

HighlightCell * ValuesParameterController::reusableCell(int index) {
  assert(index >= 0);
  assert(index < k_totalNumberOfCell);
  HighlightCell * cells[] = {&m_deleteColumn, &m_setInterval}; //{&m_deleteColumn, &m_copyColumn, &m_setInterval};
  return cells[index];
}

int ValuesParameterController::reusableCellCount() const {
  return k_totalNumberOfCell;
}

KDCoordinate ValuesParameterController::cellHeight() {
  return Metric::ParameterCellHeight;
}

}

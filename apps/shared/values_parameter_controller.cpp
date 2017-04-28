#include "values_parameter_controller.h"
#include <assert.h>

namespace Shared {

ValuesParameterController::ValuesParameterController(Responder * parentResponder, IntervalParameterController * intervalParameterController, I18n::Message title) :
  ViewController(parentResponder),
  m_pageTitle(title),
  m_deleteColumn(MessageTableCell(I18n::Message::Default)),
  //m_copyColumn(MessageTableCellWithChevron(I18n::Message::Default)),
  m_setInterval(MessageTableCellWithChevron(I18n::Message::Default)),
  m_selectableTableView(SelectableTableView(this, this, 0, 1, Metric::CommonTopMargin, Metric::CommonRightMargin,
    Metric::CommonBottomMargin, Metric::CommonLeftMargin, this)),
  m_intervalParameterController(intervalParameterController)
{
}

const char * ValuesParameterController::title() {
  return I18n::translate(m_pageTitle);
}

View * ValuesParameterController::view() {
  return &m_selectableTableView;
}

void ValuesParameterController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  MessageTableCell * myCell = (MessageTableCell *)cell;
  I18n::Message labels[k_totalNumberOfCell] = {I18n::Message::ClearColumn, I18n::Message::IntervalSet};// {I18n::Message::ClearColumn, I18n::Message::CopyColumnInList, I18n::Message::IntervalSet};
  myCell->setMessage(labels[index]);
}

void ValuesParameterController::didBecomeFirstResponder() {
  selectCellAtLocation(0, 0);
  app()->setFirstResponder(&m_selectableTableView);
}

bool ValuesParameterController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK) {
    switch (selectedRow()) {
      case 0:
      {
        Interval * interval = m_intervalParameterController->interval();
        interval->setEnd(0.0f);
        interval->setStep(1.0f);
        interval->setStart(1.0f);
        StackViewController * stack = ((StackViewController *)parentResponder());
        stack->pop();
        return true;
      }
      /* TODO: implement copy column
       * case 1:
        return false;
        */
      case 1:
      {
        StackViewController * stack = ((StackViewController *)parentResponder());
        stack->push(m_intervalParameterController);
        return true;
      }
      default:
        assert(false);
        return false;
    }
  }
  return false;
}

int ValuesParameterController::numberOfRows() {
  return k_totalNumberOfCell;
};

HighlightCell * ValuesParameterController::reusableCell(int index) {
  assert(index >= 0);
  assert(index < k_totalNumberOfCell);
  HighlightCell * cells[] = {&m_deleteColumn, &m_setInterval}; //{&m_deleteColumn, &m_copyColumn, &m_setInterval};
  return cells[index];
}

int ValuesParameterController::reusableCellCount() {
  return k_totalNumberOfCell;
}

KDCoordinate ValuesParameterController::cellHeight() {
  return Metric::ParameterCellHeight;
}

}

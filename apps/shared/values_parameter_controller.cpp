#include "values_parameter_controller.h"
#include <assert.h>

namespace Shared {

ValuesParameterController::ValuesParameterController(Responder * parentResponder, IntervalParameterController * intervalParameterController, I18n::Message title) :
  ViewController(parentResponder),
  m_pageTitle(title),
  m_deleteColumn(I18n::Message::Default),
#if COPY_COLUMN
  m_copyColumn(I18n::Message::Default),
#endif
  m_setInterval(I18n::Message::Default),
  m_selectableTableView(this, this, 0, 1, Metric::CommonTopMargin, Metric::CommonRightMargin,
    Metric::CommonBottomMargin, Metric::CommonLeftMargin, this),
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
  app()->setFirstResponder(&m_selectableTableView);
}

bool ValuesParameterController::handleEvent(Ion::Events::Event event) {
#if COPY_COLUMN
  if (event == Ion::Events::OK || event == Ion::Events::EXE || (event == Ion::Events::Right && selectedRow() == 2)) {
#else
  if (event == Ion::Events::OK || event == Ion::Events::EXE || (event == Ion::Events::Right && selectedRow() == 1)) {
#endif
    switch (selectedRow()) {
      case 0:
      {
        Interval * interval = m_intervalParameterController->interval();
        interval->setEnd(0.0);
        interval->setStep(1.0);
        interval->setStart(1.0);
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

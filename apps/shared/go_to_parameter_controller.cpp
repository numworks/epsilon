#include "go_to_parameter_controller.h"
#include "text_field_delegate_app.h"
#include <assert.h>

namespace Shared {

GoToParameterController::GoToParameterController(Responder * parentResponder, InteractiveCurveViewRange * graphRange, CurveViewCursor * cursor, I18n::Message symbol) :
  FloatParameterController(parentResponder),
  m_cursor(cursor),
  m_graphRange(graphRange),
  m_abscisseCell(nullptr),
  m_abscissaSymbol(symbol)
{
}

int GoToParameterController::numberOfRows() {
  return 2;
}

HighlightCell * GoToParameterController::reusableParameterCell(int index, int type) {
  assert(index == 0);
  return m_abscisseCell;
}

int GoToParameterController::reusableParameterCellCount(int type) {
  return 1;
}

bool GoToParameterController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Left) {
    stackController()->pop();
    return true;
  }
  return false;
}

void GoToParameterController::buttonAction() {
  StackViewController * stack = (StackViewController *)parentResponder();
  stack->pop();
  stack->pop();
}

View * GoToParameterController::loadView() {
  SelectableTableView * tableView = (SelectableTableView *)FloatParameterController::loadView();
  m_abscisseCell = new MessageTableCellWithEditableText(tableView, this, m_draftTextBuffer, m_abscissaSymbol);
  return tableView;
}

void GoToParameterController::unloadView(View * view) {
  delete m_abscisseCell;
  m_abscisseCell = nullptr;
  FloatParameterController::unloadView(view);
}

}

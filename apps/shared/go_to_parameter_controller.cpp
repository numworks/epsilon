#include "go_to_parameter_controller.h"
#include "text_field_delegate_app.h"
#include <assert.h>

namespace Shared {

GoToParameterController::GoToParameterController(Responder * parentResponder, InteractiveCurveViewRange * graphRange, CurveViewCursor * cursor, I18n::Message symbol) :
  FloatParameterController(parentResponder),
  m_cursor(cursor),
  m_abscisseCell(nullptr),
  m_graphRange(graphRange),
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

void GoToParameterController::buttonAction() {
  m_graphRange->centerAxisAround(CurveViewRange::Axis::X, m_cursor->x());
  m_graphRange->centerAxisAround(CurveViewRange::Axis::Y, m_cursor->y());
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

#include "simple_list_controller.h"
#include "../app.h"

using namespace Poincare;

namespace Calculation {

/* List Controller */

SimpleListController::ListController::ListController(SimpleListController * dataSource) :
  ViewController(dataSource),
  m_selectableTableView(this, dataSource, dataSource)
{
  m_selectableTableView.setMargins(0);
  m_selectableTableView.setDecoratorType(ScrollView::Decorator::Type::None);
}

/* Simple list controller */

SimpleListController::SimpleListController(Responder * parentResponder) :
  StackViewController(parentResponder, &m_listController, KDColorWhite, Palette::PurpleBright, Palette::PurpleDark),
  m_listController(this),
  m_cells{}
{
}

bool SimpleListController::handleEvent(Ion::Events::Event event) {
  return false;
}

void SimpleListController::didBecomeFirstResponder() {
  Container::activeApp()->setFirstResponder(&m_listController);
}

int SimpleListController::reusableCellCount(int type) {
  return k_maxNumberOfCells;
}

HighlightCell * SimpleListController::reusableCell(int index, int type) {
  return &m_cells[index];
}

KDCoordinate SimpleListController::rowHeight(int j) {
  Layout l = layoutAtIndex(j);
  if (l.isUninitialized()) {
    return 0;
  }
  return l.layoutSize().height() + 2 * Metric::CommonSmallMargin + Metric::CellSeparatorThickness;
}

void SimpleListController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  ExpressionTableCellWithPointer * myCell = static_cast<ExpressionTableCellWithPointer *>(cell);
  myCell->setLayout(layoutAtIndex(index));
  myCell->setAccessoryMessage(messageAtIndex(index));
}

}

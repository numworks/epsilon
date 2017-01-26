#include "store_controller.h"
#include "app.h"
#include "../apps_container.h"
#include "../constant.h"
#include "../../poincare/src/layout/subscript_layout.h"
#include "../../poincare/src/layout/string_layout.h"
#include <assert.h>

namespace Regression {

StoreController::StoreController(Responder * parentResponder, Store * store, HeaderViewController * header) :
  ::StoreController(parentResponder, store, header),
  m_titleCells{EvenOddExpressionCell(0.5f, 0.5f), EvenOddExpressionCell(0.5f, 0.5f)}
{
  m_titleLayout[0] = new SubscriptLayout(new StringLayout("X", 1, KDText::FontSize::Small), new StringLayout("i", 1, KDText::FontSize::Small));
  m_titleLayout[1] = new SubscriptLayout(new StringLayout("Y", 1, KDText::FontSize::Small), new StringLayout("i", 1, KDText::FontSize::Small));
}

void StoreController::willDisplayCellAtLocation(TableViewCell * cell, int i, int j) {
  EditableCellTableViewController::willDisplayCellAtLocation(cell, i, j);
  if (cellAtLocationIsEditable(i, j)) {
    return;
  }
  EvenOddExpressionCell * mytitleCell = (EvenOddExpressionCell *)cell;
  mytitleCell->setExpression(m_titleLayout[i]);
}

TableViewCell * StoreController::titleCells(int index) {
  assert(index >= 0 && index < k_numberOfTitleCells);
  return &m_titleCells[index];
}

}

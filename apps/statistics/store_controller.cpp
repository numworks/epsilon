#include "store_controller.h"
#include "app.h"
#include "../apps_container.h"
#include "../constant.h"
#include <assert.h>

namespace Statistics {

StoreController::StoreController(Responder * parentResponder, Store * store, HeaderViewController * header) :
  ::StoreController(parentResponder, store, header),
  m_titleCells{EvenOddPointerTextCell(KDText::FontSize::Small), EvenOddPointerTextCell(KDText::FontSize::Small)}
{
}

void StoreController::willDisplayCellAtLocation(TableViewCell * cell, int i, int j) {
  ::StoreController::willDisplayCellAtLocation(cell, i, j);
  if (cellAtLocationIsEditable(i, j)) {
    return;
  }
  EvenOddPointerTextCell * mytitleCell = (EvenOddPointerTextCell *)cell;
  if (i == 0) {
    mytitleCell->setText("Valeurs");
    return;
  }
  mytitleCell->setText("Effectifs");
}

TableViewCell * StoreController::titleCells(int index) {
  assert(index >= 0 && index < k_numberOfTitleCells);
  return &m_titleCells[index];
}

}

#include "store_controller.h"
#include "app.h"
#include "../apps_container.h"
#include "../constant.h"
#include <assert.h>

using namespace Shared;

namespace Statistics {

StoreController::StoreController(Responder * parentResponder, Store * store, ButtonRowController * header) :
  Shared::StoreController(parentResponder, store, header),
  m_titleCells{EvenOddPointerTextCell(KDText::FontSize::Small), EvenOddPointerTextCell(KDText::FontSize::Small)}
{
}

void StoreController::willDisplayCellAtLocation(HighlightCell * cell, int i, int j) {
  ::StoreController::willDisplayCellAtLocation(cell, i, j);
  if (cellAtLocationIsEditable(i, j)) {
    return;
  }
  EvenOddPointerTextCell * mytitleCell = (EvenOddPointerTextCell *)cell;
  if (i == 0) {
    mytitleCell->setMessage(I18n::Message::Values);
    return;
  }
  mytitleCell->setMessage(I18n::Message::Sizes);
}

HighlightCell * StoreController::titleCells(int index) {
  assert(index >= 0 && index < k_numberOfTitleCells);
  return &m_titleCells[index];
}

}

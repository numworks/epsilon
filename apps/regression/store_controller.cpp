#include "store_controller.h"
#include "app.h"
#include "../apps_container.h"
#include "../constant.h"
#include "../../poincare/src/layout/char_layout.h"
#include "../../poincare/src/layout/horizontal_layout.h"
#include "../../poincare/src/layout/vertical_offset_layout.h"
#include <assert.h>

using namespace Poincare;
using namespace Shared;

namespace Regression {

StoreController::StoreController(Responder * parentResponder, Store * store, ButtonRowController * header) :
  Shared::StoreController(parentResponder, store, header),
  m_titleCells{}
{
  m_titleLayout[0] = new HorizontalLayout(new CharLayout('X', KDText::FontSize::Small), new VerticalOffsetLayout(new CharLayout('i', KDText::FontSize::Small), VerticalOffsetLayout::Type::Subscript, false), false);
  m_titleLayout[1] = new HorizontalLayout(new CharLayout('Y', KDText::FontSize::Small), new VerticalOffsetLayout(new CharLayout('i', KDText::FontSize::Small), VerticalOffsetLayout::Type::Subscript, false), false);
}

StoreController::~StoreController() {
  for (int i = 0; i < 2; i++) {
    delete m_titleLayout[i];
    m_titleLayout[i] = nullptr;
  }
}

void StoreController::willDisplayCellAtLocation(HighlightCell * cell, int i, int j) {
  ::StoreController::willDisplayCellAtLocation(cell, i, j);
  if (cellAtLocationIsEditable(i, j)) {
    return;
  }
  EvenOddExpressionCell * mytitleCell = (EvenOddExpressionCell *)cell;
  mytitleCell->setExpressionLayout(m_titleLayout[i]);
}

HighlightCell * StoreController::titleCells(int index) {
  assert(index >= 0 && index < k_numberOfTitleCells);
  return m_titleCells[index];
}

View * StoreController::loadView() {
  for (int i = 0; i < k_numberOfTitleCells; i++) {
    m_titleCells[i] = new EvenOddExpressionCell(0.5f, 0.5f);
  }
  return Shared::StoreController::loadView();
}

void StoreController::unloadView(View * view) {
  for (int i = 0; i < k_numberOfTitleCells; i++) {
    delete m_titleCells[i];
    m_titleCells[i] = nullptr;
  }
  Shared::StoreController::unloadView(view);
}

}

#include "store_menu_controller.h"
#include <escher/clipboard.h>

using namespace Poincare;
using namespace Shared;
using namespace Ion;
using namespace Escher;

StoreMenuController::StoreMenuController() :
  NestedMenuController(nullptr, I18n::Message::Store)
{
}

void StoreMenuController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  BufferTableCell * myCell = static_cast<BufferTableCell *>(cell);
  myCell->setLabelText(Escher::Clipboard::sharedStoreBuffer()->storedText());
  myCell->reloadCell();
}

KDCoordinate StoreMenuController::nonMemoizedRowHeight(int index) {
  BufferTableCell tempCell;
  return heightForCellAtIndexWithWidthInit(&tempCell, index);
}

#include "store_controller.h"
#include "app.h"
#include "../apps_container.h"
#include "../constant.h"
#include <assert.h>

namespace Statistics {

StoreController::StoreController(Responder * parentResponder, Store * store, HeaderViewController * header) :
  ::StoreController(parentResponder, store, header)
{
}

void StoreController::willDisplayCellAtLocation(TableViewCell * cell, int i, int j) {
  EditableCellTableViewController::willDisplayCellAtLocation(cell, i, j);
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

}

#include "data_controller.h"
#include "app.h"
#include "../apps_container.h"
#include "../constant.h"
#include <assert.h>

namespace Statistics {

DataController::DataController(Responder * parentResponder, Data * data) :
  ::DataController(parentResponder, data)
{
}

void DataController::willDisplayCellAtLocation(TableViewCell * cell, int i, int j) {
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

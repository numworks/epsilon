#include "store_parameter_controller.h"
#include "store_controller.h"
#include <assert.h>

namespace Shared {

StoreParameterController::StoreParameterController(Responder * parentResponder, DoublePairStore * store, StoreController * storeController) :
  ViewController(parentResponder),
  m_store(store),
  m_series(0),
  m_selectableTableView(this, this, this),
  m_deleteColumn(I18n::Message::ClearColumn),
  m_fillWithFormula(I18n::Message::FillWithFormula),
#if COPY_IMPORT_LIST
  m_copyColumn(I18n::Message::CopyColumnInList),
  m_importList(I18n::Message::ImportList),
#endif
  m_storeController(storeController),
  m_xColumnSelected(true)
{
}

const char * StoreParameterController::title() {
  return I18n::translate(I18n::Message::ColumnOptions);
}

void StoreParameterController::didBecomeFirstResponder() {
  selectCellAtLocation(0, 0);
  Container::activeApp()->setFirstResponder(&m_selectableTableView);
}

bool StoreParameterController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    switch (selectedRow()) {
      case 0:
      {
        if (m_xColumnSelected) {
          m_store->deleteAllPairsOfSeries(m_series);
        } else {
          m_store->resetColumn(m_series, !m_xColumnSelected);
        }
        StackViewController * stack = ((StackViewController *)parentResponder());
        stack->pop();
        return true;
      }
      case 1:
      {
        m_storeController->displayFormulaInput();
        StackViewController * stack = ((StackViewController *)parentResponder());
        stack->pop();
        return true;
      }

#if COPY_IMPORT_LIST
      /* TODO: implement copy column and import list */
      case 2:
        return true;
      case 3:
        return true;
#endif
      default:
        assert(false);
        return false;
    }
  }
  return false;
}

KDCoordinate StoreParameterController::cumulatedHeightFromIndex(int j) {
  assert (j >= 0 && j <= numberOfRows());
  KDCoordinate result = 0;
  for (int i = 0; i < j; i++) {
    result+= rowHeight(i);
  }
  return result;
}

int StoreParameterController::indexFromCumulatedHeight(KDCoordinate offsetY) {
  int result = 0;
  int j = 0;
  while (result < offsetY && j < numberOfRows()) {
    result += rowHeight(j++);
  }
  return (result < offsetY || offsetY == 0) ? j : j - 1;
}

HighlightCell * StoreParameterController::reusableCell(int index, int type) {
  assert(type == k_standardCellType);
  assert(index >= 0);
  assert(index < k_totalNumberOfCell);
  HighlightCell * cells[] = {&m_deleteColumn, &m_fillWithFormula};// {&m_deleteColumn, &m_fillWithFormula, &m_copyColumn, &m_importList};
  return cells[index];
}

}

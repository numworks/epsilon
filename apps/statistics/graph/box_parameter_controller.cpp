#include "box_parameter_controller.h"

using namespace Escher;

namespace Statistics {

BoxParameterController::BoxParameterController(Responder* parentResponder,
                                               Store* store,
                                               int* selectedBoxCalculation)
    : SelectableCellListPage<
          MenuCell<MessageTextView, EmptyCellWidget, SwitchView>,
          k_numberOfParameterCells, Escher::RegularListViewDataSource>(
          parentResponder),
      m_store(store),
      m_selectedBoxCalculation(selectedBoxCalculation) {
  cellAtIndex(0)->label()->setMessage(I18n::Message::DisplayOutliers);
  // Being the only cell, it is always highlighted.
  cellAtIndex(0)->setHighlighted(true);
}

void BoxParameterController::viewWillAppear() {
  cellAtIndex(0)->accessory()->setState(m_store->displayOutliers());
}

void BoxParameterController::willDisplayCellForIndex(HighlightCell* cell,
                                                     int index) {}

bool BoxParameterController::handleEvent(Ion::Events::Event event) {
  if (cellAtIndex(0)->canBeActivatedByEvent(event)) {
    m_store->setDisplayOutliers(!m_store->displayOutliers());
    cellAtIndex(0)->accessory()->setState(m_store->displayOutliers());
    // Reset selected box calculation because the total number may have changed
    *m_selectedBoxCalculation = 0;
    return true;
  }
  return false;
}

}  // namespace Statistics

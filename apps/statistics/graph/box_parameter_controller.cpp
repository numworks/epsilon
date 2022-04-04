#include "box_parameter_controller.h"

using namespace Escher;

namespace Statistics {

BoxParameterController::BoxParameterController(Responder * parentResponder, Store * store) :
  SelectableCellListPage<MessageTableCellWithSwitch, k_numberOfParameterCells, Escher::RegularListViewDataSource>(parentResponder),
  m_store(store)
{
  cellAtIndex(0)->setMessage(I18n::Message::DisplayOutliers);
  // Being the only cell, it is always highlighted.
  cellAtIndex(0)->setHighlighted(true);
}

void BoxParameterController::viewWillAppear() {
  cellAtIndex(0)->setState(m_store->displayOutliers());
}

void BoxParameterController::willDisplayCellForIndex(HighlightCell * cell, int index) {
}

bool BoxParameterController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    m_store->setDisplayOutliers(!m_store->displayOutliers());
    cellAtIndex(0)->setState(m_store->displayOutliers());
    return true;
  }
  return false;
}

}

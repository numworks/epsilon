#include "box_parameter_controller.h"

using namespace Escher;

namespace Statistics {

BoxParameterController::BoxParameterController(
    Responder* parentResponder, Store* store,
    DataViewController* dataViewController)
    : UniformSelectableListController<
          MenuCell<MessageTextView, EmptyCellWidget, SwitchView>,
          k_numberOfCells>(parentResponder),
      m_store(store),
      m_dataViewController(dataViewController) {
  cell(0)->label()->setMessage(I18n::Message::DisplayOutliers);
  // Being the only cell, it is always highlighted.
  cell(0)->setHighlighted(true);
}

void BoxParameterController::viewWillAppear() { updateDisplayOutliersSwitch(); }

bool BoxParameterController::handleEvent(Ion::Events::Event event) {
  if (cell(0)->canBeActivatedByEvent(event)) {
    m_store->setDisplayOutliers(!m_store->displayOutliers());
    updateDisplayOutliersSwitch();
    // Reset selected box calculation because the total number may have changed
    m_dataViewController->setSelectedIndex(0);
    return true;
  }
  return false;
}

}  // namespace Statistics

#include "grid_type_controller.h"

#include <apps/i18n.h>

#include "images/cartesian_grid_icon.h"
#include "images/polar_grid_icon.h"

using namespace Escher;

namespace Shared {

GridTypeController::GridTypeController(
    Escher::Responder *parentResponder,
    InteractiveCurveViewRange *interactiveCurveViewRange)
    : Escher::ExplicitSelectableListViewController(parentResponder, this),
      m_viewRange(interactiveCurveViewRange) {
  m_cells[0].label()->setMessage(I18n::Message::CartesianGrid);
  m_cells[1].label()->setMessage(I18n::Message::PolarGrid);

  m_cells[0].accessory()->setImage(ImageStore::CartesianGridIcon);
  m_cells[1].accessory()->setImage(ImageStore::PolarGridIcon);
}

const char *GridTypeController::title() {
  return I18n::translate(I18n::Message::GridType);
}

void GridTypeController::viewWillAppear() {
  selectRow(m_viewRange->gridType() == GridType::Cartesian ? 0 : 1);
}

bool GridTypeController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    /* Update the view range before ViewWillAppear is
     * called. */
    m_viewRange->setGridType(selectedRow() == 0 ? GridType::Cartesian
                                                : GridType::Polar);
    stackController()->pop();
    return true;
  }
  if (event == Ion::Events::Back || event == Ion::Events::Left) {
    stackController()->pop();
    return true;
  }
  return false;
}

}  // namespace Shared

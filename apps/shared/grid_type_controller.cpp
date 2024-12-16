#include "grid_type_controller.h"

#include <apps/i18n.h>

#include "cartesian_grid_icon.h"
#include "interactive_curve_view_range.h"
#include "polar_grid_icon.h"

using namespace Escher;

namespace Shared {

GridTypeController::GridTypeController(
    Escher::Responder* parentResponder,
    InteractiveCurveViewRange* interactiveCurveViewRange)
    : UniformSelectableListController<
          MenuCell<MessageTextView, EmptyCellWidget, TransparentImageView>, 2>(
          parentResponder, nullptr),
      m_interactiveCurveViewRange(interactiveCurveViewRange) {
  cell(0)->label()->setMessage(I18n::Message::Cartesian);
  cell(1)->label()->setMessage(I18n::Message::Polar);

  cell(0)->accessory()->setImage(ImageStore::CartesianGridIcon);
  cell(1)->accessory()->setImage(ImageStore::PolarGridIcon);
}

const char* GridTypeController::title() const {
  return I18n::translate(I18n::Message::GridType);
}

void GridTypeController::viewWillAppear() {
  selectRow(m_interactiveCurveViewRange->gridType() == GridType::Cartesian ? 0
                                                                           : 1);
}

bool GridTypeController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    m_interactiveCurveViewRange->setGridType(
        selectedRow() == 0 ? GridType::Cartesian : GridType::Polar);
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

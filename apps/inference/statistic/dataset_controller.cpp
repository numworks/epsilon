#include "dataset_controller.h"

namespace Inference {

bool DatasetController::handleEvent(Ion::Events::Event event) {
  if (!cell(0)->canBeActivatedByEvent(event)) {
    return popFromStackViewControllerOnLeftEvent(event);
  }
  stackOpenPage(m_inputController);
  return true;
}

}  // namespace Inference

#include "categorical_type_controller.h"

#include <apps/i18n.h>
#include <escher/stack_view_controller.h>
#include <poincare/statistics/inference.h>

#include "../controller_container.h"
#include "inference/app.h"
#include "inference/controllers/inference_controller.h"

using namespace Inference;

CategoricalTypeController::CategoricalTypeController(
    Escher::StackViewController* parent, Chi2Test* inference,
    ControllerContainer* controllerContainer)
    : InferenceController(inference, controllerContainer),
      Escher::UniformSelectableListController<
          Escher::MenuCell<Escher::MessageTextView, Escher::EmptyCellWidget,
                           Escher::ChevronView>,
          k_numberOfCells>(parent) {
  selectRow(0);  // Select first row by default
  cell(k_indexOfGoodnessCell)
      ->label()
      ->setMessage(I18n::Message::GoodnessOfFit);
  cell(k_indexOfHomogeneityCell)
      ->label()
      ->setMessage(I18n::Message::Homogeneity);

  // Init selection
  selectRow(0);
}

void CategoricalTypeController::stackOpenPage(
    Escher::ViewController* nextPage) {
  selectRow(static_cast<int>(m_inferenceModel->categoricalType()));
  ViewController::stackOpenPage(nextPage);
}

bool CategoricalTypeController::handleEvent(Ion::Events::Event event) {
  // canBeActivatedByEvent can be called on any cell with chevron
  if (cell(0)->canBeActivatedByEvent(event)) {
    InputCategoricalController* controller = nullptr;
    CategoricalType type;
    switch (selectedRow()) {
      case k_indexOfGoodnessCell:
        controller = &m_controllerContainer->m_inputGoodnessController;
        type = CategoricalType::GoodnessOfFit;
        break;
      default:
        assert(selectedRow() == k_indexOfHomogeneityCell);
        controller = &m_controllerContainer->m_inputHomogeneityController;
        type = CategoricalType::Homogeneity;
        break;
    }
    assert(controller != nullptr);
    if (m_inferenceModel->initializeCategoricalType(type)) {
      controller->selectRow(0);
    }
    stackOpenPage(controller);
    return true;
  }
  return popFromStackViewControllerOnLeftEvent(event);
}

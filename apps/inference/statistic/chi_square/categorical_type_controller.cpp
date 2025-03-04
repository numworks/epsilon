#include "categorical_type_controller.h"

#include <apps/i18n.h>
#include <escher/stack_view_controller.h>
#include <poincare/statistics/inference.h>

#include "inference/app.h"
#include "input_goodness_controller.h"
#include "input_homogeneity_controller.h"

using namespace Inference;

CategoricalTypeController::CategoricalTypeController(
    Escher::StackViewController* parent, Chi2Test* statistic,
    InputGoodnessController* inputGoodnessController,
    InputHomogeneityController* inputHomogeneityController)
    : Escher::UniformSelectableListController<
          Escher::MenuCell<Escher::MessageTextView, Escher::EmptyCellWidget,
                           Escher::ChevronView>,
          k_numberOfCells>(parent),
      m_statistic(statistic),
      m_inputGoodnessController(inputGoodnessController),
      m_inputHomogeneityController(inputHomogeneityController) {
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
  selectRow(static_cast<int>(m_statistic->categoricalType()));
  ViewController::stackOpenPage(nextPage);
}

bool CategoricalTypeController::handleEvent(Ion::Events::Event event) {
  // canBeActivatedByEvent can be called on any cell with chevron
  if (cell(0)->canBeActivatedByEvent(event)) {
    InputCategoricalController* controller = nullptr;
    PcrInference::CategoricalType type;
    switch (selectedRow()) {
      case k_indexOfGoodnessCell:
        controller = m_inputGoodnessController;
        type = PcrInference::CategoricalType::GoodnessOfFit;
        break;
      default:
        assert(selectedRow() == k_indexOfHomogeneityCell);
        controller = m_inputHomogeneityController;
        type = PcrInference::CategoricalType::Homogeneity;
        break;
    }
    assert(controller != nullptr);
    if (m_statistic->initializeCategoricalType(type)) {
      controller->selectRow(0);
    }
    stackOpenPage(controller);
    return true;
  }
  return popFromStackViewControllerOnLeftEvent(event);
}

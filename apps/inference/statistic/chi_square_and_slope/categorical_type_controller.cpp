#include "categorical_type_controller.h"

#include <apps/i18n.h>
#include <escher/stack_view_controller.h>

#include "inference/app.h"
#include "input_goodness_controller.h"
#include "input_homogeneity_controller.h"

using namespace Inference;

CategoricalTypeController::CategoricalTypeController(
    Escher::StackViewController* parent, Chi2Test* statistic,
    InputGoodnessController* inputGoodnessController,
    InputHomogeneityController* inputHomogeneityController)
    : Escher::SelectableCellListPage<
          Escher::MenuCell<Escher::MessageTextView, Escher::EmptyCellWidget,
                           Escher::ChevronView>,
          k_numberOfCategoricalCells, Escher::RegularListViewDataSource>(
          parent),
      m_statistic(statistic),
      m_inputGoodnessController(inputGoodnessController),
      m_inputHomogeneityController(inputHomogeneityController) {
  selectRow(0);  // Select first row by default
  cellAtIndex(k_indexOfGoodnessCell)
      ->label()
      ->setMessage(I18n::Message::GoodnessOfFit);
  cellAtIndex(k_indexOfHomogeneityCell)
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
  if (cellAtIndex(0)->canBeActivatedByEvent(event)) {
    Escher::ViewController* controller = nullptr;
    CategoricalType type;
    switch (selectedRow()) {
      case k_indexOfGoodnessCell:
        controller = m_inputGoodnessController;
        type = CategoricalType::GoodnessOfFit;
        break;
      default:
        assert(selectedRow() == k_indexOfHomogeneityCell);
        controller = m_inputHomogeneityController;
        type = CategoricalType::Homogeneity;
        break;
    }
    assert(controller != nullptr);
    m_statistic->initializeCategoricalType(type);
    stackOpenPage(controller);
    return true;
  }
  return popFromStackViewControllerOnLeftEvent(event);
}

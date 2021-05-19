#include "categorical_type_controller.h"

#include <apps/i18n.h>
#include <escher/stack_view_controller.h>

#include "input_goodness_controller.h"
#include "input_homogeneity_controller.h"
#include "probability/app.h"
#include "probability/data.h"
#include "probability/gui/selectable_cell_list_controller.h"

using namespace Probability;

CategoricalTypeController::CategoricalTypeController(
    Escher::StackViewController * parent, InputGoodnessController * inputGoodnessController,
    InputHomogeneityController * inputHomogeneityController)
    : SelectableCellListPage(parent),
      m_inputGoodnessController(inputGoodnessController),
      m_inputHomogeneityController(inputHomogeneityController) {
  m_cells[k_indexOfGoodness].setMessage(I18n::Message::GoodnessOfFit);
  m_cells[k_indexOfHomogeneity].setMessage(I18n::Message::Homogeneity);
}

void CategoricalTypeController::didBecomeFirstResponder() {
  Probability::App::app()->snapshot()->navigation()->setPage(Data::Page::Categorical);
  // TODO factor out
  if (selectedRow() == -1) {
    selectCellAtLocation(0, 0);
  } else {
    selectCellAtLocation(selectedColumn(), selectedRow());
  }
  Escher::Container::activeApp()->setFirstResponder(&m_selectableTableView);
}

bool CategoricalTypeController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE || event == Ion::Events::Right) {
    Escher::ViewController * view;
    switch (selectedRow()) {
      case k_indexOfGoodness:
        view = m_inputGoodnessController;
        break;
      case k_indexOfHomogeneity:
        view = m_inputHomogeneityController;
        break;
    }
    assert(view != nullptr);
    openPage(view, true);
    return true;
  }
  return false;
}

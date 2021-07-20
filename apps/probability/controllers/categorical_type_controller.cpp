#include "categorical_type_controller.h"

#include <apps/i18n.h>
#include <escher/stack_view_controller.h>

#include <new>

#include "input_goodness_controller.h"
#include "input_homogeneity_controller.h"
#include "probability/app.h"
#include "probability/gui/selectable_cell_list_controller.h"
#include "probability/models/data.h"
#include "probability/models/statistic/goodness_statistic.h"
#include "probability/models/statistic/homogeneity_statistic.h"

using namespace Probability;

CategoricalTypeController::CategoricalTypeController(
    Escher::StackViewController * parent,
    Chi2Statistic * statistic,
    InputGoodnessController * inputGoodnessController,
    InputHomogeneityController * inputHomogeneityController) :
    SelectableCellListPage(parent),
    m_statistic(statistic),
    m_inputGoodnessController(inputGoodnessController),
    m_inputHomogeneityController(inputHomogeneityController) {
  m_cells[k_indexOfGoodnessCell].setMessage(I18n::Message::GoodnessOfFit);
  m_cells[k_indexOfHomogeneityCell].setMessage(I18n::Message::Homogeneity);
}

void CategoricalTypeController::didBecomeFirstResponder() {
  Probability::App::app()->setPage(Data::Page::Categorical);
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
    Data::CategoricalType type;
    switch (selectedRow()) {
      case k_indexOfGoodnessCell:
        view = m_inputGoodnessController;
        type = Data::CategoricalType::Goodness;
        break;
      case k_indexOfHomogeneityCell:
        view = m_inputHomogeneityController;
        type = Data::CategoricalType::Homogeneity;
        break;
    }
    assert(view != nullptr);
    if (type != App::app()->categoricalType()) {
      App::app()->setCategoricalType(type);
      Statistic::initializeStatistic(m_statistic,
                                     App::app()->test(),
                                     App::app()->testType(),
                                     App::app()->categoricalType());
    }
    openPage(view);
    return true;
  }
  return false;
}

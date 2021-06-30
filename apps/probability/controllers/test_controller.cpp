#include "test_controller.h"

#include <apps/i18n.h>
#include <assert.h>
#include <escher/container.h>
#include <escher/message_table_cell_with_chevron_and_message.h>
#include <escher/stack_view_controller.h>
#include <escher/view_controller.h>

#include "categorical_type_controller.h"
#include "hypothesis_controller.h"
#include "probability/app.h"
#include "probability/gui/selectable_cell_list_controller.h"
#include "probability/models/data.h"
#include "probability/models/statistic/one_proportion_statistic.h"
#include "probability/models/statistic/two_proportions_statistic.h"
#include "type_controller.h"

using namespace Probability;

TestController::TestController(Escher::StackViewController * parentResponder,
                               HypothesisController * hypothesisController,
                               TypeController * typeController,
                               CategoricalTypeController * categoricalController,
                               InputController * inputController, Data::Test * globalTest,
                               Data::TestType * globalTestType, Statistic * statistic) :
    SelectableCellListPage(parentResponder),
    m_hypothesisController(hypothesisController),
    m_typeController(typeController),
    m_inputController(inputController),
    m_categoricalController(categoricalController),
    m_globalTest(globalTest),
    m_globalTestType(globalTestType),
    m_statistic(statistic) {
  // Create cells
  m_cells[k_indexOfOneProp].setMessage(I18n::Message::TestOneProp);
  m_cells[k_indexOfOneProp].setSubtitle(I18n::Message::ZTest);
  m_cells[k_indexOfOneMean].setMessage(I18n::Message::TestOneMean);
  m_cells[k_indexOfOneMean].setSubtitle(I18n::Message::TOrZTest);
  m_cells[k_indexOfTwoProps].setMessage(I18n::Message::TestTwoProps);
  m_cells[k_indexOfTwoProps].setSubtitle(I18n::Message::ZTest);
  m_cells[k_indexOfTwoMeans].setMessage(I18n::Message::TestTwoMeans);
  m_cells[k_indexOfTwoMeans].setSubtitle(I18n::Message::TOrZTest);
  m_cells[k_indexOfCategorical].setMessage(I18n::Message::TestCategorical);
  m_cells[k_indexOfCategorical].setSubtitle(I18n::Message::X2Test);
}

const char * TestController::title() {
  return App::app()->subapp() == Data::SubApp::Tests ? "Test" : "Interval";
}

void TestController::didBecomeFirstResponder() {
  Probability::App::app()->setPage(Data::Page::Test);
  if (selectedRow() == -1 || selectedRow() >= numberOfRows()) {
    selectRow(0);
  }
  Escher::Container::activeApp()->setFirstResponder(&m_selectableTableView);
  resetMemoization();
  m_selectableTableView.reloadData();
}

bool TestController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE || event == Ion::Events::Right) {
    Data::SubApp subapp = App::app()->subapp();
    Escher::ViewController * view = nullptr;
    Data::Test test;
    switch (selectedRow()) {
      case k_indexOfOneProp:
        test = Data::Test::OneProp;
        if (subapp == Data::SubApp::Tests) {
          view = m_hypothesisController;
        } else {
          view = m_inputController;
        }
        break;
      case k_indexOfTwoProps:
        test = Data::Test::TwoProps;
        if (subapp == Data::SubApp::Tests) {
          view = m_hypothesisController;
        } else {
          view = m_inputController;
        }
        break;
      case k_indexOfOneMean:
        test = Data::Test::OneMean;
        view = m_typeController;
        break;
      case k_indexOfTwoMeans:
        test = Data::Test::TwoMeans;
        view = m_typeController;
        break;
      case k_indexOfCategorical:
        test = Data::Test::Categorical;
        view = m_categoricalController;
        break;
    }
    assert(view != nullptr);
    if (Data::isProportion(test) && (test != App::app()->test())) {
      Statistic::initializeStatistic(m_statistic, test, Data::TestType::ZTest);
      *m_globalTestType = Data::TestType::ZTest;
    }

    *m_globalTest = test;

    openPage(view);
    return true;
  }
  return false;
}

int TestController::numberOfRows() const {
  // Don't show Categorical cell for Interval
  return k_numberOfTestCells - (App::app()->subapp() == Data::SubApp::Intervals);
}

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
                               InputController * inputController) :
    SelectableCellListPage(parentResponder),
    m_hypothesisController(hypothesisController),
    m_typeController(typeController),
    m_inputController(inputController),
    m_categoricalController(categoricalController) {
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

void TestController::didBecomeFirstResponder() {
  Probability::App::app()->snapshot()->navigation()->setPage(Data::Page::Test);
  selectRowAccordingToTest(Probability::App::app()->snapshot()->data()->test());
  Escher::Container::activeApp()->setFirstResponder(&m_selectableTableView);
}

bool TestController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE || event == Ion::Events::Right) {
    Data::SubApp subapp = App::app()->snapshot()->navigation()->subapp();
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
    App::app()->snapshot()->data()->setTest(test);
    Data::TestType testType =
        Data::isProportion(test) ? Data::TestType::ZTest : Data::TestType::TTest;
    App::app()->snapshot()->data()->setTestType(testType);

    if (Data::isProportion(test)) {
      initializeStatistic(test);
    }
    initializeHypothesisParams(test);
    openPage(view);
    return true;
  }
  return false;
}

void TestController::selectRowAccordingToTest(Data::Test t) {
  // TODO this overrides the m_selectedRow behavior, is that needed ?
  int row = -1;
  switch (t) {
    case Data::Test::OneProp:
      row = k_indexOfOneProp;
      break;
    case Data::Test::TwoProps:
      row = k_indexOfTwoProps;
      break;
    case Data::Test::OneMean:
      row = k_indexOfOneMean;
      break;
    case Data::Test::TwoMeans:
      row = k_indexOfTwoMeans;
      break;
    case Data::Test::Categorical:
      row = k_indexOfCategorical;
      break;
    default:
      assert(false);
  }
  assert(row >= 0);
  selectRow(row);
}

void Probability::TestController::initializeStatistic(Data::Test t) {
  switch (t) {
    case Data::Test::OneProp:
      new (App::app()->snapshot()->data()->statistic()) OneProportionStatistic();
      break;
    case Data::Test::TwoProps:
      new (App::app()->snapshot()->data()->statistic()) TwoProportionsStatistic();
      break;
    default:
      assert(false);
      break;
  }
}

void TestController::initializeHypothesisParams(Data::Test t) {
  float firstParam;
  switch (t) {
    case Data::Test::OneProp:
      firstParam = 0.5;
      break;
    case Data::Test::OneMean:
      firstParam = 128;
      break;
    case Data::Test::TwoProps:
    case Data::Test::TwoMeans:
      firstParam = 0;
      break;
    default:
      return;
  }
  App::app()->snapshot()->data()->hypothesisParams()->setFirstParam(firstParam);
  App::app()->snapshot()->data()->hypothesisParams()->setOp(
      HypothesisParams::ComparisonOperator::Different);
}

#include "app.h"

#include <apps/shared/text_field_delegate_app.h>
#include <apps/exam_mode_configuration.h>

#include "models/statistic/homogeneity_statistic.h"
#include "probability_icon.h"

namespace Probability {

const Escher::Image * App::Descriptor::icon() const {
  return ImageStore::ProbabilityIcon;
}

App::App(Snapshot * snapshot) :
    TextFieldDelegateApp(snapshot, &m_stackViewController),
    m_statisticGraphController(&m_stackViewController, snapshot->data()->statistic()),
    m_homogeneityResultsController(
        &m_stackViewController,
        static_cast<HomogeneityStatistic *>(snapshot->data()->statistic()),
        &m_resultsController),
    m_inputHomogeneityController(
        &m_stackViewController,
        &m_homogeneityResultsController,
        this,
        static_cast<HomogeneityStatistic *>(snapshot->data()->statistic())),
    m_inputGoodnessController(&m_stackViewController,
                              &m_resultsController,
                              static_cast<GoodnessStatistic *>(snapshot->data()->statistic()),
                              this),
    m_resultsController(&m_stackViewController,
                        snapshot->data()->statistic(),
                        &m_statisticGraphController,
                        this,
                        this),
    m_inputController(&m_stackViewController,
                      &m_resultsController,
                      snapshot->data()->statistic(),
                      this),
    m_typeController(&m_stackViewController,
                     &m_hypothesisController,
                     &m_inputController,
                     snapshot->data()->testPointer(),
                     snapshot->data()->testTypePointer(),
                     snapshot->data()->statistic()),
    m_categoricalTypeController(&m_stackViewController,
                                static_cast<Chi2Statistic *>(snapshot->data()->statistic()),
                                snapshot->data()->categoricalTypePointer(),
                                &m_inputGoodnessController,
                                &m_inputHomogeneityController),
    m_hypothesisController(&m_stackViewController,
                           &m_inputController,
                           this,
                           snapshot->data()->statistic()),
    m_calculationController(&m_stackViewController,
                            this,
                            snapshot->data()->distribution(),
                            snapshot->data()->calculation()),
    m_parameterController(&m_stackViewController,
                          this,
                          snapshot->data()->distribution(),
                          &m_calculationController),
    m_distributionController(&m_stackViewController,
                             snapshot->data()->distribution(),
                             &m_parameterController),
    m_testController(&m_stackViewController,
                     &m_hypothesisController,
                     &m_typeController,
                     &m_categoricalTypeController,
                     &m_inputController,
                     snapshot->data()->testPointer(),
                     snapshot->data()->testTypePointer(),
                     snapshot->data()->categoricalTypePointer(),
                     snapshot->data()->statistic()),
    m_menuController(&m_stackViewController,
                     &m_distributionController,
                     &m_testController,
                     snapshot->data()->testPointer(),
                     snapshot->data()->testTypePointer(),
                     snapshot->data()->statistic(),
                     snapshot->data()->distribution(),
                     snapshot->data()->calculation()),
    m_stackViewController(&m_modalViewController, &m_menuController),
    m_bufferDestructor(nullptr)
{
}

void App::didBecomeActive(Window * windows) {
  // Reopen correct page
  // TODO delegate decisions to controllers somehow
  Data::Page page = snapshot()->navigation()->page();
  Data::SubApp subapp = snapshot()->navigation()->subapp();
  Data::Test test = snapshot()->data()->test();
  Data::TestType type = snapshot()->data()->testType();
  Data::CategoricalType categoricalType = snapshot()->data()->categoricalType();

  initTableSelections(page, subapp, test, type, categoricalType);
  switch (page) {
    case Data::Page::Menu:
      break;
    case Data::Page::Distribution:
      m_menuController.stackOpenPage(&m_distributionController, 0);
      break;
    case Data::Page::Parameters:
      m_menuController.stackOpenPage(&m_distributionController, 0);
      m_distributionController.stackOpenPage(&m_parameterController, 0);
      break;
    case Data::Page::ProbaGraph:
      m_menuController.stackOpenPage(&m_distributionController, 0);
      m_distributionController.stackOpenPage(&m_parameterController, 0);
      m_parameterController.stackOpenPage(&m_calculationController, 1);
      break;
    case Data::Page::Test:
      m_menuController.stackOpenPage(&m_testController, 0);
      break;
    case Data::Page::Type:
      m_menuController.stackOpenPage(&m_testController, 0);
      m_testController.stackOpenPage(&m_typeController, 0);
      break;
    case Data::Page::Hypothesis:
      m_menuController.stackOpenPage(&m_testController, 0);
      if (!Data::isProportion(test)) {
        m_testController.stackOpenPage(&m_typeController, 0);
        m_typeController.stackOpenPage(&m_hypothesisController, 0);
      } else {
        m_testController.stackOpenPage(&m_hypothesisController, 0);
      }
      break;
    case Data::Page::Categorical:
      m_menuController.stackOpenPage(&m_testController, 0);
      m_testController.stackOpenPage(&m_categoricalTypeController, 0);
      break;
    case Data::Page::Input:
      m_menuController.stackOpenPage(&m_testController, 0);
      if (subapp == Data::SubApp::Tests) {
        m_testController.stackOpenPage(&m_hypothesisController, 0);
        m_hypothesisController.stackOpenPage(&m_inputController, 1);
      } else {
        m_testController.stackOpenPage(&m_inputController, 0);
      }
      break;
    case Data::Page::InputGoodness:
      m_menuController.stackOpenPage(&m_testController, 0);
      m_testController.stackOpenPage(&m_categoricalTypeController, 0);
      m_categoricalTypeController.stackOpenPage(&m_inputGoodnessController, 0);
      break;
    case Data::Page::InputHomogeneity:
      m_menuController.stackOpenPage(&m_testController, 0);
      m_testController.stackOpenPage(&m_categoricalTypeController, 0);
      m_categoricalTypeController.stackOpenPage(&m_inputHomogeneityController, 0);
      break;
    case Data::Page::ResultsHomogeneity:
      m_menuController.stackOpenPage(&m_testController, 0);
      m_testController.stackOpenPage(&m_categoricalTypeController, 0);
      m_categoricalTypeController.stackOpenPage(&m_inputHomogeneityController, 0);
      m_inputHomogeneityController.stackOpenPage(&m_homogeneityResultsController, 0);
      break;
    case Data::Page::Results:
      m_menuController.stackOpenPage(&m_testController, 0);
      if (test == Data::Test::Categorical) {
        m_testController.stackOpenPage(&m_categoricalTypeController, 0);
        if (categoricalType == Data::CategoricalType::Goodness) {
          m_categoricalTypeController.stackOpenPage(&m_inputGoodnessController, 0);
          m_inputGoodnessController.stackOpenPage(&m_resultsController, 1);
        } else {
          m_categoricalTypeController.stackOpenPage(&m_inputHomogeneityController, 0);
          m_inputHomogeneityController.stackOpenPage(&m_homogeneityResultsController, 0);
          m_homogeneityResultsController.stackOpenPage(&m_resultsController, 1);
        }
      } else if (subapp == Data::SubApp::Intervals) {
        if (Data::isProportion(test)) {
          m_testController.stackOpenPage(&m_inputController, 0);
        } else {
          m_testController.stackOpenPage(&m_typeController, 0);
          m_typeController.stackOpenPage(&m_inputController, 0);
        }
        m_inputController.stackOpenPage(&m_resultsController, 1);
      } else {
        if (Data::isProportion(test)) {
          m_testController.stackOpenPage(&m_hypothesisController, 0);
        } else {
          m_testController.stackOpenPage(&m_typeController, 0);
          m_typeController.stackOpenPage(&m_hypothesisController, 0);
        }
        m_hypothesisController.stackOpenPage(&m_inputController, 1);
        m_inputController.stackOpenPage(&m_resultsController, 1);
      }
      break;
    case Data::Page::Graph:
      m_menuController.stackOpenPage(&m_testController, 0);
      if (test == Data::Test::Categorical) {
        m_testController.stackOpenPage(&m_categoricalTypeController, 0);
        if (categoricalType == Data::CategoricalType::Goodness) {
          m_categoricalTypeController.stackOpenPage(&m_inputGoodnessController, 0);
          m_inputGoodnessController.stackOpenPage(&m_resultsController, 0);
        } else {
          m_categoricalTypeController.stackOpenPage(&m_inputHomogeneityController, 0);
          m_inputHomogeneityController.stackOpenPage(&m_homogeneityResultsController, 0);
          m_homogeneityResultsController.stackOpenPage(&m_resultsController, 1);
        }
      } else if (subapp == Data::SubApp::Intervals) {
        if (Data::isProportion(test)) {
          m_testController.stackOpenPage(&m_inputController, 0);
        } else {
          m_testController.stackOpenPage(&m_typeController, 0);
          m_typeController.stackOpenPage(&m_inputController, 0);
        }
        m_inputController.stackOpenPage(&m_resultsController, 1);
      } else {
        if (Data::isProportion(test)) {
          m_testController.stackOpenPage(&m_hypothesisController, 0);
        } else {
          m_testController.stackOpenPage(&m_typeController, 0);
          m_typeController.stackOpenPage(&m_hypothesisController, 0);
        }
        m_hypothesisController.stackOpenPage(&m_inputController, 1);
        m_inputController.stackOpenPage(&m_resultsController, 1);
      }
      assert(!(subapp == Data::SubApp::Tests && ExamModeConfiguration::testsGraphResultsAreForbidden()));
      m_resultsController.stackOpenPage(&m_statisticGraphController, (test == Data::Test::Categorical ? 1 : 2));
      break;
  }
  Escher::App::didBecomeActive(windows);
}

void App::initTableSelections(Data::Page page,
                              Data::SubApp subapp,
                              Data::Test test,
                              Data::TestType type,
                              Data::CategoricalType categoricalType) {
  /* Set correct selection for list controllers
  when recreating App */
  if (page != Data::Page::Menu) {
    // Menu selection
    if (subapp == Data::SubApp::Probability) {
      m_menuController.selectRow(MenuController::k_indexOfProbability);
    } else if (subapp == Data::SubApp::Tests) {
      m_menuController.selectRow(MenuController::k_indexOfTest);
    } else if (subapp == Data::SubApp::Intervals) {
      m_menuController.selectRow(MenuController::k_indexOfInterval);
    }

    if (subapp != Data::SubApp::Probability && page != Data::Page::Test) {
      // Test selection
      switch (test) {
        case Data::Test::OneProp:
          m_testController.selectRow(TestController::k_indexOfOneProp);
          break;
        case Data::Test::OneMean:
          m_testController.selectRow(TestController::k_indexOfOneMean);
          break;
        case Data::Test::TwoProps:
          m_testController.selectRow(TestController::k_indexOfTwoProps);
          break;
        case Data::Test::TwoMeans:
          m_testController.selectRow(TestController::k_indexOfTwoMeans);
          break;
        case Data::Test::Categorical:
          m_testController.selectRow(TestController::k_indexOfCategorical);
          break;
        default:
          assert(false);
          break;
      }

      int row;
      if (page != Data::Page::Type &&
          (test == Data::Test::OneMean || test == Data::Test::TwoMeans)) {
        // Type selection
        switch (type) {
          case Data::TestType::TTest:
            m_typeController.selectRow(TypeController::k_indexOfTTest);
            break;
          case Data::TestType::ZTest:
            row = TypeController::k_indexOfZTest;
            if (test == Data::Test::OneMean) {
              row--;  // PooledTTest row is not shown
            }
            m_typeController.selectRow(row);
            break;
          case Data::TestType::PooledTTest:
            m_typeController.selectRow(TypeController::k_indexOfPooledTest);
            break;
          default:
            assert(false);
            break;
        }
      } else if (test == Data::Test::Categorical && page != Data::Page::Categorical) {
        m_categoricalTypeController.selectRow(
            categoricalType == Data::CategoricalType::Goodness
                ? CategoricalTypeController::k_indexOfGoodnessCell
                : CategoricalTypeController::k_indexOfHomogeneityCell);
      }
    }
  }
}

void App::cleanBuffer(DynamicCellsDataSourceDestructor * destructor) {
  assert(destructor);
  if (m_bufferDestructor) {
    m_bufferDestructor->destroyCells();
  }
  m_bufferDestructor = destructor;
}

const App::Descriptor * App::Snapshot::descriptor() const {
  static App::Descriptor s_descriptor;
  return &s_descriptor;
}

App::Snapshot::~Snapshot() {
  Data::SubApp subApp = navigation()->subapp();
  if (subApp == Data::SubApp::Probability) {
    m_data.distribution()->~Distribution();
    m_data.calculation()->~Calculation();
  } else if (subApp == Data::SubApp::Tests || subApp == Data::SubApp::Intervals) {
    m_data.statistic()->~Statistic();
  }
}

void App::Snapshot::tidy() {
  Data::SubApp subApp = navigation()->subapp();
  if (subApp == Data::SubApp::Tests || subApp == Data::SubApp::Intervals) {
    m_data.statistic()->tidy();
  }
}

void App::Snapshot::reset() {
  navigation()->setSubapp(Data::SubApp::Unset);
  navigation()->setPage(Data::Page::Menu);
}

}  // namespace Probability

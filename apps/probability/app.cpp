#include "app.h"

#include <apps/shared/text_field_delegate_app.h>

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
                     snapshot->navigation()->subappPointer(),
                     snapshot->data()->testPointer(),
                     snapshot->data()->testTypePointer(),
                     snapshot->data()->distribution(),
                     snapshot->data()->calculation()),
    m_stackViewController(&m_modalViewController, &m_menuController) {
  // Reopen correct page
  // TODO delegate decisions to controllers somehow
  Data::Page page = snapshot->navigation()->page();
  Data::SubApp subapp = snapshot->navigation()->subapp();
  Data::Test test = snapshot->data()->test();
  Data::CategoricalType categoricalType = snapshot->data()->categoricalType();

  // TODO open Parameters and Calculation too
  switch (page) {
    case Data::Page::Menu:
      break;
    case Data::Page::Distribution:
      m_menuController.openPage(&m_distributionController);
      break;
    case Data::Page::Test:
      m_menuController.openPage(&m_testController);
      break;
    case Data::Page::Type:
      m_menuController.openPage(&m_testController);
      m_testController.openPage(&m_typeController);
      break;
    case Data::Page::Hypothesis:
      m_menuController.openPage(&m_testController);
      if (!Data::isProportion(test)) {
        m_testController.openPage(&m_typeController);
        m_typeController.openPage(&m_hypothesisController);
      } else {
        m_testController.openPage(&m_hypothesisController);
      }
      break;
    case Data::Page::Categorical:
      m_menuController.openPage(&m_testController);
      m_testController.openPage(&m_categoricalTypeController);
      break;
    case Data::Page::IntervalInput:
      if (!Data::isProportion(test)) {
        m_testController.openPage(&m_typeController);
        m_typeController.openPage(&m_inputController);
      } else {
        m_testController.openPage(&m_inputController);
      }
      break;
    case Data::Page::Input:
      m_menuController.openPage(&m_testController);
      m_testController.openPage(&m_hypothesisController);
      m_hypothesisController.openPage(&m_inputController);
      break;
    case Data::Page::InputGoodness:
      m_menuController.openPage(&m_testController);
      m_testController.openPage(&m_categoricalTypeController);
      m_categoricalTypeController.openPage(&m_inputGoodnessController);
      break;
    case Data::Page::InputHomogeneity:
      m_menuController.openPage(&m_testController);
      m_testController.openPage(&m_categoricalTypeController);
      m_categoricalTypeController.openPage(&m_inputHomogeneityController);
      break;
    case Data::Page::ResultsHomogeneity:
      m_menuController.openPage(&m_testController);
      m_testController.openPage(&m_categoricalTypeController);
      m_categoricalTypeController.openPage(&m_inputHomogeneityController);
      m_inputHomogeneityController.openPage(&m_homogeneityResultsController);
      break;
    case Data::Page::Results:
      m_menuController.openPage(&m_testController);
      if (test == Data::Test::Categorical) {
        m_testController.openPage(&m_categoricalTypeController);
        if (categoricalType == Data::CategoricalType::Goodness) {
          m_categoricalTypeController.openPage(&m_inputGoodnessController);
          m_inputGoodnessController.openPage(&m_resultsController);
        } else {
          m_categoricalTypeController.openPage(&m_inputHomogeneityController);
          m_inputHomogeneityController.openPage(&m_homogeneityResultsController);
          m_homogeneityResultsController.openPage(&m_resultsController);
        }
      } else if (subapp == Data::SubApp::Intervals) {
        if (Data::isProportion(test)) {
          m_testController.openPage(&m_inputController);
        } else {
          m_testController.openPage(&m_typeController);
          m_typeController.openPage(&m_inputController);
        }
        // TODO open results from interval
      } else {
        if (Data::isProportion(test)) {
          m_testController.openPage(&m_hypothesisController);
        } else {
          m_testController.openPage(&m_typeController);
          m_typeController.openPage(&m_hypothesisController);
        }
        m_hypothesisController.openPage(&m_inputController);
        m_inputController.openPage(&m_resultsController);
      }
      break;
    case Data::Page::ProbaGraph:
      m_menuController.openPage(&m_distributionController);
      m_stackViewController.push(&m_calculationController);
      break;
    case Data::Page::Graph:
      m_menuController.openPage(&m_testController);
      m_testController.openPage(&m_hypothesisController);
      m_hypothesisController.openPage(&m_inputController);
      m_inputController.openPage(&m_resultsController);
      m_resultsController.openPage(&m_statisticGraphController);
      break;
  }
}

const App::Descriptor * App::Snapshot::descriptor() const {
  static App::Descriptor s_descriptor;
  return &s_descriptor;
};

}  // namespace Probability
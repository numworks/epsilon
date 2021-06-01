#include "app.h"

#include <apps/shared/text_field_delegate_app.h>
#include <escher/app.h>

#include "probability_icon.h"

namespace Probability {

const Escher::Image * App::Descriptor::icon() const { return ImageStore::ProbabilityIcon; }

App::App(Snapshot * snapshot) :
    TextFieldDelegateApp(snapshot, &m_stackViewController),
    m_graphController(&m_stackViewController),
    m_homogeneityResultsController(&m_stackViewController),
    m_inputHomogeneityController(&m_stackViewController, &m_homogeneityResultsController, this,
                                 this),
    m_inputGoodnessController(&m_stackViewController, &m_resultsController, this, this),
    m_resultsController(&m_stackViewController, &m_graphController, this, this),
    m_inputController(&m_stackViewController, &m_resultsController,
                      snapshot->data()->testInputParams(), this),
    m_typeController(&m_stackViewController, &m_hypothesisController, &m_intervalInputController),
    m_intervalInputController(&m_stackViewController, &m_resultsController, this),
    m_categoricalTypeController(&m_stackViewController, &m_inputGoodnessController,
                                &m_inputHomogeneityController),
    m_hypothesisController(&m_stackViewController, &m_inputController, this, this),
    m_calculationController(&m_stackViewController, this, snapshot->data()->distribution(),
                            snapshot->data()->calculation()),
    m_parameterController(&m_stackViewController, this, snapshot->data()->distribution(),
                          &m_calculationController),
    m_distributionController(&m_stackViewController, snapshot->data()->distribution(),
                             &m_parameterController),
    m_testController(&m_stackViewController, &m_hypothesisController, &m_typeController,
                     &m_categoricalTypeController, &m_intervalInputController),
    m_menuController(&m_stackViewController, &m_distributionController, &m_testController),
    m_stackViewController(&m_modalViewController, &m_menuController) {
  // Reopen correct page
  // TODO delegate decisions to controllers somehow
  Data::Page page = snapshot->navigation()->page();
  Data::SubApp subapp = snapshot->navigation()->subapp();
  Data::Test test = snapshot->data()->test();

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
        m_typeController.openPage(&m_intervalInputController);
      } else {
        m_testController.openPage(&m_intervalInputController);
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
        if (snapshot->data()->categoricalData()->m_type == Data::CategoricalType::Goodness) {
          m_categoricalTypeController.openPage(&m_inputGoodnessController);
          m_inputGoodnessController.openPage(&m_resultsController);
        } else {
          m_categoricalTypeController.openPage(&m_inputHomogeneityController);
          m_inputHomogeneityController.openPage(&m_homogeneityResultsController);
          m_homogeneityResultsController.openPage(&m_resultsController);
        }
      } else if (subapp == Data::SubApp::Intervals)
      {
        if (Data::isProportion(test)) {
          m_testController.openPage(&m_intervalInputController);
        } else {
          m_testController.openPage(&m_typeController);
          m_typeController.openPage(&m_intervalInputController);
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
      m_resultsController.openPage(&m_graphController);
      break;
  }
}

}  // namespace Probability
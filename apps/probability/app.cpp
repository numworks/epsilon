#include "app.h"

#include <apps/shared/text_field_delegate_app.h>
#include <escher/app.h>

#include "probability_icon.h"

namespace Probability {

const Escher::Image * App::Descriptor::icon() const { return ImageStore::ProbabilityIcon; }

App::App(Snapshot * snapshot)
    : TextFieldDelegateApp(snapshot, &m_stackViewController),
      m_graphController(&m_stackViewController),
      m_homogeneityResultsController(&m_stackViewController),
      m_inputHomogeneityController(&m_stackViewController, &m_homogeneityResultsController, this, this),
      m_inputGoodnessController(&m_stackViewController, &m_resultsController, this, this),
      m_resultsController(&m_stackViewController, &m_graphController, this, this),
      m_inputController(&m_stackViewController, &m_resultsController, this, this),
      m_typeController(&m_stackViewController, &m_hypothesisController),
      m_categoricalTypeController(&m_stackViewController, &m_inputGoodnessController, &m_inputHomogeneityController),
      m_hypothesisController(&m_stackViewController, &m_inputController, this, this),
      m_calculationController(&m_stackViewController, this, &m_distribution, &m_calculation),
      m_parameterController(&m_stackViewController, this, &m_distribution, &m_calculationController),
      m_distributionController(&m_stackViewController, &m_distribution, &m_parameterController),
      m_testController(&m_stackViewController, &m_hypothesisController, &m_typeController,
                       &m_categoricalTypeController),
      m_menuController(&m_stackViewController, &m_distributionController, &m_testController),
      m_stackViewController(&m_modalViewController, &m_menuController),
      m_calculation(&m_distribution) {
  switch (snapshot->navigation()->page())
  {
  case Data::Page::Menu:
    break;
  case Data::Page::Distribution:
    m_stackViewController.push(&m_distributionController);
    break;
  case Data::Page::Test:
    m_stackViewController.push(&m_testController);
    break;
  case Data::Page::Type:
    m_stackViewController.push(&m_typeController);
    break;
  case Data::Page::Hypothesis:
    m_stackViewController.push(&m_hypothesisController);
    break;
  case Data::Page::Categorical:
    m_stackViewController.push(&m_categoricalTypeController);
    break;
  case Data::Page::Input:
    m_stackViewController.push(&m_hypothesisController);
    m_stackViewController.push(&m_inputController);
    break;
  case Data::Page::InputGoodness:
    m_stackViewController.push(&m_inputGoodnessController);
    break;
  case Data::Page::InputHomogeneity:
    m_stackViewController.push(&m_inputHomogeneityController);
    break;
  case Data::Page::Results:
    m_stackViewController.push(&m_hypothesisController);
    m_stackViewController.push(&m_resultsController);
    break;
  case Data::Page::ResultsHomogeneity:
    m_stackViewController.push(&m_inputHomogeneityController);
    m_stackViewController.push(&m_homogeneityResultsController);
    break;
  case Data::Page::ProbaGraph:
    m_stackViewController.push(&m_calculationController);
    break;
  case Data::Page::Graph:
      m_stackViewController.push(&m_inputController);
      m_stackViewController.push(&m_graphController);
    break;
  }
}

}  // namespace Probability
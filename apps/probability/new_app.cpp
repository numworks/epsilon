#include "new_app.h"

#include <apps/shared/text_field_delegate_app.h>
#include <escher/app.h>

#include "probability_icon.h"

namespace Probability {

const Escher::Image * NewApp::Descriptor::icon() const { return ImageStore::ProbabilityIcon; }

NewApp::NewApp(Escher::App::Snapshot * snapshot)
    : TextFieldDelegateApp(snapshot, &m_stackViewController),
      m_inputController(&m_stackViewController, this, this),
      m_typeController(&m_stackViewController, &m_hypothesisController),
      m_categoricalTypeController(&m_stackViewController),
      m_hypothesisController(&m_stackViewController, &m_inputController, this, this),
      m_calculation(&m_distribution),
      m_calculationController(&m_stackViewController, this, &m_distribution, &m_calculation),
      m_parameterController(&m_stackViewController, this, &m_distribution, &m_calculationController),
      m_distributionController(&m_stackViewController, &m_distribution, &m_parameterController),
      m_testController(&m_stackViewController, &m_hypothesisController, &m_typeController,
                       &m_categoricalTypeController),
      m_menuController(&m_stackViewController, &m_distributionController, &m_testController),
      m_stackViewController(&m_modalViewController, &m_menuController) {}

}  // namespace Probability
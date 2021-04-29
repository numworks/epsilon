#include "new_app.h"
#include <escher/app.h>
#include <apps/shared/text_field_delegate_app.h>
#include "probability_icon.h"


namespace Probability {

const Escher::Image * NewApp::Descriptor::icon() const { return ImageStore::ProbabilityIcon; }

NewApp::NewApp(Escher::App::Snapshot * snapshot) :
  TextFieldDelegateApp(snapshot, &m_stackViewController),
  m_menuController(&m_stackViewController),
  m_stackViewController(&m_modalViewController, &m_menuController)
{ }

}
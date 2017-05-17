#include "app.h"
#include "../i18n.h"
#include "probability_icon.h"

using namespace Shared;

namespace Probability {

I18n::Message App::Descriptor::name() {
  return I18n::Message::ProbaApp;
}

I18n::Message App::Descriptor::upperName() {
  return I18n::Message::ProbaAppCapital;
}

const Image * App::Descriptor::icon() {
  return ImageStore::ProbabilityIcon;
}

App * App::Snapshot::unpack(Container * container) {
  return new App(container, this);
}

App::Descriptor * App::Snapshot::descriptor() {
  static Descriptor descriptor;
  return &descriptor;
}

App::App(Container * container, Snapshot * snapshot) :
  TextFieldDelegateApp(container, snapshot, &m_stackViewController),
  m_lawController(nullptr),
  m_stackViewController(&m_modalViewController, &m_lawController)
{
}

}

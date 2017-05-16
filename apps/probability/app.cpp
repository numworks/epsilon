#include "app.h"
#include "probability_icon.h"

using namespace Shared;

namespace Probability {

App * App::Descriptor::build(Container * container) {
  return new App(container, this);
}

I18n::Message App::Descriptor::name() {
  return I18n::Message::ProbaApp;
}

I18n::Message App::Descriptor::upperName() {
  return I18n::Message::ProbaAppCapital;
}

const Image * App::Descriptor::icon() {
  return ImageStore::ProbabilityIcon;
}

App::App(Container * container, Descriptor * descriptor) :
  TextFieldDelegateApp(container, &m_stackViewController, descriptor),
  m_lawController(nullptr),
  m_stackViewController(&m_modalViewController, &m_lawController)
{
}

App::Descriptor * App::buildDescriptor() {
  return new App::Descriptor();
}

}

#include "app.h"
#include "../i18n.h"
#include "probability_icon.h"
#include <new>

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

App::Snapshot::Snapshot() :
  m_law{}
{
  new(m_law) BinomialLaw();
}

App::Snapshot::~Snapshot() {
  law()->~Law();
}

App * App::Snapshot::unpack(Container * container) {
  return new App(container, this);
}

App::Descriptor * App::Snapshot::descriptor() {
  static Descriptor descriptor;
  return &descriptor;
}

Law * App::Snapshot::law() {
  return (Law *)m_law;
}

App::App(Container * container, Snapshot * snapshot) :
  TextFieldDelegateApp(container, snapshot, &m_stackViewController),
  m_lawController(nullptr, snapshot->law()),
  m_stackViewController(&m_modalViewController, &m_lawController)
{
}

}

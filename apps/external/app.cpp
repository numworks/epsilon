#include "app.h"
#include "external_icon.h"
#include <apps/i18n.h>

namespace External {

I18n::Message App::Descriptor::name() {
  return I18n::Message::ExternalApp;
}

I18n::Message App::Descriptor::upperName() {
  return I18n::Message::ExternalAppCapital;
}

App::Descriptor::ExaminationLevel App::Descriptor::examinationLevel() {
  return App::Descriptor::ExaminationLevel::Basic;
}

const Image * App::Descriptor::icon() {
  return ImageStore::ExternalIcon;
}

App * App::Snapshot::unpack(Container * container) {
  return new (container->currentAppBuffer()) App(this);
}

App::Descriptor * App::Snapshot::descriptor() {
  static Descriptor descriptor;
  return &descriptor;
}

void App::didBecomeActive(Window * window) {
  ::App::didBecomeActive(window);
  m_window = window;
}

void App::redraw() {
  m_window->redraw(true);
}

App::App(Snapshot * snapshot) :
  ::App(snapshot, &m_stackViewController),
  m_mainController(&m_stackViewController, this),
  m_stackViewController(&m_modalViewController, &m_mainController)
{
}

}

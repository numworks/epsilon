#include "app.h"
#include "../apps_container.h"
#include "code_icon.h"
#include "../i18n.h"
#include <assert.h>

namespace Code {

I18n::Message App::Descriptor::name() {
  return I18n::Message::CodeApp;
}

I18n::Message App::Descriptor::upperName() {
  return I18n::Message::CodeAppCapital;
}

const Image * App::Descriptor::icon() {
  return ImageStore::CodeIcon;
}

App * App::Snapshot::unpack(Container * container) {
  return new App(container, this);
}

App::Descriptor * App::Snapshot::descriptor() {
  static Descriptor descriptor;
  return &descriptor;
}

App::App(Container * container, Snapshot * snapshot) :
  ::App(container, snapshot, &m_menuController, I18n::Message::Warning),
  m_betaVersionController(I18n::Message::BetaVersion, I18n::Message::BetaVersionMessage1, I18n::Message::BetaVersionMessage2, I18n::Message::BetaVersionMessage3, I18n::Message::BetaVersionMessage4),
  m_menuController(this)
{
}

void App::didBecomeActive(Window * window) {
  ::App::didBecomeActive(window);
  displayModalViewController(&m_betaVersionController, 0.5f, 0.5f);
}

}

#include "app.h"
#include "apps/apps_container.h"
#include "code_icon.h"
#include "apps/i18n.h"
#include <assert.h>

namespace Code {

const char * App::Descriptor::uriName() {
  return "numworks.code";
}

const I18n::Message *App::Descriptor::name() {
  return &I18n::Common::CodeApp;
}

const I18n::Message *App::Descriptor::upperName() {
  return &I18n::Common::CodeAppCapital;
}

const Image * App::Descriptor::icon() {
  return ImageStore::CodeIcon;
}

App::Snapshot::Snapshot() :
  m_program()
{
}

App * App::Snapshot::unpack(Container * container) {
  return new App(container, this);
}

App::Descriptor * App::Snapshot::descriptor() {
  static Descriptor descriptor;
  return &descriptor;
}

void App::Snapshot::reset() {
  m_program.setContent("");
}

Program * App::Snapshot::program() {
  return &m_program;
}

static const I18n::Message *sCodeMessages[] = {&I18n::Common::BetaVersion, &I18n::Common::BetaVersionMessage1, &I18n::Common::BetaVersionMessage2, &I18n::Common::BetaVersionMessage3, &I18n::Common::BetaVersionMessage4};

static KDColor sCodeColors[] = {KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack};

App::App(Container * container, Snapshot * snapshot) :
  ::App(container, snapshot, &m_menuController, &I18n::Common::Warning),
  m_betaVersionController(sCodeMessages, sCodeColors),
  m_menuController(this, snapshot->program())
{
}

void App::didBecomeActive(Window * window) {
  ::App::didBecomeActive(window);
  displayModalViewController(&m_betaVersionController, 0.5f, 0.5f);
}

static App::Snapshot::Register r(new App::Snapshot());

}

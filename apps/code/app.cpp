#include "app.h"
#include "../apps_container.h"
#include "code_icon.h"
#include "../i18n.h"

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

static KDColor sCodeColors[] = {KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack};

App::App(Container * container, Snapshot * snapshot) :
  ::App(container, snapshot, &m_menuController, I18n::Message::Warning),
  m_menuController(this, snapshot->program())
{
}

}

#include "app.h"
#include "../apps_container.h"
#include <assert.h>

namespace Code {

I18n::Message App::Descriptor::name() {
  return I18n::Message::Matrices;
}

I18n::Message App::Descriptor::upperName() {
  return I18n::Message::Matrices;
}

App * App::Snapshot::unpack(Container * container) {
  return new App(container, this);
}

App::Descriptor * App::Snapshot::descriptor() {
  static Descriptor descriptor;
  return &descriptor;
}

App::App(Container * container, Snapshot * snapshot) :
  ::App(container, snapshot, &m_editorController),
  m_editorController(this)
{
}

}

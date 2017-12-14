#include "app.h"
#include "../i18n.h"

namespace ExpressionEditor {

I18n::Message App::Descriptor::name() {
  return I18n::Message::ExpressionEditorApp;
}

I18n::Message App::Descriptor::upperName() {
  return I18n::Message::ExpressionEditorAppCapital;
}

App * App::Snapshot::unpack(Container * container) {
  return new App(container, this);
}

App::Descriptor * App::Snapshot::descriptor() {
  static Descriptor descriptor;
  return &descriptor;
}

App::App(Container * container, Snapshot * snapshot) :
  ::App(container, snapshot, &m_controller),
  m_controller(&m_modalViewController, snapshot->expressionAndLayout()->expressionLayout())
{
}

}

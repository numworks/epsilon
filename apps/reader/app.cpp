#include "app.h"
#include "reader_icon.h"
#include "apps/apps_container.h"
#include "apps/i18n.h"


namespace reader {

I18n::Message App::Descriptor::name() {
  return I18n::Message::ReaderApp;
}

I18n::Message App::Descriptor::upperName() {
  return I18n::Message::ReaderAppCapital;
}

const Image * App::Descriptor::icon() {
  return ImageStore::ReaderIcon;
}


App * App::Snapshot::unpack(Container * container) {
  return new (container->currentAppBuffer()) App(this);
}

App::Descriptor * App::Snapshot::descriptor() {
  static Descriptor descriptor;
  return &descriptor;
}


App::App(Snapshot * snapshot) :
  ::App(snapshot, &m_stackViewController),
  m_listBookController(&m_stackViewController),
  m_stackViewController(nullptr, &m_listBookController)
{
}

}

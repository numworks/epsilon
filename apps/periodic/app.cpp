#include "app.h"
#include "elements_data_base.h"
#include "../apps_container.h"
#include "../settings/settings_icon.h"
#include <apps/i18n.h>
#include <escher/palette.h>

namespace Periodic {

// App::Descriptor

I18n::Message App::Descriptor::name() const {
  return I18n::Message::PeriodicApp;
}

I18n::Message App::Descriptor::upperName() const {
  return I18n::Message::PeriodicAppCapital;
}

const Escher::Image * App::Descriptor::icon() const {
  // TODO
  return ImageStore::SettingsIcon;
}

// App::Snapshot

App * App::Snapshot::unpack(Escher::Container * container) {
  return new (container->currentAppBuffer()) App(this);
}

const App::Descriptor * App::Snapshot::descriptor() const {
  constexpr static Descriptor s_descriptor;
  return &s_descriptor;
}

// TODO Remove
// App::PlaceholderController::ContentView

void App::PlaceholderController::ContentView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(rect, Escher::Palette::WallScreen);
  ctx->drawString(I18n::translate(ElementsDataBase::Name(1)), rect.topLeft().translatedBy(KDPoint(20,20)), KDFont::Size::Large, KDColorBlack, Escher::Palette::WallScreen);
}

// App
App::App(Snapshot * snapshot) :
  Shared::TextFieldDelegateApp(snapshot, &m_controller),
  m_controller(&m_modalViewController)
{}

}

#include "app.h"
#include "../apps_container.h"
#include "finance_icon_temp.h"
#include <apps/i18n.h>
#include <escher/palette.h>

namespace Finance {

// App::Descriptor

I18n::Message App::Descriptor::name() const {
  return I18n::Message::FinanceApp;
}

I18n::Message App::Descriptor::upperName() const {
  return I18n::Message::FinanceAppCapital;
}

const Escher::Image * App::Descriptor::icon() const {
  // TODO
  return ImageStore::FinanceIconTemp;
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
  ctx->drawString("Under construction", rect.topLeft().translatedBy(KDPoint(20,20)), KDFont::Size::Large, KDColorBlack, Escher::Palette::WallScreen);
}

// App
App::App(Snapshot * snapshot) :
  Shared::InputEventHandlerDelegateApp(snapshot, &m_controller),
  m_controller(&m_modalViewController)
{}

}
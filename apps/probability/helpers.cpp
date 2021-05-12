#include "helpers.h"

Shared::TextFieldDelegateApp * getProbabilityApp() {
  Escher::App * app = Escher::Container::activeApp();
  if (app->snapshot()->descriptor()->name() == I18n::Message::ProbaApp) {
    return static_cast<Shared::TextFieldDelegateApp *>(app);
  }
  assert(false);
  return nullptr;
}
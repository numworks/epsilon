#include "helpers.h"

#include <string.h>

#include "app.h"

Shared::TextFieldDelegateApp * getProbabilityApp() {
  Escher::App * app = Escher::Container::activeApp();
  if (app->snapshot()->descriptor()->name() == I18n::Message::ProbaApp) {
    return static_cast<Shared::TextFieldDelegateApp *>(app);
  }
  assert(false);
  return nullptr;
}

int Probability::testToText(Data::Test t, char * buffer, int bufferLength) {
  // TODO replace with messages
  const char * txt;
  switch (t) {
    case Data::Test::OneProp:
      txt = "one proportion";
      break;
    case Data::Test::OneMean:
      txt = "one mean";
      break;
    case Data::Test::TwoProps:
      txt = "two proportions";
      break;
    case Data::Test::TwoMeans:
      txt = "twt means";
      break;
  }
  assert(strlen(txt) < bufferLength);
  memcpy(buffer, txt, strlen(txt) + 1);

  return strlen(txt) + 1;
}

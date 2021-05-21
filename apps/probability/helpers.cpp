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
      txt = "two means";
      break;
  }
  assert(strlen(txt) < bufferLength);
  memcpy(buffer, txt, strlen(txt) + 1);

  return strlen(txt) + 1;
}


const char * Probability::testToTextSymbol(Data::Test t) {
  switch (t) {
    case Data::Test::OneProp:
      return "p";
    case Data::Test::OneMean:
      return "u";  // TODO mu
    case Data::Test::TwoProps:
      return "p1-p2";  // TODO subscript
    case Data::Test::TwoMeans:
      return "u1-u2";
  }
  assert(false);
  return nullptr;
}

int Probability::testTypeToText(Data::TestType t, char * buffer, int bufferLength) {
  const char * txt;
  switch (t) {
    case Data::TestType::TTest:
      txt = "t-test";
      break;
    case Data::TestType::PooledTTest:
      txt = "Pooled t-test";
      break;
    case Data::TestType::ZTest:
      txt = "z-test";
      break;
  }
  assert(strlen(txt) + 1 < bufferLength);
  memcpy(buffer, txt, strlen(txt) + 1);
  return strlen(txt) + 1;
}
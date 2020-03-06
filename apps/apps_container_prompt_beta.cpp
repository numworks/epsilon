#include "apps_container.h"

I18n::Message AppsContainer::k_promptMessages[] = {
  I18n::Message::BetaVersion,
  I18n::Message::BetaVersionMessage1,
  I18n::Message::BetaVersionMessage2,
  I18n::Message::BetaVersionMessage3,
  I18n::Message::BlankMessage,
  I18n::Message::BetaVersionMessage4,
  I18n::Message::BetaVersionMessage5,
  I18n::Message::BetaVersionMessage6};

KDColor AppsContainer::k_promptColors[] = {
  KDColorBlack,
  KDColorBlack,
  KDColorBlack,
  KDColorBlack,
  KDColorWhite,
  KDColorBlack,
  KDColorBlack,
  Palette::YellowDark};

int AppsContainer::k_promptNumberOfMessages = 8;

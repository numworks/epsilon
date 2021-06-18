#include "apps_container.h"

I18n::Message AppsContainer::k_promptMessages[] = {
  I18n::Message::UpdateAvailable,
  I18n::Message::UpdateMessage1,
  I18n::Message::UpdateMessage2,
  I18n::Message::BlankMessage,
  I18n::Message::UpdateMessage3,
  I18n::Message::UpdateMessage4};

KDColor AppsContainer::k_promptColors[] = {
  KDColorBlack,
  KDColorBlack,
  KDColorBlack,
  KDColorWhite,
  KDColorBlack,
  Palette::AccentText};

int AppsContainer::k_promptNumberOfMessages = 6;

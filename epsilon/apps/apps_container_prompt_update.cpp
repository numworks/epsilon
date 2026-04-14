#include "apps_container.h"

const I18n::Message AppsContainer::k_promptMessages[] = {
    I18n::Message::UpdateAvailable, I18n::Message::UpdateMessage1,
    I18n::Message::UpdateMessage2,  I18n::Message::BlankMessage,
    I18n::Message::UpdateMessage3,  I18n::Message::UpdateMessage4};

const Escher::ThemeComponent AppsContainer::k_promptColors[] = {
    Escher::ThemeComponent::TextDefault, Escher::ThemeComponent::TextDefault,
    Escher::ThemeComponent::TextDefault,
    // TODO: TextWhite
    Escher::ThemeComponent::TokenWhite,
    // TODO: TextYellowDark
    Escher::ThemeComponent::TextDefault, Escher::ThemeComponent::TokenYellow};

const int AppsContainer::k_promptNumberOfMessages = 6;

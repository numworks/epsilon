#include "apps_container.h"

const I18n::Message AppsContainer::k_promptMessages[] = {
    I18n::Message::BetaVersion,         I18n::Message::BetaVersionMessage1,
    I18n::Message::BetaVersionMessage2, I18n::Message::BetaVersionMessage3,
    I18n::Message::BlankMessage,        I18n::Message::BetaVersionMessage4,
    I18n::Message::BetaVersionMessage5, I18n::Message::BetaVersionMessage6};

const Escher::ThemeComponent AppsContainer::k_promptColors[] = {
    Escher::ThemeComponent::TextDefault,
    Escher::ThemeComponent::TextDefault,
    Escher::ThemeComponent::TextDefault,
    Escher::ThemeComponent::TextDefault,
    Escher::ThemeComponent::TokenWhite,
    Escher::ThemeComponent::TextDefault,
    Escher::ThemeComponent::TextDefault,
    Escher::ThemeComponent::TokenMainColor};

const int AppsContainer::k_promptNumberOfMessages = 8;

#include <apps/i18n.h>

#include "main_controller.h"

namespace Settings {

constexpr MessageTree s_modelMenu[] = {
    MessageTree(I18n::Message::AngleUnit, s_modelAngleChildren),
    MessageTree(I18n::Message::DisplayMode, s_modelFloatDisplayModeChildren),
    MessageTree(I18n::Message::EditionMode, s_modelEditionModeChildren),
    MessageTree(I18n::Message::ComplexFormat, s_modelComplexFormatChildren),
    MessageTree(I18n::Message::Brightness),
    MessageTree(I18n::Message::FontSizes, s_modelFontChildren),
    MessageTree(I18n::Message::Language),
    MessageTree(I18n::Message::Country),
    MessageTree(I18n::Message::ExamMode),
    MessageTree(I18n::Message::PressToTest),
    MessageTree(I18n::Message::TestMode, s_modelTestModeMenu),
    MessageTree(I18n::Message::BetaPopUp),
    MessageTree(I18n::Message::About, s_modelAboutChildren),
    MessageTree(I18n::Message::ResetCalculator)};

constexpr MessageTree s_model =
    MessageTree(I18n::Message::SettingsApp, s_modelMenu);

}  // namespace Settings

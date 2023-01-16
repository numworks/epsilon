#include "main_controller.h"
#include "../exam_mode_configuration.h"
#include <apps/i18n.h>

namespace Settings {

using namespace Shared;

constexpr SettingsMessageTree s_modelMenu[] =
  {SettingsMessageTree(I18n::Message::MathOptions, s_modelMathOptionsChildren),
    SettingsMessageTree(I18n::Message::BrightnessSettings, s_brightnessChildren),
    SettingsMessageTree(I18n::Message::DateTime, s_modelDateTimeChildren),
    SettingsMessageTree(I18n::Message::Language),
    SettingsMessageTree(I18n::Message::Country),
    SettingsMessageTree(I18n::Message::ExamMode, ExamModeConfiguration::s_modelExamChildren),
#ifdef HAS_CODE
    SettingsMessageTree(I18n::Message::CodeApp, s_codeChildren),
#endif
    SettingsMessageTree(I18n::Message::UpdatePopUp),
    SettingsMessageTree(I18n::Message::Accessibility, s_accessibilityChildren),
    SettingsMessageTree(I18n::Message::ExternalApps, s_externalChildren),
    SettingsMessageTree(I18n::Message::About, s_modelAboutChildren)};

constexpr SettingsMessageTree s_model = SettingsMessageTree(I18n::Message::SettingsApp, s_modelMenu);

I18n::Message MainController::promptMessage() const {
  return I18n::Message::UpdatePopUp;
}

}

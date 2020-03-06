#include "main_controller.h"
#include "../exam_mode_configuration.h"
#include <apps/i18n.h>

namespace Settings {

using namespace Shared;

constexpr SettingsMessageTree s_modelMenu[] =
  {SettingsMessageTree(I18n::Message::AngleUnit, s_modelAngleChildren),
    SettingsMessageTree(I18n::Message::DisplayMode, s_modelFloatDisplayModeChildren),
    SettingsMessageTree(I18n::Message::EditionMode, s_modelEditionModeChildren),
    SettingsMessageTree(I18n::Message::ComplexFormat, s_modelComplexFormatChildren),
    SettingsMessageTree(I18n::Message::Brightness),
    SettingsMessageTree(I18n::Message::FontSizes, s_modelFontChildren),
    SettingsMessageTree(I18n::Message::Language),
    SettingsMessageTree(I18n::Message::ExamMode, ExamModeConfiguration::s_modelExamChildren),
    SettingsMessageTree(I18n::Message::UpdatePopUp),
    SettingsMessageTree(I18n::Message::About, s_modelAboutChildren)};

constexpr SettingsMessageTree s_model = SettingsMessageTree(I18n::Message::SettingsApp, s_modelMenu);

I18n::Message MainController::promptMessage() const {
  return I18n::Message::UpdatePopUp;
}

}

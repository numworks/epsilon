#include "main_controller.h"
#include <apps/i18n.h>

namespace Settings {

constexpr SettingsMessageTree s_modelAngleChildren[3] = {SettingsMessageTree(I18n::Message::Degrees), SettingsMessageTree(I18n::Message::Radian), SettingsMessageTree(I18n::Message::Gradians)};
constexpr SettingsMessageTree s_modelEditionModeChildren[2] = {SettingsMessageTree(I18n::Message::Edition2D), SettingsMessageTree(I18n::Message::EditionLinear)};
constexpr SettingsMessageTree s_modelFloatDisplayModeChildren[4] = {SettingsMessageTree(I18n::Message::Decimal), SettingsMessageTree(I18n::Message::Scientific), SettingsMessageTree(I18n::Message::Engineering), SettingsMessageTree(I18n::Message::SignificantFigures)};
constexpr SettingsMessageTree s_modelComplexFormatChildren[3] = {SettingsMessageTree(I18n::Message::Real), SettingsMessageTree(I18n::Message::Cartesian), SettingsMessageTree(I18n::Message::Polar)};
constexpr SettingsMessageTree s_modelExamChildren[2] = {SettingsMessageTree(I18n::Message::ActivateExamMode), SettingsMessageTree(I18n::Message::ActivateDutchExamMode)};
constexpr SettingsMessageTree s_modelAboutChildren[3] = {SettingsMessageTree(I18n::Message::SoftwareVersion), SettingsMessageTree(I18n::Message::SerialNumber), SettingsMessageTree(I18n::Message::FccId)};

constexpr SettingsMessageTree s_modelMenu[] =
  {SettingsMessageTree(I18n::Message::AngleUnit, s_modelAngleChildren, 3),
    SettingsMessageTree(I18n::Message::DisplayMode, s_modelFloatDisplayModeChildren, 4),
    SettingsMessageTree(I18n::Message::EditionMode, s_modelEditionModeChildren, 2),
    SettingsMessageTree(I18n::Message::ComplexFormat, s_modelComplexFormatChildren, 3),
    SettingsMessageTree(I18n::Message::Brightness),
    SettingsMessageTree(I18n::Message::Language),
    SettingsMessageTree(I18n::Message::ExamMode, s_modelExamChildren, 2),
    SettingsMessageTree(I18n::Message::UpdatePopUp),
    SettingsMessageTree(I18n::Message::About, s_modelAboutChildren, 3)};

constexpr SettingsMessageTree s_model = SettingsMessageTree(I18n::Message::SettingsApp, s_modelMenu, 9);

I18n::Message MainController::promptMessage() const {
  return I18n::Message::UpdatePopUp;
}

const SettingsMessageTree * MainController::model() {
  return &s_model;
}

}

#include "main_controller.h"
#include <apps/i18n.h>

namespace Settings {

// TODO: factorize most parts of the final models with main_controller_prompt_beta and main_controller_prompt_update
//sub-sub-menus
constexpr SettingsMessageTree s_ledColorChildren[4] = {SettingsMessageTree(I18n::Message::ColorWhite), SettingsMessageTree(I18n::Message::ColorGreen), SettingsMessageTree(I18n::Message::ColorBlue), SettingsMessageTree(I18n::Message::ColorYellow)};
constexpr SettingsMessageTree s_examModeMode[3] = {SettingsMessageTree(I18n::Message::ExamModeModeStandard), SettingsMessageTree(I18n::Message::ExamModeModeNoSym), SettingsMessageTree(I18n::Message::ExamModeModeDutch)};
constexpr SettingsMessageTree s_contributorsChildren[7] = {SettingsMessageTree(I18n::Message::QuentinGuidee), SettingsMessageTree(I18n::Message::DannySimmons), SettingsMessageTree(I18n::Message::JoachimLeFournis), SettingsMessageTree(I18n::Message::JeanBaptisteBoric), SettingsMessageTree(I18n::Message::MaximeFriess), SettingsMessageTree(I18n::Message::David),SettingsMessageTree(I18n::Message::DamienNicolet)};
constexpr SettingsMessageTree s_modelAngleChildren[3] = {SettingsMessageTree(I18n::Message::Degrees), SettingsMessageTree(I18n::Message::Radian), SettingsMessageTree(I18n::Message::Gradians)};
constexpr SettingsMessageTree s_modelEditionModeChildren[2] = {SettingsMessageTree(I18n::Message::Edition2D), SettingsMessageTree(I18n::Message::EditionLinear)};
constexpr SettingsMessageTree s_modelFloatDisplayModeChildren[4] = {SettingsMessageTree(I18n::Message::Decimal), SettingsMessageTree(I18n::Message::Scientific), SettingsMessageTree(I18n::Message::Engineering), SettingsMessageTree(I18n::Message::SignificantFigures)};
constexpr SettingsMessageTree s_modelComplexFormatChildren[3] = {SettingsMessageTree(I18n::Message::Real), SettingsMessageTree(I18n::Message::Cartesian), SettingsMessageTree(I18n::Message::Polar)};
constexpr SettingsMessageTree s_symbolChildren[4] = {SettingsMessageTree(I18n::Message::SymbolMultiplicationCross),SettingsMessageTree(I18n::Message::SymbolMultiplicationMiddleDot),SettingsMessageTree(I18n::Message::SymbolMultiplicationStar),SettingsMessageTree(I18n::Message::SymbolMultiplicationAutoSymbol)};

//sub-menus
constexpr SettingsMessageTree s_modelMathOptionsChildren[5] = {SettingsMessageTree(I18n::Message::AngleUnit, s_modelAngleChildren, 3), SettingsMessageTree(I18n::Message::DisplayMode, s_modelFloatDisplayModeChildren, 4), SettingsMessageTree(I18n::Message::EditionMode, s_modelEditionModeChildren, 2), SettingsMessageTree(I18n::Message::ComplexFormat, s_modelComplexFormatChildren, 3), SettingsMessageTree(I18n::Message::SymbolMultiplication, s_symbolChildren, 4)};
constexpr SettingsMessageTree s_modelExamChildren[3] = {SettingsMessageTree(I18n::Message::LEDColor, s_ledColorChildren, 4), SettingsMessageTree(I18n::Message::ExamModeMode, s_examModeMode, 3), SettingsMessageTree(I18n::Message::ActivateExamMode)};
constexpr SettingsMessageTree s_pythonFont[2] = {SettingsMessageTree(I18n::Message::Large), SettingsMessageTree(I18n::Message::Small)};
constexpr SettingsMessageTree s_accessibilityChildren[6] = {SettingsMessageTree(I18n::Message::AccessibilityInvertColors), SettingsMessageTree(I18n::Message::AccessibilityMagnify),SettingsMessageTree(I18n::Message::AccessibilityGamma),SettingsMessageTree(I18n::Message::AccessibilityGammaRed),SettingsMessageTree(I18n::Message::AccessibilityGammaGreen),SettingsMessageTree(I18n::Message::AccessibilityGammaBlue)};
#ifdef USERNAME
constexpr SettingsMessageTree s_modelAboutChildren[7] = {SettingsMessageTree(I18n::Message::Username), SettingsMessageTree(I18n::Message::SoftwareVersion), SettingsMessageTree(I18n::Message::CustomSoftwareVersion), SettingsMessageTree(I18n::Message::MicroPythonVersion), SettingsMessageTree(I18n::Message::SerialNumber), SettingsMessageTree(I18n::Message::FccId), SettingsMessageTree(I18n::Message::Contributors, s_contributorsChildren, 7)};
#else
constexpr SettingsMessageTree s_modelAboutChildren[6] = {SettingsMessageTree(I18n::Message::SoftwareVersion), SettingsMessageTree(I18n::Message::CustomSoftwareVersion), SettingsMessageTree(I18n::Message::MicroPythonVersion), SettingsMessageTree(I18n::Message::SerialNumber), SettingsMessageTree(I18n::Message::FccId), SettingsMessageTree(I18n::Message::Contributors, s_contributorsChildren, 7)};
#endif

constexpr SettingsMessageTree s_modelMenu[] =
  {SettingsMessageTree(I18n::Message::MathOptions, s_modelMathOptionsChildren, 5),
    SettingsMessageTree(I18n::Message::Brightness),
    SettingsMessageTree(I18n::Message::Language),
    SettingsMessageTree(I18n::Message::ExamMode, s_modelExamChildren, 3),
    SettingsMessageTree(I18n::Message::PythonFont, s_pythonFont, 2),
    SettingsMessageTree(I18n::Message::Accessibility, s_accessibilityChildren, 6),
#ifdef USERNAME
    SettingsMessageTree(I18n::Message::About, s_modelAboutChildren, 7)};
#else
    SettingsMessageTree(I18n::Message::About, s_modelAboutChildren, 6)};
#endif

constexpr SettingsMessageTree s_model = SettingsMessageTree(I18n::Message::SettingsApp, s_modelMenu, 7);

I18n::Message MainController::promptMessage() const {
  return I18n::Message::Default;
}

const SettingsMessageTree * MainController::model() {
  return &s_model;
}

}

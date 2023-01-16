#include "main_controller.h"
#include "../global_preferences.h"
#include <apps/i18n.h>
#include <assert.h>
#include <ion/backlight.h>

using namespace Poincare;
using namespace Shared;

namespace Settings {

constexpr SettingsMessageTree s_modelAngleChildren[3] = {SettingsMessageTree(I18n::Message::Degrees), SettingsMessageTree(I18n::Message::Radian), SettingsMessageTree(I18n::Message::Gradians)};
constexpr SettingsMessageTree s_modelEditionModeChildren[2] = {SettingsMessageTree(I18n::Message::Edition2D), SettingsMessageTree(I18n::Message::EditionLinear)};
constexpr SettingsMessageTree s_modelFloatDisplayModeChildren[4] = {SettingsMessageTree(I18n::Message::Decimal), SettingsMessageTree(I18n::Message::Scientific), SettingsMessageTree(I18n::Message::Engineering), SettingsMessageTree(I18n::Message::SignificantFigures)};
constexpr SettingsMessageTree s_modelComplexFormatChildren[3] = {SettingsMessageTree(I18n::Message::Real), SettingsMessageTree(I18n::Message::Cartesian), SettingsMessageTree(I18n::Message::Polar)};
constexpr SettingsMessageTree s_modelDateTimeChildren[3] = {SettingsMessageTree(I18n::Message::ActivateClock), SettingsMessageTree(I18n::Message::Date), SettingsMessageTree(I18n::Message::Time)};
constexpr SettingsMessageTree s_symbolChildren[4] = {SettingsMessageTree(I18n::Message::SymbolMultiplicationCross),SettingsMessageTree(I18n::Message::SymbolMultiplicationMiddleDot),SettingsMessageTree(I18n::Message::SymbolMultiplicationStar),SettingsMessageTree(I18n::Message::SymbolMultiplicationAutoSymbol)};
constexpr SettingsMessageTree s_externalChildren[2] = {SettingsMessageTree(I18n::Message::ExtAppWrite), SettingsMessageTree(I18n::Message::ExtAppEnabled)};
constexpr SettingsMessageTree s_symbolFunctionChildren[3] = {SettingsMessageTree(I18n::Message::SymbolDefaultFunction), SettingsMessageTree(I18n::Message::SymbolArgDefaultFunction), SettingsMessageTree(I18n::Message::SymbolArgFunction)};
constexpr SettingsMessageTree s_modelMathOptionsChildren[6] = {SettingsMessageTree(I18n::Message::AngleUnit, s_modelAngleChildren), SettingsMessageTree(I18n::Message::DisplayMode, s_modelFloatDisplayModeChildren), SettingsMessageTree(I18n::Message::EditionMode, s_modelEditionModeChildren), SettingsMessageTree(I18n::Message::SymbolFunction, s_symbolFunctionChildren), SettingsMessageTree(I18n::Message::ComplexFormat, s_modelComplexFormatChildren), SettingsMessageTree(I18n::Message::SymbolMultiplication, s_symbolChildren)};
constexpr SettingsMessageTree s_brightnessChildren[4] = {SettingsMessageTree(I18n::Message::Brightness), SettingsMessageTree(I18n::Message::IdleTimeBeforeDimming), SettingsMessageTree(I18n::Message::IdleTimeBeforeSuspend), SettingsMessageTree(I18n::Message::BrightnessShortcut)};
constexpr SettingsMessageTree s_accessibilityChildren[6] = {SettingsMessageTree(I18n::Message::AccessibilityInvertColors), SettingsMessageTree(I18n::Message::AccessibilityMagnify),SettingsMessageTree(I18n::Message::AccessibilityGamma),SettingsMessageTree(I18n::Message::AccessibilityGammaRed),SettingsMessageTree(I18n::Message::AccessibilityGammaGreen),SettingsMessageTree(I18n::Message::AccessibilityGammaBlue)};
constexpr SettingsMessageTree s_contributorsChildren[18] = {SettingsMessageTree(I18n::Message::LaurianFournier), SettingsMessageTree(I18n::Message::YannCouturier), SettingsMessageTree(I18n::Message::DavidLuca), SettingsMessageTree(I18n::Message::LoicE), SettingsMessageTree(I18n::Message::VictorKretz), SettingsMessageTree(I18n::Message::QuentinGuidee), SettingsMessageTree(I18n::Message::JoachimLeFournis), SettingsMessageTree(I18n::Message::MaximeFriess), SettingsMessageTree(I18n::Message::JeanBaptisteBoric), SettingsMessageTree(I18n::Message::SandraSimmons), SettingsMessageTree(I18n::Message::David), SettingsMessageTree(I18n::Message::DamienNicolet), SettingsMessageTree(I18n::Message::EvannDreumont), SettingsMessageTree(I18n::Message::SzaboLevente), SettingsMessageTree(I18n::Message::VenceslasDuet), SettingsMessageTree(I18n::Message::CharlotteThomas), SettingsMessageTree(I18n::Message::AntoninLoubiere), SettingsMessageTree(I18n::Message::CyprienMejat)};

// Code Settings
#ifdef HAS_CODE
constexpr SettingsMessageTree s_codeChildren[3] = {SettingsMessageTree(I18n::Message::FontSizes, s_modelFontChildren), SettingsMessageTree(I18n::Message::Autocomplete), SettingsMessageTree(I18n::Message::SyntaxHighlighting)};
#endif
constexpr SettingsMessageTree s_modelFontChildren[2] = {SettingsMessageTree(I18n::Message::LargeFont), SettingsMessageTree(I18n::Message::SmallFont)};

constexpr SettingsMessageTree s_modelAboutChildren[10] = {SettingsMessageTree(I18n::Message::Username), SettingsMessageTree(I18n::Message::UpsilonVersion), SettingsMessageTree(I18n::Message::OmegaVersion), SettingsMessageTree(I18n::Message::SoftwareVersion), SettingsMessageTree(I18n::Message::MicroPythonVersion), SettingsMessageTree(I18n::Message::Battery), SettingsMessageTree(I18n::Message::MemUse), SettingsMessageTree(I18n::Message::SerialNumber), SettingsMessageTree(I18n::Message::FccId), SettingsMessageTree(I18n::Message::Contributors, s_contributorsChildren)};

MainController::MainController(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate) :
  ViewController(parentResponder),
  m_popUpCell(I18n::Message::Default, KDFont::LargeFont),
  m_selectableTableView(this),
  m_mathOptionsController(this, inputEventHandlerDelegate),
  m_brightnessController(this, inputEventHandlerDelegate),
  m_localizationController(this, Metric::CommonTopMargin, LocalizationController::Mode::Language),
  m_accessibilityController(this),
  m_dateTimeController(this),
  m_codeOptionsController(this),
  m_examModeController(this),
  m_aboutController(this),
  m_preferencesController(this),
  m_externalController(this)
{
  for (int i = 0; i < k_numberOfSimpleChevronCells; i++) {
    m_cells[i].setMessageFont(KDFont::LargeFont);
  }
}

View * MainController::view() {
  return &m_selectableTableView;
}

void MainController::didBecomeFirstResponder() {
  if (selectedRow() < 0) {
    selectCellAtLocation(0, 0);
  }
  Container::activeApp()->setFirstResponder(&m_selectableTableView);
}

bool MainController::handleEvent(Ion::Events::Event event) {
  GlobalPreferences * globalPreferences = GlobalPreferences::sharedGlobalPreferences();
  if (model()->childAtIndex(selectedRow())->numberOfChildren() == 0) {
    if (model()->childAtIndex(selectedRow())->label() == promptMessage()) {
      if (event == Ion::Events::OK || event == Ion::Events::EXE || event == Ion::Events::Right) {
        globalPreferences->setShowPopUp(!globalPreferences->showPopUp());
        m_selectableTableView.reloadCellAtLocation(m_selectableTableView.selectedColumn(), m_selectableTableView.selectedRow());
        return true;
      }
      return false;
    }
    if (model()->childAtIndex(selectedRow())->label() == I18n::Message::Language || model()->childAtIndex(selectedRow())->label() == I18n::Message::Country) {
      if (event == Ion::Events::OK || event == Ion::Events::EXE || event == Ion::Events::Right) {
        m_localizationController.setMode(model()->childAtIndex(selectedRow())->label() == I18n::Message::Language ? LocalizationController::Mode::Language : LocalizationController::Mode::Country);
        stackController()->push(&m_localizationController);
        return true;
      }
      return false;
    }
  }
  if (event == Ion::Events::OK || event == Ion::Events::EXE || event == Ion::Events::Right) {
    GenericSubController * subController = nullptr;
    I18n::Message title = model()->childAtIndex(selectedRow())->label();
    if (title == I18n::Message::Brightness || title == I18n::Message::Language) {
      assert(false);
    } else if (title == I18n::Message::ExamMode) {
      subController = &m_examModeController;
    } else if (title == I18n::Message::About) {
      subController = &m_aboutController;
    } else if (title == I18n::Message::BrightnessSettings) {
      subController = &m_brightnessController;
    } else if (title == I18n::Message::Accessibility) {
      subController = &m_accessibilityController;
    } else if (title == I18n::Message::DateTime) {
      subController = &m_dateTimeController;
    } else if (title == I18n::Message::MathOptions) {
      subController = &m_mathOptionsController;
    } else if (title == I18n::Message::CodeApp) {
      subController = &m_codeOptionsController;
    } else if (title == I18n::Message::ExternalApps) {
      subController = &m_externalController;
    } else {
      subController = &m_preferencesController;
    }
    subController->setMessageTreeModel(model()->childAtIndex(selectedRow()));
    StackViewController * stack = stackController();
    stack->push(subController);
    return true;
  }
  return false;
}

int MainController::numberOfRows() const {
  return model()->numberOfChildren();
};

KDCoordinate MainController::rowHeight(int j) {
  if (model()->childAtIndex(j)->label() == I18n::Message::Brightness) {
    return Metric::ParameterCellHeight + CellWithSeparator::k_margin;
  }
  return Metric::ParameterCellHeight;
}

KDCoordinate MainController::cumulatedHeightFromIndex(int j) {
  return j * rowHeight(0);
}

int MainController::indexFromCumulatedHeight(KDCoordinate offsetY) {
  if (offsetY < rowHeight(0)*k_indexOfBrightnessCell + CellWithSeparator::k_margin) {
    return offsetY/rowHeight(0);
  }
  return (offsetY - CellWithSeparator::k_margin)/rowHeight(0);
}

HighlightCell * MainController::reusableCell(int index, int type) {
  assert(index >= 0);
  if (type == 0) {
    assert(index < k_numberOfSimpleChevronCells);
    return &m_cells[index];
  }
  assert(index == 0);
  assert(type == 2);
  return &m_popUpCell;
}

int MainController::reusableCellCount(int type) {
  if (type == 0) {
    return k_numberOfSimpleChevronCells;
  }
  return 1;
}

int MainController::typeAtLocation(int i, int j) {
  if (model()->childAtIndex(j)->label() == I18n::Message::Brightness) {
    return 1;
  }
  if (model()->childAtIndex(j)->label() == I18n::Message::UpdatePopUp || model()->childAtIndex(j)->label() == I18n::Message::BetaPopUp) {
    return 2;
  }
  return 0;
}

void MainController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  GlobalPreferences * globalPreferences = GlobalPreferences::sharedGlobalPreferences();
  I18n::Message title = model()->childAtIndex(index)->label();
  if (model()->childAtIndex(index)->label() == I18n::Message::Brightness) {
    MessageTableCellWithGaugeWithSeparator * myGaugeCell = (MessageTableCellWithGaugeWithSeparator *)cell;
    myGaugeCell->setMessage(title);
    GaugeView * myGauge = (GaugeView *)myGaugeCell->accessoryView();
    myGauge->setLevel((float)globalPreferences->brightnessLevel()/(float)Ion::Backlight::MaxBrightness);
    return;
  }
  MessageTableCell<> * myCell = (MessageTableCell<> *)cell;
  myCell->setMessage(title);
  if (model()->childAtIndex(index)->label() == I18n::Message::Language) {
    int index = (int)(globalPreferences->language());
    static_cast<MessageTableCellWithChevronAndMessage *>(cell)->setSubtitle(I18n::LanguageNames[index]);
    return;
  }
  if (model()->childAtIndex(index)->label() == I18n::Message::Country) {
    int index = (int)(globalPreferences->country());
    static_cast<MessageTableCellWithChevronAndMessage *>(cell)->setSubtitle(I18n::CountryNames[index]);
    return;
  }
  if (model()->childAtIndex(index)->label() == I18n::Message::UpdatePopUp || model()->childAtIndex(index)->label() == I18n::Message::BetaPopUp) {
    MessageTableCellWithSwitch * mySwitchCell = (MessageTableCellWithSwitch *)cell;
    SwitchView * mySwitch = (SwitchView *)mySwitchCell->accessoryView();
    mySwitch->setState(globalPreferences->showPopUp());
    return;
  }
  MessageTableCellWithChevronAndMessage * myTextCell = (MessageTableCellWithChevronAndMessage *)cell;
  int childIndex = -1;
  switch (model()->childAtIndex(index)->label()) {
    default:
      break;
  }
  I18n::Message message = childIndex >= 0 ? model()->childAtIndex(index)->childAtIndex(childIndex)->label() : I18n::Message::Default;
  myTextCell->setSubtitle(message);
}

void MainController::viewWillAppear() {
  ViewController::viewWillAppear();
  m_selectableTableView.reloadData();
}

const SettingsMessageTree * MainController::model() {
  return &s_model;
}

StackViewController * MainController::stackController() const {
  return (StackViewController *)parentResponder();
}

}

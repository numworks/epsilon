#include "main_controller.h"
#include "../global_preferences.h"
#include <apps/i18n.h>
#include <assert.h>
#include <ion/backlight.h>

using namespace Poincare;

namespace Settings {

constexpr SettingsMessageTree s_modelAngleChildren[3] = {SettingsMessageTree(I18n::Message::Degrees), SettingsMessageTree(I18n::Message::Radian), SettingsMessageTree(I18n::Message::Gradians)};
constexpr SettingsMessageTree s_modelEditionModeChildren[2] = {SettingsMessageTree(I18n::Message::Edition2D), SettingsMessageTree(I18n::Message::EditionLinear)};
constexpr SettingsMessageTree s_modelFloatDisplayModeChildren[4] = {SettingsMessageTree(I18n::Message::Decimal), SettingsMessageTree(I18n::Message::Scientific), SettingsMessageTree(I18n::Message::Engineering), SettingsMessageTree(I18n::Message::SignificantFigures)};
constexpr SettingsMessageTree s_modelComplexFormatChildren[3] = {SettingsMessageTree(I18n::Message::Real), SettingsMessageTree(I18n::Message::Cartesian), SettingsMessageTree(I18n::Message::Polar)};
constexpr SettingsMessageTree s_modelFontChildren[2] = {SettingsMessageTree(I18n::Message::LargeFont), SettingsMessageTree(I18n::Message::SmallFont)};
constexpr SettingsMessageTree s_modelAboutChildren[3] = {SettingsMessageTree(I18n::Message::SoftwareVersion), SettingsMessageTree(I18n::Message::SerialNumber), SettingsMessageTree(I18n::Message::FccId)};

MainController::MainController(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate) :
  ViewController(parentResponder),
  m_brightnessCell(I18n::Message::Default, KDFont::LargeFont),
  m_popUpCell(I18n::Message::Default, KDFont::LargeFont),
  m_selectableTableView(this),
  m_mathOptionsController(this, inputEventHandlerDelegate),
  m_languageController(this, 13),
  m_accessibilityController(this),
  m_examModeController(this),
  m_aboutController(this),
  m_preferencesController(this)
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
  if (event == Ion::Events::BrightnessPlus || event == Ion::Events::BrightnessMinus){
    int delta = Ion::Backlight::MaxBrightness/GlobalPreferences::NumberOfBrightnessStates;
    int direction = (event == Ion::Events::BrightnessPlus) ? Ion::Backlight::NumberOfStepsPerShortcut*delta : -delta*Ion::Backlight::NumberOfStepsPerShortcut;
    GlobalPreferences::sharedGlobalPreferences()->setBrightnessLevel(GlobalPreferences::sharedGlobalPreferences()->brightnessLevel()+direction);
    m_selectableTableView.reloadCellAtLocation(m_selectableTableView.selectedColumn(), 1);
    return true;
  }
  if (model()->children(selectedRow())->numberOfChildren() == 0) {
    if (model()->children(selectedRow())->label() == promptMessage()) {
      if (event == Ion::Events::OK || event == Ion::Events::EXE) {
        globalPreferences->setShowPopUp(!globalPreferences->showPopUp());
        m_selectableTableView.reloadCellAtLocation(m_selectableTableView.selectedColumn(), m_selectableTableView.selectedRow());
        return true;
      }
      return false;
    }
    if (model()->children(selectedRow())->label() == I18n::Message::Brightness) {
      if (event == Ion::Events::Right || event == Ion::Events::Left || event == Ion::Events::Plus || event == Ion::Events::Minus) {
        int delta = Ion::Backlight::MaxBrightness/GlobalPreferences::NumberOfBrightnessStates;
        int direction = (event == Ion::Events::Right || event == Ion::Events::Plus) ? delta : -delta;
        globalPreferences->setBrightnessLevel(globalPreferences->brightnessLevel()+direction);
        m_selectableTableView.reloadCellAtLocation(m_selectableTableView.selectedColumn(), m_selectableTableView.selectedRow());
        return true;
      }
      return false;
    }
    if (model()->children(selectedRow())->label() == I18n::Message::Language) {
      if (event == Ion::Events::OK || event == Ion::Events::EXE || event == Ion::Events::Right) {
        stackController()->push(&m_languageController);
        return true;
      }
      return false;
    }
  }
  if (event == Ion::Events::OK || event == Ion::Events::EXE || event == Ion::Events::Right) {
    GenericSubController * subController = nullptr;
<<<<<<< HEAD
    if (model()->children(selectedRow())->label() == I18n::Message::Brightness || model()->children(selectedRow())->label() == I18n::Message::Language) {
      assert(false);
    } else if (model()->children(selectedRow())->label() == I18n::Message::ExamMode) {
      subController = &m_examModeController;
    } else if (model()->children(selectedRow())->label() == I18n::Message::About) {
=======
    int rowIndex = selectedRow();
    if (rowIndex == k_indexOfDisplayModeCell) {
      subController = &m_displayModeController;
    } else if (rowIndex == k_indexOfBrightnessCell || rowIndex == k_indexOfLanguageCell) {
      assert(false);
    } else if (rowIndex == k_indexOfExamModeCell) {
      subController = &m_examModeController;
    } else if (rowIndex == k_indexOfAboutCell + hasPrompt()) {
>>>>>>> upstream/master
      subController = &m_aboutController;
    } else if (model()->children(selectedRow())->label() == I18n::Message::Accessibility) {
      subController = &m_accessibilityController;
    } else if (model()->children(selectedRow())->label() == I18n::Message::MathOptions) {
      subController = &m_mathOptionsController;
    } else {
      subController = &m_preferencesController;
    }
    subController->setMessageTreeModel(model()->children(selectedRow()));
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
  if (j == k_indexOfBrightnessCell) {
    return Metric::ParameterCellHeight + CellWithSeparator::k_margin;
  }
  return Metric::ParameterCellHeight;
}

KDCoordinate MainController::cumulatedHeightFromIndex(int j) {
  KDCoordinate height = j * rowHeight(0);
  if (j > k_indexOfBrightnessCell) {
    height += CellWithSeparator::k_margin;
  }
  return height;
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
  if (type == 2) {
    return &m_popUpCell;
  }
  assert(type == 1);
  return &m_brightnessCell;
}

int MainController::reusableCellCount(int type) {
  if (type == 0) {
    return k_numberOfSimpleChevronCells;
  }
  return 1;
}

int MainController::typeAtLocation(int i, int j) {
<<<<<<< HEAD
  if (model()->children(j)->label() == I18n::Message::Brightness) {
    return 1;
  }
  if (model()->children(j)->label() == I18n::Message::UpdatePopUp || model()->children(j)->label() == I18n::Message::BetaPopUp) {
=======
  if (j == k_indexOfBrightnessCell) {
    return 1;
  }
  if (hasPrompt() && j == k_indexOfPopUpCell) {
>>>>>>> upstream/master
    return 2;
  }
  return 0;
}

void MainController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  GlobalPreferences * globalPreferences = GlobalPreferences::sharedGlobalPreferences();
  Preferences * preferences = Preferences::sharedPreferences();
<<<<<<< HEAD
  MessageTableCell * myCell = (MessageTableCell *)cell;
  I18n::Message thisLabel = model()->children(index)->label();
  myCell->setMessage(thisLabel);

  //switch to irregular cell types
  if (thisLabel == I18n::Message::Brightness) {
    MessageTableCellWithGauge * myGaugeCell = (MessageTableCellWithGauge *)cell;
=======
  I18n::Message title = model()->children(index)->label();
  if (index == k_indexOfBrightnessCell) {
    MessageTableCellWithGaugeWithSeparator * myGaugeCell = (MessageTableCellWithGaugeWithSeparator *)cell;
    myGaugeCell->setMessage(title);
>>>>>>> upstream/master
    GaugeView * myGauge = (GaugeView *)myGaugeCell->accessoryView();
    myGauge->setLevel((float)globalPreferences->brightnessLevel()/(float)Ion::Backlight::MaxBrightness);
    return;
  }
<<<<<<< HEAD
  if (thisLabel == I18n::Message::Language) {
=======
  MessageTableCell * myCell = (MessageTableCell *)cell;
  myCell->setMessage(title);
  if (index == k_indexOfLanguageCell) {
>>>>>>> upstream/master
    int index = (int)globalPreferences->language()-1;
    static_cast<MessageTableCellWithChevronAndMessage *>(cell)->setSubtitle(I18n::LanguageNames[index]);
    return;
  }
<<<<<<< HEAD
  if (thisLabel == I18n::Message::PythonFont) {
    int childIndex = (int)preferences->pythonFont();
    static_cast<MessageTableCellWithChevronAndMessage *>(cell)->setSubtitle(model()->children(index)->children(childIndex)->label());
    return;
  }
  if (hasPrompt() && (thisLabel == I18n::Message::UpdatePopUp || thisLabel == I18n::Message::BetaPopUp)) {
=======
  if (hasPrompt() && index == k_indexOfPopUpCell) {
>>>>>>> upstream/master
    MessageTableCellWithSwitch * mySwitchCell = (MessageTableCellWithSwitch *)cell;
    SwitchView * mySwitch = (SwitchView *)mySwitchCell->accessoryView();
    mySwitch->setState(globalPreferences->showPopUp());
    return;
  }
<<<<<<< HEAD
  static_cast<MessageTableCellWithChevronAndMessage *>(cell)->setSubtitle(I18n::Message::Default);
=======
  MessageTableCellWithChevronAndMessage * myTextCell = (MessageTableCellWithChevronAndMessage *)cell;
  int childIndex = -1;
  switch (index) {
    case k_indexOfAngleUnitCell:
      childIndex = (int)preferences->angleUnit();
      break;
    case k_indexOfDisplayModeCell:
      childIndex = (int)preferences->displayMode();
      break;
    case k_indexOfEditionModeCell:
      childIndex = (int)preferences->editionMode();
      break;
    case k_indexOfComplexFormatCell:
      childIndex = (int)preferences->complexFormat();
      break;
    case k_indexOfFontCell:
      childIndex = GlobalPreferences::sharedGlobalPreferences()->font() == KDFont::LargeFont ? 0 : 1;
      break;
  }
  I18n::Message message = childIndex >= 0 ? model()->children(index)->children(childIndex)->label() : I18n::Message::Default;
  myTextCell->setSubtitle(message);
>>>>>>> upstream/master
}

void MainController::viewWillAppear() {
  m_selectableTableView.reloadData();
}

const SettingsMessageTree * MainController::model() {
  return &s_model;
}

StackViewController * MainController::stackController() const {
  return (StackViewController *)parentResponder();
}

}

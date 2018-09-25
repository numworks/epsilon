#include "main_controller.h"
#include "../global_preferences.h"
#include "../i18n.h"
#include <assert.h>

using namespace Poincare;

namespace Settings {

const SettingsMessageTree angleChildren[2] = {SettingsMessageTree(I18n::Message::Degres), SettingsMessageTree(I18n::Message::Radian)};
const SettingsMessageTree editionModeChildren[2] = {SettingsMessageTree(I18n::Message::Edition2D), SettingsMessageTree(I18n::Message::EditionLinear)};
const SettingsMessageTree floatDisplayModeChildren[3] = {SettingsMessageTree(I18n::Message::Decimal), SettingsMessageTree(I18n::Message::Scientific), SettingsMessageTree(I18n::Message::SignificantFigures)};
const SettingsMessageTree complexFormatChildren[2] = {SettingsMessageTree(I18n::Message::Cartesian), SettingsMessageTree(I18n::Message::Polar)};
const SettingsMessageTree examChildren[1] = {SettingsMessageTree(I18n::Message::ActivateExamMode)};
const SettingsMessageTree aboutChildren[3] = {SettingsMessageTree(I18n::Message::SoftwareVersion), SettingsMessageTree(I18n::Message::SerialNumber), SettingsMessageTree(I18n::Message::FccId)};

#if EPSILON_SOFTWARE_UPDATE_PROMPT
const SettingsMessageTree menu[9] =
#else
const SettingsMessageTree menu[8] =
#endif
  {SettingsMessageTree(I18n::Message::AngleUnit, angleChildren, 2),
    SettingsMessageTree(I18n::Message::DisplayMode, floatDisplayModeChildren, 3),
    SettingsMessageTree(I18n::Message::EditionMode, editionModeChildren, 2),
    SettingsMessageTree(I18n::Message::ComplexFormat, complexFormatChildren, 2),
    SettingsMessageTree(I18n::Message::Brightness),
    SettingsMessageTree(I18n::Message::Language),
    SettingsMessageTree(I18n::Message::ExamMode, examChildren, 1),
#if EPSILON_SOFTWARE_UPDATE_PROMPT
  SettingsMessageTree(I18n::Message::UpdatePopUp),
#endif
  SettingsMessageTree(I18n::Message::About, aboutChildren, 3)};
#if EPSILON_SOFTWARE_UPDATE_PROMPT
const SettingsMessageTree model = SettingsMessageTree(I18n::Message::SettingsApp, menu, 9);
#else
const SettingsMessageTree model = SettingsMessageTree(I18n::Message::SettingsApp, menu, 8);
#endif

MainController::MainController(Responder * parentResponder) :
  ViewController(parentResponder),
#if EPSILON_SOFTWARE_UPDATE_PROMPT
  m_updateCell(I18n::Message::Default, KDText::FontSize::Large),
#endif
  m_brightnessCell(I18n::Message::Default, KDText::FontSize::Large),
  m_selectableTableView(this),
  m_messageTreeModel((MessageTree *)&model),
  m_preferencesController(this),
  m_displayModeController(this),
  m_languageController(this, 13),
  m_examModeController(this),
  m_aboutController(this)
{
  for (int i = 0; i < k_numberOfSimpleChevronCells; i++) {
    m_cells[i].setMessageFontSize(KDText::FontSize::Large);
  }
}

View * MainController::view() {
  return &m_selectableTableView;
}

void MainController::didBecomeFirstResponder() {
  if (selectedRow() < 0) {
    selectCellAtLocation(0, 0);
  }
  app()->setFirstResponder(&m_selectableTableView);
}

bool MainController::handleEvent(Ion::Events::Event event) {
  if (m_messageTreeModel->children(selectedRow())->numberOfChildren() == 0) {
#if EPSILON_SOFTWARE_UPDATE_PROMPT
    if (m_messageTreeModel->children(selectedRow())->label() == I18n::Message::UpdatePopUp) {
      if (event == Ion::Events::OK || event == Ion::Events::EXE) {
        GlobalPreferences::sharedGlobalPreferences()->setShowUpdatePopUp(!GlobalPreferences::sharedGlobalPreferences()->showUpdatePopUp());
        m_selectableTableView.reloadCellAtLocation(m_selectableTableView.selectedColumn(), m_selectableTableView.selectedRow());
        return true;
      }
      return false;
    }
#endif
    if (m_messageTreeModel->children(selectedRow())->label() == I18n::Message::Brightness) {
      if (event == Ion::Events::Right || event == Ion::Events::Left || event == Ion::Events::Plus || event == Ion::Events::Minus) {
        int delta = Ion::Backlight::MaxBrightness/GlobalPreferences::NumberOfBrightnessStates;
        int direction = (event == Ion::Events::Right || event == Ion::Events::Plus) ? delta : -delta;
        GlobalPreferences::sharedGlobalPreferences()->setBrightnessLevel(GlobalPreferences::sharedGlobalPreferences()->brightnessLevel()+direction);
        m_selectableTableView.reloadCellAtLocation(m_selectableTableView.selectedColumn(), m_selectableTableView.selectedRow());
        return true;
      }
      return false;
    }
    if (m_messageTreeModel->children(selectedRow())->label() == I18n::Message::Language) {
      if (event == Ion::Events::OK || event == Ion::Events::EXE || event == Ion::Events::Right) {
        stackController()->push(&m_languageController);
        return true;
      }
      return false;
    }
  }
  if (event == Ion::Events::OK || event == Ion::Events::EXE || event == Ion::Events::Right) {
    GenericSubController * subController = nullptr;
    switch (selectedRow()) {
      case 1:
        subController = &m_displayModeController;
        break;
      case 4:
      case 5:
        assert(false);
      case 6:
        subController = &m_examModeController;
        break;
#if EPSILON_SOFTWARE_UPDATE_PROMPT
      case 8:
#else
      case 7:
#endif
        subController = &m_aboutController;
        break;
      default:
        subController = &m_preferencesController;
    }
    subController->setMessageTreeModel(m_messageTreeModel->children(selectedRow()));
    StackViewController * stack = stackController();
    stack->push(subController);
    return true;
  }
  return false;
}

int MainController::numberOfRows() {
  return m_messageTreeModel->numberOfChildren();
};

KDCoordinate MainController::rowHeight(int j) {
  return Metric::ParameterCellHeight;
}

KDCoordinate MainController::cumulatedHeightFromIndex(int j) {
  return j*rowHeight(0);
}

int MainController::indexFromCumulatedHeight(KDCoordinate offsetY) {
  return offsetY/rowHeight(0);
}

HighlightCell * MainController::reusableCell(int index, int type) {
  assert(index >= 0);
  if (type == 0) {
    assert(index < k_numberOfSimpleChevronCells);
    return &m_cells[index];
  }
  assert(index == 0);
#if EPSILON_SOFTWARE_UPDATE_PROMPT
  if (type == 2) {
    return &m_updateCell;
  }
#endif
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
  if (j == 4) {
    return 1;
  }
#if EPSILON_SOFTWARE_UPDATE_PROMPT
  if (j == 7) {
    return 2;
  }
#endif
  return 0;
}

void MainController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  MessageTableCell * myCell = (MessageTableCell *)cell;
  myCell->setMessage(m_messageTreeModel->children(index)->label());
  if (index == 4) {
    MessageTableCellWithGauge * myGaugeCell = (MessageTableCellWithGauge *)cell;
    GaugeView * myGauge = (GaugeView *)myGaugeCell->accessoryView();
    myGauge->setLevel((float)GlobalPreferences::sharedGlobalPreferences()->brightnessLevel()/(float)Ion::Backlight::MaxBrightness);
    return;
  }
  if (index == 5) {
    int index = (int)GlobalPreferences::sharedGlobalPreferences()->language()-1;
    static_cast<MessageTableCellWithChevronAndMessage *>(cell)->setSubtitle(I18n::LanguageNames[index]);
    return;
  }
#if EPSILON_SOFTWARE_UPDATE_PROMPT
  if (index == 7) {
    MessageTableCellWithSwitch * mySwitchCell = (MessageTableCellWithSwitch *)cell;
    SwitchView * mySwitch = (SwitchView *)mySwitchCell->accessoryView();
    mySwitch->setState(GlobalPreferences::sharedGlobalPreferences()->showUpdatePopUp());
    return;
  }
#endif
  MessageTableCellWithChevronAndMessage * myTextCell = (MessageTableCellWithChevronAndMessage *)cell;
  int childIndex = -1;
  switch (index) {
    case 0:
      childIndex = (int)Preferences::sharedPreferences()->angleUnit();
      break;
    case 1:
      childIndex = (int)Preferences::sharedPreferences()->displayMode();
      break;
    case 2:
      childIndex = (int)Preferences::sharedPreferences()->editionMode();
      break;
    case 3:
      childIndex = (int)Preferences::sharedPreferences()->complexFormat();
      break;
  }
  I18n::Message message = childIndex >= 0 ? m_messageTreeModel->children(index)->children(childIndex)->label() : I18n::Message::Default;
  myTextCell->setSubtitle(message);
}

void MainController::viewWillAppear() {
  m_selectableTableView.reloadData();
}

StackViewController * MainController::stackController() const {
  return (StackViewController *)parentResponder();
}

}

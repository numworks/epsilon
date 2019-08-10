#include "main_controller.h"
#include "../global_preferences.h"
#include <apps/i18n.h>
#include <assert.h>

using namespace Poincare;

namespace Settings {

const SettingsMessageTree angleChildren[2] = {SettingsMessageTree(I18n::Message::Degres), SettingsMessageTree(I18n::Message::Radian)};
const SettingsMessageTree editionModeChildren[2] = {SettingsMessageTree(I18n::Message::Edition2D), SettingsMessageTree(I18n::Message::EditionLinear)};
const SettingsMessageTree floatDisplayModeChildren[3] = {SettingsMessageTree(I18n::Message::Decimal), SettingsMessageTree(I18n::Message::Scientific), SettingsMessageTree(I18n::Message::SignificantFigures)};
const SettingsMessageTree complexFormatChildren[3] = {SettingsMessageTree(I18n::Message::Real), SettingsMessageTree(I18n::Message::Cartesian), SettingsMessageTree(I18n::Message::Polar)};
const SettingsMessageTree examChildren[1] = {SettingsMessageTree(I18n::Message::ActivateExamMode)};
const SettingsMessageTree aboutChildren[4] = {SettingsMessageTree(I18n::Message::SoftwareVersion), SettingsMessageTree(I18n::Message::CustomSoftwareVersion), SettingsMessageTree(I18n::Message::SerialNumber), SettingsMessageTree(I18n::Message::FccId)};

#ifdef EPSILON_BOOT_PROMPT
const SettingsMessageTree menu[9] =
#else
const SettingsMessageTree menu[8] =
#endif
  {SettingsMessageTree(I18n::Message::AngleUnit, angleChildren, 2),
    SettingsMessageTree(I18n::Message::DisplayMode, floatDisplayModeChildren, 3),
    SettingsMessageTree(I18n::Message::EditionMode, editionModeChildren, 2),
    SettingsMessageTree(I18n::Message::ComplexFormat, complexFormatChildren, 3),
    SettingsMessageTree(I18n::Message::Brightness),
    SettingsMessageTree(I18n::Message::Language),
    SettingsMessageTree(I18n::Message::ExamMode, examChildren, 1),
#if EPSILON_BOOT_PROMPT == EPSILON_BETA_PROMPT
  SettingsMessageTree(I18n::Message::BetaPopUp),
#elif EPSILON_BOOT_PROMPT == EPSILON_UPDATE_PROMPT
  SettingsMessageTree(I18n::Message::UpdatePopUp),
#endif
  SettingsMessageTree(I18n::Message::About, aboutChildren, 4)};
#ifdef EPSILON_BOOT_PROMPT
const SettingsMessageTree model = SettingsMessageTree(I18n::Message::SettingsApp, menu, 9);
#else
const SettingsMessageTree model = SettingsMessageTree(I18n::Message::SettingsApp, menu, 8);
#endif

MainController::MainController(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate) :
  ViewController(parentResponder),
#ifdef EPSILON_BOOT_PROMPT
  m_popUpCell(I18n::Message::Default, KDFont::LargeFont),
#endif
  m_brightnessCell(I18n::Message::Default, KDFont::LargeFont),
  m_selectableTableView(this),
  m_messageTreeModel((MessageTree *)&model),
  m_preferencesController(this),
  m_displayModeController(this, inputEventHandlerDelegate),
  m_languageController(this, 13),
  m_examModeController(this),
  m_aboutController(this)
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
  app()->setFirstResponder(&m_selectableTableView);
}

bool MainController::handleEvent(Ion::Events::Event event) {
  GlobalPreferences * globalPreferences = GlobalPreferences::sharedGlobalPreferences();
  if (m_messageTreeModel->children(selectedRow())->numberOfChildren() == 0) {
#if EPSILON_BOOT_PROMPT == EPSILON_BETA_PROMPT
    if (m_messageTreeModel->children(selectedRow())->label() == I18n::Message::BetaPopUp) {
#elif EPSILON_BOOT_PROMPT == EPSILON_UPDATE_PROMPT
    if (m_messageTreeModel->children(selectedRow())->label() == I18n::Message::UpdatePopUp) {
#endif
#ifdef EPSILON_BOOT_PROMPT
      if (event == Ion::Events::OK || event == Ion::Events::EXE) {
        globalPreferences->setShowPopUp(!globalPreferences->showPopUp());
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
        globalPreferences->setBrightnessLevel(globalPreferences->brightnessLevel()+direction);
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
#ifdef EPSILON_BOOT_PROMPT
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
#ifdef EPSILON_BOOT_PROMPT
  if (type == 2) {
    return &m_popUpCell;
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
#ifdef EPSILON_BOOT_PROMPT
  if (j == 7) {
    return 2;
  }
#endif
  return 0;
}

void MainController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  GlobalPreferences * globalPreferences = GlobalPreferences::sharedGlobalPreferences();
  Preferences * preferences = Preferences::sharedPreferences();
  MessageTableCell * myCell = (MessageTableCell *)cell;
  myCell->setMessage(m_messageTreeModel->children(index)->label());
  if (index == 4) {
    MessageTableCellWithGauge * myGaugeCell = (MessageTableCellWithGauge *)cell;
    GaugeView * myGauge = (GaugeView *)myGaugeCell->accessoryView();
    myGauge->setLevel((float)globalPreferences->brightnessLevel()/(float)Ion::Backlight::MaxBrightness);
    return;
  }
  if (index == 5) {
    int index = (int)globalPreferences->language()-1;
    static_cast<MessageTableCellWithChevronAndMessage *>(cell)->setSubtitle(I18n::LanguageNames[index]);
    return;
  }
#ifdef EPSILON_BOOT_PROMPT
  if (index == 7) {
    MessageTableCellWithSwitch * mySwitchCell = (MessageTableCellWithSwitch *)cell;
    SwitchView * mySwitch = (SwitchView *)mySwitchCell->accessoryView();
    mySwitch->setState(globalPreferences->showPopUp());
    return;
  }
#endif
  MessageTableCellWithChevronAndMessage * myTextCell = (MessageTableCellWithChevronAndMessage *)cell;
  int childIndex = -1;
  switch (index) {
    case 0:
      childIndex = (int)preferences->angleUnit();
      break;
    case 1:
      childIndex = (int)preferences->displayMode();
      break;
    case 2:
      childIndex = (int)preferences->editionMode();
      break;
    case 3:
      childIndex = (int)preferences->complexFormat();
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

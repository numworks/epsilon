#include "main_controller.h"
#include "../global_preferences.h"
#include <apps/i18n.h>
#include <assert.h>

using namespace Poincare;
using namespace Shared;
using namespace Escher;

namespace Settings {

constexpr MessageTree s_modelAngleChildren[3] = {MessageTree(I18n::Message::Radian), MessageTree(I18n::Message::Degrees), MessageTree(I18n::Message::Gradians)};
constexpr MessageTree s_modelEditionModeChildren[2] = {MessageTree(I18n::Message::Edition2D), MessageTree(I18n::Message::EditionLinear)};
constexpr MessageTree s_modelExamChildren[2] = {MessageTree(I18n::Message::ActivateExamMode), MessageTree(I18n::Message::ActivateDutchExamMode)};
constexpr MessageTree s_modelFloatDisplayModeChildren[4] = {MessageTree(I18n::Message::Decimal), MessageTree(I18n::Message::Scientific), MessageTree(I18n::Message::Engineering), MessageTree(I18n::Message::SignificantFigures)};
constexpr MessageTree s_modelComplexFormatChildren[3] = {MessageTree(I18n::Message::Real), MessageTree(I18n::Message::Cartesian), MessageTree(I18n::Message::Polar)};
constexpr MessageTree s_modelFontChildren[2] = {MessageTree(I18n::Message::LargeFont), MessageTree(I18n::Message::SmallFont)};
constexpr MessageTree s_modelAboutChildren[3] = {MessageTree(I18n::Message::SoftwareVersion), MessageTree(I18n::Message::SerialNumber), MessageTree(I18n::Message::FccId)};

MainController::MainController(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate) :
  SelectableListViewController(parentResponder),
  m_brightnessCell(I18n::Message::Default),
  m_popUpCell(I18n::Message::Default),
  m_preferencesController(this),
  m_displayModeController(this, inputEventHandlerDelegate),
  m_localizationController(this, LocalizationController::Mode::Language),
  m_examModeController(this),
  m_aboutController(this)
{
}

void MainController::didBecomeFirstResponder() {
  if (selectedRow() < 0) {
    selectCellAtLocation(0, 0);
  }
  Container::activeApp()->setFirstResponder(&m_selectableTableView);
}

bool MainController::handleEvent(Ion::Events::Event event) {
  GlobalPreferences * globalPreferences = GlobalPreferences::sharedGlobalPreferences();
  int rowIndex = selectedRow();

  if (hasPrompt() && rowIndex == k_indexOfPopUpCell) {
    if (event == Ion::Events::OK || event == Ion::Events::EXE) {
      globalPreferences->setShowPopUp(!globalPreferences->showPopUp());
      m_selectableTableView.reloadCellAtLocation(m_selectableTableView.selectedColumn(), m_selectableTableView.selectedRow());
      return true;
    }
    return false;
  }

  if (rowIndex == k_indexOfBrightnessCell
   && (event == Ion::Events::Left || event == Ion::Events::Right || event == Ion::Events::Minus || event == Ion::Events::Plus)) {
    int delta = Ion::Backlight::MaxBrightness/GlobalPreferences::NumberOfBrightnessStates;
    int direction = (event == Ion::Events::Right || event == Ion::Events::Plus) ? delta : -delta;
    globalPreferences->setBrightnessLevel(globalPreferences->brightnessLevel()+direction);
    m_selectableTableView.reloadCellAtLocation(m_selectableTableView.selectedColumn(), m_selectableTableView.selectedRow());
    return true;
  }

  if (event == Ion::Events::OK || event == Ion::Events::EXE || event == Ion::Events::Right) {
    if (rowIndex == k_indexOfBrightnessCell) {
      /* Nothing is supposed to happen when OK or EXE are pressed on the
       * brightness cell. The case of pressing Right has been handled above. */
      return true;
    }

    if (rowIndex == k_indexOfLanguageCell) {
      m_localizationController.setMode(LocalizationController::Mode::Language);
    } else if (rowIndex == k_indexOfCountryCell) {
      m_localizationController.setMode(LocalizationController::Mode::Country);
    }
    /* The About cell can either be found at index k_indexOfExamModeCell + 1 or
     * k_indexOfExamModeCell + 2, depending on whether there is a Pop-Up cell.
     * Since the Pop-Up cell has been handled above, we can use those two
     * indices for the About cell. */
    ViewController * subControllers[k_indexOfAboutCell + 2] = {
      &m_preferencesController,
      &m_displayModeController,
      &m_preferencesController,
      &m_preferencesController,
      nullptr, //&m_brightnessController
      &m_preferencesController,
      &m_localizationController,
      &m_localizationController,
      &m_examModeController,
      &m_aboutController,
      &m_aboutController
    };
    ViewController * selectedSubController = subControllers[rowIndex];
    assert(selectedSubController);
    if (model()->childAtIndex(rowIndex)->numberOfChildren() != 0) {
      static_cast<GenericSubController *>(selectedSubController)->setMessageTreeModel(model()->childAtIndex(rowIndex));
      static_cast<GenericSubController *>(selectedSubController)->resetMemoization();
    }
    stackController()->push(selectedSubController);
    return true;
  }

  return false;
}

int MainController::numberOfRows() const {
  return model()->numberOfChildren();
};

KDCoordinate MainController::nonMemoizedRowHeight(int index) {
  if (index == k_indexOfBrightnessCell) {
    return heightForCellAtIndex(&m_brightnessCell, index);
  }
  if (hasPrompt() && index == k_indexOfPopUpCell) {
    return heightForCellAtIndex(&m_popUpCell, index);
  }
  MessageTableCellWithChevronAndMessage tempCell;
  return heightForCellAtIndex(&tempCell, index);
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

int MainController::typeAtIndex(int index) {
  if (index == k_indexOfBrightnessCell) {
    return 1;
  }
  if (hasPrompt() && index == k_indexOfPopUpCell) {
    return 2;
  }
  return 0;
}

void MainController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  GlobalPreferences * globalPreferences = GlobalPreferences::sharedGlobalPreferences();
  Preferences * preferences = Preferences::sharedPreferences();
  I18n::Message title = model()->childAtIndex(index)->label();
  if (index == k_indexOfBrightnessCell) {
    MessageTableCellWithGaugeWithSeparator * myGaugeCell = static_cast<MessageTableCellWithGaugeWithSeparator *>(cell);
    myGaugeCell->setMessage(title);
    GaugeView * myGauge = (GaugeView *)myGaugeCell->accessoryView();
    myGauge->setLevel((float)globalPreferences->brightnessLevel()/(float)Ion::Backlight::MaxBrightness);
    return;
  }
  MessageTableCell * myCell = static_cast<MessageTableCell *>(cell);
  myCell->setMessage(title);
  if (index == k_indexOfLanguageCell) {
    int index = (int)(globalPreferences->language());
    static_cast<MessageTableCellWithChevronAndMessage *>(cell)->setSubtitle(I18n::LanguageNames[index]);
    return;
  }
  if (index == k_indexOfCountryCell) {
    int index = (int)(globalPreferences->country());
    static_cast<MessageTableCellWithChevronAndMessage *>(cell)->setSubtitle(I18n::CountryNames[index]);
    return;
  }
  if (hasPrompt() && index == k_indexOfPopUpCell) {
    MessageTableCellWithSwitch * mySwitchCell = static_cast<MessageTableCellWithSwitch *>(cell);
    SwitchView * mySwitch = (SwitchView *)mySwitchCell->accessoryView();
    mySwitch->setState(globalPreferences->showPopUp());
    return;
  }
  MessageTableCellWithChevronAndMessage * myTextCell = static_cast<MessageTableCellWithChevronAndMessage *>(cell);
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
  I18n::Message message = childIndex >= 0 ? model()->childAtIndex(index)->childAtIndex(childIndex)->label() : I18n::Message::Default;
  myTextCell->setSubtitle(message);
}

void MainController::viewWillAppear() {
  ViewController::viewWillAppear();
  resetMemoization();
  m_selectableTableView.reloadData();
}

const MessageTree * MainController::model() {
  return &s_model;
}

StackViewController * MainController::stackController() const {
  return (StackViewController *)parentResponder();
}

}

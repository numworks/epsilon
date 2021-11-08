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
  m_resetButton(&m_selectableTableView, I18n::Message::ResetCalculator, Invocation([](void * context, void * sender) {
    static_cast<MainController *>(context)->m_resetController.presentModally();
    return true;
  }, this)),
  m_preferencesController(this),
  m_displayModeController(this, inputEventHandlerDelegate),
  m_localizationController(this, LocalizationController::Mode::Language),
  m_examModeController(this),
  m_aboutController(this),
  m_resetController(
    Invocation([](void * context, void * sender) {
      Ion::Reset::core();
      return true;
      }, this),
    {
      I18n::Message::ResetCalculatorWarning1,
      I18n::Message::ResetCalculatorWarning2,
      I18n::Message::ResetCalculatorWarning3,
    }
  )
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
  I18n::Message selectedMessage = messageAtIndex(selectedRow());

  if (selectedMessage == I18n::Message::ResetCalculator) {
    // Needed for event == Ion::Events::Right when the Reset button is selected
    return false;
  }

  if (selectedMessage == I18n::Message::UpdatePopUp || selectedMessage == I18n::Message::BetaPopUp) {
    if (event == Ion::Events::OK || event == Ion::Events::EXE) {
      globalPreferences->setShowPopUp(!globalPreferences->showPopUp());
      m_selectableTableView.reloadCellAtLocation(m_selectableTableView.selectedColumn(), m_selectableTableView.selectedRow());
      return true;
    }
    return false;
  }

  if (selectedMessage == I18n::Message::Brightness
   && (event == Ion::Events::Left || event == Ion::Events::Right || event == Ion::Events::Minus || event == Ion::Events::Plus)) {
    int delta = Ion::Backlight::MaxBrightness/GlobalPreferences::NumberOfBrightnessStates;
    int direction = (event == Ion::Events::Right || event == Ion::Events::Plus) ? delta : -delta;
    globalPreferences->setBrightnessLevel(globalPreferences->brightnessLevel()+direction);
    m_selectableTableView.reloadCellAtLocation(m_selectableTableView.selectedColumn(), m_selectableTableView.selectedRow());
    return true;
  }

  if (event == Ion::Events::OK || event == Ion::Events::EXE || event == Ion::Events::Right) {
    if (selectedMessage == I18n::Message::Brightness) {
      /* Nothing is supposed to happen when OK or EXE are pressed on the
       * brightness cell. The case of pressing Right has been handled above. */
      return true;
    }

    if (selectedMessage == I18n::Message::Language) {
      m_localizationController.setMode(LocalizationController::Mode::Language);
    } else if (selectedMessage == I18n::Message::Country) {
      m_localizationController.setMode(LocalizationController::Mode::Country);
    }


    ViewController * selectedSubController = subControllerForCell(selectedMessage);
    assert(selectedSubController);
    if (model()->childAtIndex(selectedRow())->numberOfChildren() != 0) {
      static_cast<GenericSubController *>(selectedSubController)->setMessageTreeModel(model()->childAtIndex(selectedRow()));
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
  switch (messageAtIndex(index)) {
    case I18n::Message::Brightness:
      return heightForCellAtIndex(&m_brightnessCell, index);
    case I18n::Message::UpdatePopUp:
    case  I18n::Message::BetaPopUp:
      return heightForCellAtIndex(&m_popUpCell, index);
    case I18n::Message::ResetCalculator:
      return heightForCellAtIndex(&m_resetButton, index);
    default:
      MessageTableCellWithChevronAndMessage tempCell;
      return heightForCellAtIndex(&tempCell, index);
  }
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
  if (type == 3) {
    return &m_resetButton;
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
  switch (messageAtIndex(index)) {
    case I18n::Message::Brightness:
      return 1;
    case I18n::Message::UpdatePopUp:
    case I18n::Message::BetaPopUp:
      return 2;
    case I18n::Message::ResetCalculator:
      return 3;
    default:
      return 0;
  };
  return 0;
}

void MainController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  GlobalPreferences * globalPreferences = GlobalPreferences::sharedGlobalPreferences();
  Preferences * preferences = Preferences::sharedPreferences();
  I18n::Message title = model()->childAtIndex(index)->label();
  I18n::Message message = messageAtIndex(index);
  if (message == I18n::Message::Brightness) {
    MessageTableCellWithGaugeWithSeparator * myGaugeCell = static_cast<MessageTableCellWithGaugeWithSeparator *>(cell);
    myGaugeCell->setMessage(title);
    GaugeView * myGauge = (GaugeView *)myGaugeCell->accessoryView();
    myGauge->setLevel((float)globalPreferences->brightnessLevel()/(float)Ion::Backlight::MaxBrightness);
    return;
  }
  if (message == I18n::Message::ResetCalculator) {
    return;
  }
  MessageTableCell * myCell = static_cast<MessageTableCell *>(cell);
  myCell->setMessage(title);
  if (message == I18n::Message::Language) {
    int index = (int)(globalPreferences->language());
    static_cast<MessageTableCellWithChevronAndMessage *>(cell)->setSubtitle(I18n::LanguageNames[index]);
    return;
  }
  if (message == I18n::Message::Country) {
    int index = (int)(globalPreferences->country());
    static_cast<MessageTableCellWithChevronAndMessage *>(cell)->setSubtitle(I18n::CountryNames[index]);
    return;
  }
  if (message == I18n::Message::UpdatePopUp || message == I18n::Message::BetaPopUp) {
    MessageTableCellWithSwitch * mySwitchCell = static_cast<MessageTableCellWithSwitch *>(cell);
    SwitchView * mySwitch = (SwitchView *)mySwitchCell->accessoryView();
    mySwitch->setState(globalPreferences->showPopUp());
    return;
  }
  MessageTableCellWithChevronAndMessage * myTextCell = static_cast<MessageTableCellWithChevronAndMessage *>(cell);
  int childIndex = -1;
  switch (message) {
    case I18n::Message::AngleUnit:
      childIndex = (int)preferences->angleUnit();
      break;
    case I18n::Message::DisplayMode:
      childIndex = (int)preferences->displayMode();
      break;
    case I18n::Message::EditionMode:
      childIndex = (int)preferences->editionMode();
      break;
    case I18n::Message::ComplexFormat:
      childIndex = (int)preferences->complexFormat();
      break;
    case I18n::Message::FontSizes:
      childIndex = GlobalPreferences::sharedGlobalPreferences()->font() == KDFont::LargeFont ? 0 : 1;
      break;
    default:
      childIndex = -1;
      break;
  }
  I18n::Message subtitle = childIndex >= 0 ? model()->childAtIndex(index)->childAtIndex(childIndex)->label() : I18n::Message::Default;
  myTextCell->setSubtitle(subtitle);
}

void MainController::viewWillAppear() {
  ViewController::viewWillAppear();
  resetMemoization();
  m_selectableTableView.reloadData();
}

I18n::Message MainController::messageAtIndex(int i) const {
  return model()->childAtIndex(i)->label();
}

const MessageTree * MainController::model() {
  return &s_model;
}

StackViewController * MainController::stackController() const {
  return (StackViewController *)parentResponder();
}

ViewController * MainController::subControllerForCell(I18n::Message cellMessage) {
  switch (cellMessage) {
    case I18n::Message::AngleUnit:
    case I18n::Message::EditionMode:
    case I18n::Message::ComplexFormat:
    case I18n::Message::FontSizes:
      return &m_preferencesController;
    case I18n::Message::DisplayMode:
      return &m_displayModeController;
    case I18n::Message::Language:
    case I18n::Message::Country:
      return &m_localizationController;
    case I18n::Message::ExamMode:
      return &m_examModeController;
    case I18n::Message::About:
      return &m_aboutController;
    default:
      return nullptr;
  }
}

}

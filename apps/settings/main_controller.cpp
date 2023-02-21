#include "main_controller.h"

#include <apps/i18n.h>
#include <assert.h>

#include "../exam_mode_configuration.h"
#include "../global_preferences.h"

using namespace Poincare;
using namespace Shared;
using namespace Escher;

namespace Settings {

constexpr MessageTree s_modelAngleChildren[3] = {
    MessageTree(I18n::Message::Radian), MessageTree(I18n::Message::Degrees),
    MessageTree(I18n::Message::Gradians)};
constexpr MessageTree s_modelEditionModeChildren[2] = {
    MessageTree(I18n::Message::Edition2D),
    MessageTree(I18n::Message::EditionLinear)};
constexpr MessageTree s_modelFloatDisplayModeChildren[4] = {
    MessageTree(I18n::Message::Decimal), MessageTree(I18n::Message::Scientific),
    MessageTree(I18n::Message::Engineering),
    MessageTree(I18n::Message::SignificantFigures)};
constexpr MessageTree s_modelComplexFormatChildren[3] = {
    MessageTree(I18n::Message::Real), MessageTree(I18n::Message::Cartesian),
    MessageTree(I18n::Message::Polar)};
constexpr MessageTree s_modelFontChildren[2] = {
    MessageTree(I18n::Message::LargeFont),
    MessageTree(I18n::Message::SmallFont)};
constexpr MessageTree s_modelTestModeMenu[2] = {
    MessageTree(I18n::Message::ExamMode),
    MessageTree(I18n::Message::PressToTest)};
constexpr MessageTree
    s_modelAboutChildren[AboutController::k_totalNumberOfCell] = {
        MessageTree(I18n::Message::SoftwareVersion),
        MessageTree(I18n::Message::SerialNumber),
        MessageTree(I18n::Message::FccId),
#if TERMS_OF_USE
        MessageTree(I18n::Message::TermsOfUse),
#endif
};

MainController::MainController(
    Responder *parentResponder,
    InputEventHandlerDelegate *inputEventHandlerDelegate)
    : SelectableListViewController(parentResponder),
      m_brightnessCell(I18n::Message::Default),
      m_popUpCell(I18n::Message::Default),
      m_resetButton(&m_selectableTableView, I18n::Message::ResetCalculator,
                    Invocation::Builder<MainController>(
                        [](MainController *controller, void *sender) {
                          controller->m_resetController.presentModally();
                          return true;
                        },
                        this)),
      m_preferencesController(this),
      m_displayModeController(this, inputEventHandlerDelegate),
      m_localizationController(this, LocalizationController::Mode::Language),
      m_examModeController(this),
      m_pressToTestController(this),
      m_testModeController(this, this),
      m_aboutController(this),
      m_resetController(Invocation::Builder<MainController>(
                            [](MainController *controller, void *sender) {
                              Ion::Reset::core();
                              return true;
                            },
                            this),
                        I18n::Message::ResetCalculatorWarning) {
  // Assert the ExamMode and Press-to-test cells are correctly placed.
  assert(subControllerForCell(messageAtModelIndex(k_indexOfExamModeCell)) ==
         &m_examModeController);
  assert(subControllerForCell(messageAtModelIndex(k_indexOfExamModeCell + 1)) ==
         &m_pressToTestController);
}

void MainController::didBecomeFirstResponder() {
  if (selectedRow() < 0) {
    selectCellAtLocation(0, 0);
  }
  Container::activeApp()->setFirstResponder(&m_selectableTableView);
}

bool MainController::handleEvent(Ion::Events::Event event) {
  GlobalPreferences *globalPreferences =
      GlobalPreferences::sharedGlobalPreferences;
  int type = typeAtIndex(selectedRow());

  if (type == k_resetCellType) {
    // Needed for event == Ion::Events::Right when the Reset button is selected
    return false;
  }

  if (type == k_popUpCellType) {
    if (event == Ion::Events::OK || event == Ion::Events::EXE) {
      globalPreferences->setShowPopUp(!globalPreferences->showPopUp());
      m_selectableTableView.reloadCellAtLocation(
          m_selectableTableView.selectedColumn(),
          m_selectableTableView.selectedRow());
      return true;
    }
    return false;
  }

  if (type == k_brightnessCellType &&
      (event == Ion::Events::Left || event == Ion::Events::Right ||
       event == Ion::Events::Minus || event == Ion::Events::Plus)) {
    int delta = Ion::Backlight::MaxBrightness /
                GlobalPreferences::NumberOfBrightnessStates;
    int direction = (event == Ion::Events::Right || event == Ion::Events::Plus)
                        ? delta
                        : -delta;
    globalPreferences->setBrightnessLevel(globalPreferences->brightnessLevel() +
                                          direction);
    m_selectableTableView.reloadCellAtLocation(
        m_selectableTableView.selectedColumn(),
        m_selectableTableView.selectedRow());
    return true;
  }

  if (event == Ion::Events::OK || event == Ion::Events::EXE ||
      event == Ion::Events::Right) {
    if (type == k_brightnessCellType) {
      /* Nothing is supposed to happen when OK or EXE are pressed on the
       * brightness cell. The case of pressing Right has been handled above. */
      return true;
    }
    assert(type == k_defaultCellType);
    int modelIndex = getModelIndex(selectedRow());
    I18n::Message selectedMessage = messageAtModelIndex(modelIndex);

    if (selectedMessage == I18n::Message::Language) {
      m_localizationController.setMode(LocalizationController::Mode::Language);
    } else if (selectedMessage == I18n::Message::Country) {
      m_localizationController.setMode(LocalizationController::Mode::Country);
    }
    pushModel(model()->childAtIndex(modelIndex));
    return true;
  }

  return false;
}

void MainController::pushModel(const Escher::MessageTree *messageTreeModel) {
  ViewController *selectedSubController =
      subControllerForCell(messageTreeModel->label());
  assert(selectedSubController);
  if (messageTreeModel->numberOfChildren() != 0) {
    static_cast<GenericSubController *>(selectedSubController)
        ->setMessageTreeModel(messageTreeModel);
    static_cast<GenericSubController *>(selectedSubController)
        ->resetMemoization();
  }
  stackController()->push(selectedSubController);
}

int MainController::numberOfRows() const {
  assert(hasExamModeCell() + hasPressToTestCell() + hasTestModeCell() == 1);
  return model()->numberOfChildren() + hasExamModeCell() +
         hasPressToTestCell() + hasTestModeCell() - 3;
};

KDCoordinate MainController::nonMemoizedRowHeight(int index) {
  switch (typeAtIndex(index)) {
    case k_brightnessCellType:
      return heightForCellAtIndex(&m_brightnessCell, index);
    case k_popUpCellType:
      return heightForCellAtIndex(&m_popUpCell, index);
    case k_resetCellType:
      return heightForCellAtIndex(&m_resetButton, index);
    default:
      MessageTableCellWithChevronAndMessage tempCell;
      return heightForCellAtIndexWithWidthInit(&tempCell, index);
  }
}

HighlightCell *MainController::reusableCell(int index, int type) {
  assert(index >= 0);
  if (type == k_defaultCellType) {
    assert(index < k_numberOfSimpleChevronCells);
    return &m_cells[index];
  }
  assert(index == 0);
  if (type == k_popUpCellType) {
    return &m_popUpCell;
  }
  if (type == k_resetCellType) {
    return &m_resetButton;
  }
  assert(type == k_brightnessCellType);
  return &m_brightnessCell;
}

int MainController::reusableCellCount(int type) {
  if (type == k_defaultCellType) {
    return k_numberOfSimpleChevronCells;
  }
  return 1;
}

int MainController::typeAtIndex(int index) const {
  switch (messageAtModelIndex(getModelIndex(index))) {
    case I18n::Message::Brightness:
      return k_brightnessCellType;
    case I18n::Message::UpdatePopUp:
    case I18n::Message::BetaPopUp:
      return k_popUpCellType;
    case I18n::Message::ResetCalculator:
      return k_resetCellType;
    default:
      return k_defaultCellType;
  };
}

void MainController::willDisplayCellForIndex(HighlightCell *cell, int index) {
  GlobalPreferences *globalPreferences =
      GlobalPreferences::sharedGlobalPreferences;
  Preferences *preferences = Preferences::sharedPreferences;
  int modelIndex = getModelIndex(index);
  I18n::Message title = model()->childAtIndex(modelIndex)->label();
  int type = typeAtIndex(index);
  if (type == k_brightnessCellType) {
    MessageTableCellWithGaugeWithSeparator *myGaugeCell =
        static_cast<MessageTableCellWithGaugeWithSeparator *>(cell);
    myGaugeCell->setMessage(title);
    GaugeView *myGauge = (GaugeView *)myGaugeCell->accessoryView();
    myGauge->setLevel((float)globalPreferences->brightnessLevel() /
                      (float)Ion::Backlight::MaxBrightness);
    return;
  }
  if (type == k_resetCellType) {
    return;
  }
  MessageTableCell *myCell = static_cast<MessageTableCell *>(cell);
  myCell->setMessage(title);
  if (type == k_popUpCellType) {
    MessageTableCellWithSwitch *mySwitchCell =
        static_cast<MessageTableCellWithSwitch *>(cell);
    mySwitchCell->setState(globalPreferences->showPopUp());
    return;
  }
  assert(type == k_defaultCellType);
  I18n::Message message = messageAtModelIndex(modelIndex);
  if (message == I18n::Message::Language) {
    int languageIndex = (int)(globalPreferences->language());
    static_cast<MessageTableCellWithChevronAndMessage *>(cell)->setSubtitle(
        I18n::LanguageNames[languageIndex]);
    return;
  }
  if (message == I18n::Message::Country) {
    int countryIndex = (int)(globalPreferences->country());
    static_cast<MessageTableCellWithChevronAndMessage *>(cell)->setSubtitle(
        I18n::CountryNames[countryIndex]);
    return;
  }
  MessageTableCellWithChevronAndMessage *myTextCell =
      static_cast<MessageTableCellWithChevronAndMessage *>(cell);
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
      childIndex = GlobalPreferences::sharedGlobalPreferences->font() ==
                           KDFont::Size::Large
                       ? 0
                       : 1;
      break;
    default:
      childIndex = -1;
      break;
  }
  I18n::Message subtitle =
      childIndex >= 0
          ? model()->childAtIndex(modelIndex)->childAtIndex(childIndex)->label()
          : I18n::Message::Default;
  myTextCell->setSubtitle(subtitle);
}

void MainController::viewWillAppear() {
  ViewController::viewWillAppear();
  resetMemoization();
  m_selectableTableView.reloadData();
}

I18n::Message MainController::messageAtModelIndex(int i) const {
  return model()->childAtIndex(i)->label();
}

const MessageTree *MainController::model() { return &s_model; }

StackViewController *MainController::stackController() const {
  return (StackViewController *)parentResponder();
}

ViewController *MainController::subControllerForCell(
    I18n::Message cellMessage) {
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
    case I18n::Message::PressToTest:
      return &m_pressToTestController;
    case I18n::Message::TestMode:
      return &m_testModeController;
    case I18n::Message::About:
      return &m_aboutController;
    default:
      return nullptr;
  }
}

bool MainController::hasExamModeCell() const {
  return !hasTestModeCell() &&
         ExamModeConfiguration::numberOfAvailableExamMode() > 0;
}

bool MainController::hasPressToTestCell() const {
  return !hasTestModeCell() &&
         ExamModeConfiguration::pressToTestExamModeAvailable();
}

bool MainController::hasTestModeCell() const {
  return ExamModeConfiguration::testModeAvailable();
}

int MainController::getModelIndex(int index) const {
  /* Return the index of the model from the index of the displayed row.
   * Up until k_indexOfExamModeCell, no cell is hidden, the index is the same.
   * Then, either the exam mode or the press-to-test cell is hidden. */
  assert(index >= 0 && index < numberOfRows());
  if (index > k_indexOfExamModeCell) {
    // 2 of the 3 exam mode cells are hidden.
    index += 2;
  } else if (index == k_indexOfExamModeCell) {
    if (!hasExamModeCell()) {
      // Hidden exam mode cell
      index += 1;
      if (!hasPressToTestCell()) {
        // Hidden press-to-test cell
        assert(hasTestModeCell());
        index += 1;
      }
    }
  }
  assert(index < model()->numberOfChildren());
  return index;
}

}  // namespace Settings

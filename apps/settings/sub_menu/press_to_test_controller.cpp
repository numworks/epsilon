#include "press_to_test_controller.h"

#include <apps/apps_container.h>
#include <apps/i18n.h>
#include <assert.h>
#include <escher/stack_view_controller.h>

#include <cmath>

#include "press_to_test_success.h"

using namespace Poincare;
using namespace Shared;
using namespace Escher;

namespace Settings {

PressToTestController::PressToTestController(Responder* parentResponder)
    : ListWithTopAndBottomController(parentResponder, &m_topMessageView,
                                     &m_bottomMessageView),
      m_topMessageView(I18n::Message::Default, k_messageFormat),
      m_bottomMessageView(I18n::Message::ToDeactivatePressToTest,
                          k_messageFormat),
      m_tempPressToTestParams{},
      m_activateButton(
          &m_selectableListView, I18n::Message::ActivateTestMode,
          Invocation::Builder<PressToTestController>(
              [](PressToTestController* controller, void* sender) {
                AppsContainer::sharedAppsContainer()->displayExamModePopUp(
                    ExamMode(ExamMode::Ruleset::PressToTest,
                             controller->getPressToTestParams()));
                return true;
              },
              this),
          ButtonCell::Style::EmbossedLight),
      m_confirmPopUpController(Invocation::Builder<PressToTestController>(
          [](PressToTestController* controller, void* sender) {
            controller->resetController();
            static_cast<StackViewController*>(controller->parentResponder())
                ->pop();
            return true;
          },
          this)) {
  for (int i = 0; i < k_numberOfReusableSwitchCells; i++) {
    m_switchCells[i].accessory()->setDisplayImage(false);
    m_switchCells[i].accessory()->imageView()->setImage(
        ImageStore::PressToTestSuccess);
    m_switchCells[i].accessory()->imageView()->setBackgroundColor(KDColorWhite);
  }
  resetController();
}

void PressToTestController::resetController() {
  selectFirstCell();
  if (Preferences::SharedPreferences()->examMode().isActive()) {
    // Reset switches states to press-to-test current parameter.
    m_tempPressToTestParams =
        Preferences::SharedPreferences()->examMode().flags();
  } else {
    // Reset switches so that all features are enabled.
    m_tempPressToTestParams = {};
  }
}

ExamMode::PressToTestFlags PressToTestController::getPressToTestParams() {
  return m_tempPressToTestParams;
}

KDCoordinate PressToTestController::nonMemoizedRowHeight(int row) {
  if (typeAtRow(row) == k_buttonCellType) {
    /* Do not call protectedNonMemoizedRowHeight since bounds can be empty (when
     * exam mode is on). Moreover, fillCellForRow does nothing for
     * m_activateButton. */
    return m_activateButton.minimalSizeForOptimalDisplay().height();
  }
  assert(typeAtRow(row) == k_switchCellType);
  PressToTestSwitch tempCell;
  return protectedNonMemoizedRowHeight(&tempCell, row);
}

void PressToTestController::setParamAtIndex(int index, bool value) {
  m_tempPressToTestParams.setFlag(static_cast<ExamMode::Flags>(index), value);
}

bool PressToTestController::getParamAtIndex(int index) {
  return m_tempPressToTestParams.getFlag(static_cast<ExamMode::Flags>(index));
}

void PressToTestController::setMessages() {
  if (Preferences::SharedPreferences()->examMode().isActive()) {
    assert(Preferences::SharedPreferences()->examMode().ruleset() ==
           ExamMode::Ruleset::PressToTest);
    m_topMessageView.setMessage(I18n::Message::PressToTestActiveIntro);
    setBottomView(&m_bottomMessageView);
  } else {
    m_topMessageView.setMessage(I18n::Message::PressToTestIntro);
    setBottomView(nullptr);
  }
}

bool PressToTestController::handleEvent(Ion::Events::Event event) {
  int row = innerSelectedRow();
  if (typeAtRow(row) == k_switchCellType &&
      static_cast<PressToTestSwitch*>(m_selectableListView.cell(selectedRow()))
          ->canBeActivatedByEvent(event) &&
      !Preferences::SharedPreferences()->examMode().isActive()) {
    assert(row >= 0 && row < k_numberOfSwitchCells);
    setParamAtIndex(row, !getParamAtIndex(row));
    /* Memoization isn't resetted here because changing a switch state does not
     * alter the cell's height. */
    m_selectableListView.reloadSelectedCell();
    return true;
  }
  if (event == Ion::Events::Left || event == Ion::Events::Back) {
    // Deselect table because select cell will change anyway
    m_selectableListView.deselectTable();
    if (!Preferences::SharedPreferences()->examMode().isActive() &&
        !(m_tempPressToTestParams == ExamMode::PressToTestFlags{})) {
      // Scroll to validation cell if m_confirmPopUpController is discarded.
      selectLastCell();
      // Open pop-up to confirm discarding values
      m_confirmPopUpController.presentModally();
    } else {
      resetController();
      static_cast<StackViewController*>(parentResponder())->pop();
    }
    return true;
  }
  return false;
}

void PressToTestController::viewWillAppear() {
  // Reset selection and params only if exam mode has been activated.
  if (Preferences::SharedPreferences()->examMode().isActive()) {
    resetController();
  }
  setMessages();
  ListWithTopAndBottomController::viewWillAppear();
}

int PressToTestController::numberOfRows() const {
  return k_numberOfSwitchCells +
         (Preferences::SharedPreferences()->examMode().isActive() ? 0 : 1);
}

int PressToTestController::typeAtRow(int row) const {
  assert(row >= 0 && row <= k_numberOfSwitchCells);
  return row < k_numberOfSwitchCells ? k_switchCellType : k_buttonCellType;
}

HighlightCell* PressToTestController::reusableCell(int index, int type) {
  if (type == k_buttonCellType) {
    assert(index == 0);
    return &m_activateButton;
  }
  assert(type == k_switchCellType);
  assert(index >= 0 && index < k_numberOfReusableSwitchCells);
  return &m_switchCells[index];
}

int PressToTestController::reusableCellCount(int type) const {
  return type == k_buttonCellType ? 1 : k_numberOfReusableSwitchCells;
}

void PressToTestController::fillCellForRow(HighlightCell* cell, int row) {
  if (typeAtRow(row) == k_buttonCellType) {
    assert(!Preferences::SharedPreferences()->examMode().isActive());
    return;
  }
  assert(typeAtRow(row) == k_switchCellType);
  PressToTestSwitch* myCell = static_cast<PressToTestSwitch*>(cell);
  // A true params means the feature is disabled,
  bool featureIsDisabled = getParamAtIndex(row);
  myCell->label()->setMessage(LabelAtIndex(row));
  myCell->label()->setTextColor(
      Preferences::SharedPreferences()->examMode().isActive() &&
              featureIsDisabled
          ? Palette::GrayDark
          : KDColorBlack);
  myCell->subLabel()->setMessage(SubLabelAtIndex(row));
  // Switch is toggled if the feature must stay activated.
  myCell->accessory()->switchView()->setState(!featureIsDisabled);
  myCell->accessory()->setDisplayImage(
      Preferences::SharedPreferences()->examMode().isActive());
}

I18n::Message PressToTestController::LabelAtIndex(int i) {
  assert(i >= 0 && i < k_numberOfSwitchCells);
  constexpr I18n::Message labels[k_numberOfSwitchCells] = {
      I18n::Message::PressToTestExactResults,
      I18n::Message::PressToTestEquationSolver,
      I18n::Message::PressToTestInequalityGraphing,
      I18n::Message::PressToTestImplicitPlots,
      I18n::Message::PressToTestGraphDetails,
      I18n::Message::PressToTestElements,
      I18n::Message::PressToTestStatDiagnostics,
      I18n::Message::PressToTestVectors,
      I18n::Message::PressToTestLogBaseA,
      I18n::Message::PressToTestSum};
  return labels[i];
}

I18n::Message PressToTestController::SubLabelAtIndex(int i) {
  switch (LabelAtIndex(i)) {
    case I18n::Message::PressToTestStatDiagnostics:
      return I18n::Message::PressToTestStatDiagnosticsDescription;
    case I18n::Message::PressToTestVectors:
      return I18n::Message::PressToTestVectorsDescription;
    default:
      return I18n::Message::Default;
  }
}

}  // namespace Settings

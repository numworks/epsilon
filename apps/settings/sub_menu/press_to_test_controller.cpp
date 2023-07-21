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

PressToTestController::PressToTestController(Responder *parentResponder)
    : ListWithTopAndBottomController(parentResponder, &m_topMessageView,
                                     &m_bottomMessageView),
      m_topMessageView(I18n::Message::Default, k_messageFormat),
      m_bottomMessageView(I18n::Message::ToDeactivatePressToTest,
                          k_messageFormat),
      m_tempPressToTestParams{},
      m_activateButton(
          &m_selectableListView, I18n::Message::ActivateTestMode,
          Invocation::Builder<PressToTestController>(
              [](PressToTestController *controller, void *sender) {
                AppsContainer::sharedAppsContainer()->displayExamModePopUp(
                    ExamMode(ExamMode::Ruleset::PressToTest,
                             controller->getPressToTestParams()));
                return true;
              },
              this)),
      m_confirmPopUpController(Invocation::Builder<PressToTestController>(
          [](PressToTestController *controller, void *sender) {
            controller->resetController();
            static_cast<StackViewController *>(controller->parentResponder())
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
  if (Preferences::sharedPreferences->examMode().isActive()) {
    // Reset switches states to press-to-test current parameter.
    m_tempPressToTestParams =
        Preferences::sharedPreferences->examMode().flags();
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
  switch (LabelAtIndex(index)) {
    case I18n::Message::PressToTestExactResults:
      m_tempPressToTestParams.forbidExactResults = value;
      break;
    case I18n::Message::PressToTestEquationSolver:
      m_tempPressToTestParams.forbidEquationSolver = value;
      break;
    case I18n::Message::PressToTestInequalityGraphing:
      m_tempPressToTestParams.forbidInequalityGraphing = value;
      break;
    case I18n::Message::PressToTestImplicitPlots:
      m_tempPressToTestParams.forbidImplicitPlots = value;
      break;
    case I18n::Message::PressToTestGraphDetails:
      m_tempPressToTestParams.forbidGraphDetails = value;
      break;
    case I18n::Message::PressToTestElements:
      m_tempPressToTestParams.forbidElementsApp = value;
      break;
    case I18n::Message::PressToTestStatDiagnostics:
      m_tempPressToTestParams.forbidStatsDiagnostics = value;
      break;
    case I18n::Message::PressToTestVectors:
      m_tempPressToTestParams.forbidVectors = value;
      break;
    case I18n::Message::PressToTestLogBaseA:
      m_tempPressToTestParams.forbidBasedLogarithm = value;
      break;
    case I18n::Message::PressToTestSum:
      m_tempPressToTestParams.forbidSum = value;
      break;
    default:
      assert(false);
  }
}

bool PressToTestController::getParamAtIndex(int index) {
  switch (LabelAtIndex(index)) {
    case I18n::Message::PressToTestExactResults:
      return m_tempPressToTestParams.forbidExactResults;
    case I18n::Message::PressToTestEquationSolver:
      return m_tempPressToTestParams.forbidEquationSolver;
    case I18n::Message::PressToTestInequalityGraphing:
      return m_tempPressToTestParams.forbidInequalityGraphing;
    case I18n::Message::PressToTestImplicitPlots:
      return m_tempPressToTestParams.forbidImplicitPlots;
    case I18n::Message::PressToTestGraphDetails:
      return m_tempPressToTestParams.forbidGraphDetails;
    case I18n::Message::PressToTestElements:
      return m_tempPressToTestParams.forbidElementsApp;
    case I18n::Message::PressToTestStatDiagnostics:
      return m_tempPressToTestParams.forbidStatsDiagnostics;
    case I18n::Message::PressToTestVectors:
      return m_tempPressToTestParams.forbidVectors;
    case I18n::Message::PressToTestLogBaseA:
      return m_tempPressToTestParams.forbidBasedLogarithm;
    case I18n::Message::PressToTestSum:
      return m_tempPressToTestParams.forbidSum;
    default:
      assert(false);
      return false;
  }
}

void PressToTestController::setMessages() {
  if (Preferences::sharedPreferences->examMode().isActive()) {
    assert(Preferences::sharedPreferences->examMode().ruleset() ==
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
      static_cast<PressToTestSwitch *>(m_selectableListView.cell(selectedRow()))
          ->canBeActivatedByEvent(event) &&
      !Preferences::sharedPreferences->examMode().isActive()) {
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
    if (!Preferences::sharedPreferences->examMode().isActive() &&
        !(m_tempPressToTestParams == ExamMode::PressToTestFlags{})) {
      // Scroll to validation cell if m_confirmPopUpController is discarded.
      selectLastCell();
      // Open pop-up to confirm discarding values
      m_confirmPopUpController.presentModally();
    } else {
      resetController();
      static_cast<StackViewController *>(parentResponder())->pop();
    }
    return true;
  }
  return false;
}

void PressToTestController::viewWillAppear() {
  // Reset selection and params only if exam mode has been activated.
  if (Preferences::sharedPreferences->examMode().isActive()) {
    resetController();
  }
  setMessages();
  ListWithTopAndBottomController::viewWillAppear();
}

int PressToTestController::numberOfRows() const {
  return k_numberOfSwitchCells +
         (Preferences::sharedPreferences->examMode().isActive() ? 0 : 1);
}

int PressToTestController::typeAtRow(int row) const {
  assert(row >= 0 && row <= k_numberOfSwitchCells);
  return row < k_numberOfSwitchCells ? k_switchCellType : k_buttonCellType;
}

HighlightCell *PressToTestController::reusableCell(int index, int type) {
  if (type == k_buttonCellType) {
    assert(index == 0);
    return &m_activateButton;
  }
  assert(type == k_switchCellType);
  assert(index >= 0 && index < k_numberOfReusableSwitchCells);
  return &m_switchCells[index];
}

int PressToTestController::reusableCellCount(int type) {
  return type == k_buttonCellType ? 1 : k_numberOfReusableSwitchCells;
}

void PressToTestController::fillCellForRow(HighlightCell *cell, int row) {
  if (typeAtRow(row) == k_buttonCellType) {
    assert(!Preferences::sharedPreferences->examMode().isActive());
    return;
  }
  assert(typeAtRow(row) == k_switchCellType);
  PressToTestSwitch *myCell = static_cast<PressToTestSwitch *>(cell);
  // A true params means the feature is disabled,
  bool featureIsDisabled = getParamAtIndex(row);
  myCell->label()->setMessage(LabelAtIndex(row));
  myCell->label()->setTextColor(
      Preferences::sharedPreferences->examMode().isActive() && featureIsDisabled
          ? Palette::GrayDark
          : KDColorBlack);
  myCell->subLabel()->setMessage(SubLabelAtIndex(row));
  // Switch is toggled if the feature must stay activated.
  myCell->accessory()->switchView()->setState(!featureIsDisabled);
  myCell->accessory()->setDisplayImage(
      Preferences::sharedPreferences->examMode().isActive());
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

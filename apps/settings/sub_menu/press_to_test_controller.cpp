#include "press_to_test_controller.h"

#include <apps/i18n.h>
#include <assert.h>

#include <cmath>

#include "../../apps_container.h"

using namespace Poincare;
using namespace Shared;
using namespace Escher;

namespace Settings {

PressToTestController::PressToTestController(Responder *parentResponder)
    : SelectableListViewController(parentResponder, &m_view),
      m_topMessageView(KDFont::Size::Small, I18n::Message::Default,
                       KDContext::k_alignCenter, KDContext::k_alignCenter,
                       Palette::GrayDark, Palette::WallScreen),
      m_bottomMessageView(KDFont::Size::Small,
                          I18n::Message::ToDeactivatePressToTest1,
                          KDContext::k_alignCenter, KDContext::k_alignCenter,
                          KDColorBlack, Palette::WallScreen),
      m_view(&m_selectableListView, this, &m_topMessageView,
             &m_bottomMessageView),
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
  resetController();
}

void PressToTestController::resetController() {
  selectCell(0);
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

KDCoordinate PressToTestController::nonMemoizedRowHeight(int j) {
  if (typeAtIndex(j) == k_buttonCellType) {
    return heightForCellAtIndex(&m_activateButton, j);
  }
  assert(typeAtIndex(j) == k_switchCellType);
  PressToTestSwitch tempCell;
  return heightForCellAtIndexWithWidthInit(&tempCell, j);
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
    m_view.setBottomView(&m_bottomMessageView);
  } else {
    m_topMessageView.setMessage(I18n::Message::PressToTestIntro1);
    m_view.setBottomView(nullptr);
  }
}

bool PressToTestController::handleEvent(Ion::Events::Event event) {
  int row = selectedRow();
  if ((event == Ion::Events::OK || event == Ion::Events::EXE) &&
      typeAtIndex(row) == k_switchCellType &&
      !Preferences::sharedPreferences->examMode().isActive()) {
    assert(row >= 0 && row < k_numberOfSwitchCells);
    setParamAtIndex(row, !getParamAtIndex(row));
    /* Memoization isn't resetted here because changing a switch state does not
     * alter the cell's height. */
    m_selectableListView.reloadCell(row);
    return true;
  }
  if (event == Ion::Events::Left || event == Ion::Events::Back) {
    // Deselect table because select cell will change anyway
    m_selectableListView.deselectTable();
    if (!Preferences::sharedPreferences->examMode().isActive() &&
        !(m_tempPressToTestParams == ExamMode::PressToTestFlags{})) {
      // Scroll to validation cell if m_confirmPopUpController is discarded.
      selectCell(numberOfRows() - 1);
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

void PressToTestController::didEnterResponderChain(
    Responder *previousFirstResponder) {
  // Reset selection and params only if exam mode has been activated.
  if (Preferences::sharedPreferences->examMode().isActive()) {
    resetController();
  }
  assert(selectedRow() >= 0);
  setMessages();
  resetMemoization();
  m_view.reload();
}

int PressToTestController::numberOfRows() const {
  return k_numberOfSwitchCells +
         (Preferences::sharedPreferences->examMode().isActive() ? 0 : 1);
}

int PressToTestController::typeAtIndex(int index) const {
  assert(index >= 0 && index <= k_numberOfSwitchCells);
  return index < k_numberOfSwitchCells ? k_switchCellType : k_buttonCellType;
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

void PressToTestController::willDisplayCellForIndex(HighlightCell *cell,
                                                    int index) {
  if (typeAtIndex(index) == k_buttonCellType) {
    assert(!Preferences::sharedPreferences->examMode().isActive());
    return;
  }
  PressToTestSwitch *myCell = static_cast<PressToTestSwitch *>(cell);
  // A true params means the feature is disabled,
  bool featureIsDisabled = getParamAtIndex(index);
  myCell->setMessage(LabelAtIndex(index));
  myCell->setTextColor(Preferences::sharedPreferences->examMode().isActive() &&
                               featureIsDisabled
                           ? Palette::GrayDark
                           : KDColorBlack);
  myCell->setSubLabelMessage(SubLabelAtIndex(index));
  // Switch is toggled if the feature must stay activated.
  myCell->setState(!featureIsDisabled);
  myCell->setDisplayImage(
      Preferences::sharedPreferences->examMode().isActive());
}

I18n::Message PressToTestController::LabelAtIndex(int i) {
  assert(i >= 0 && i < k_numberOfSwitchCells);
  constexpr I18n::Message labels[k_numberOfSwitchCells] = {
      I18n::Message::PressToTestExactResults,
      I18n::Message::PressToTestEquationSolver,
      I18n::Message::PressToTestInequalityGraphing,
      I18n::Message::PressToTestImplicitPlots,
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
    case I18n::Message::PressToTestImplicitPlots:
      return I18n::Message::PressToTestImplicitPlotsDescription;
    default:
      return I18n::Message::Default;
  }
}

}  // namespace Settings

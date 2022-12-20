#include "press_to_test_controller.h"
#include "../../apps_container.h"
#include <apps/i18n.h>
#include <assert.h>
#include <cmath>

using namespace Poincare;
using namespace Shared;
using namespace Escher;

namespace Settings {

PressToTestController::PressToTestController(Responder * parentResponder) :
  ViewController(parentResponder),
  m_selectableTableView(this, this, this, &m_view),
  m_topMessageView(KDFont::Size::Small, I18n::Message::Default, KDContext::k_alignCenter, KDContext::k_alignCenter, Palette::GrayDark, Palette::WallScreen),
  m_bottomMessageView(KDFont::Size::Small, I18n::Message::ToDeactivatePressToTest1, KDContext::k_alignCenter, KDContext::k_alignCenter, KDColorBlack, Palette::WallScreen),
  m_view(&m_selectableTableView, this, &m_topMessageView, &m_bottomMessageView),
  m_tempPressToTestParams{},
  m_activateButton(&m_selectableTableView, I18n::Message::ActivateTestMode, Invocation::Builder<PressToTestController>([](PressToTestController * controller, void * sender) {
    AppsContainer::sharedAppsContainer()->displayExamModePopUp(Preferences::ExamMode::PressToTest, controller->getPressToTestParams());
    return true; }, this)),
  m_confirmPopUpController(Invocation::Builder<PressToTestController>([](PressToTestController * controller, void * sender) {
    controller->resetController();
    static_cast<StackViewController *>(controller->parentResponder())->pop();
    return true; }, this))
{
  resetController();
}

void PressToTestController::resetController() {
  selectCellAtLocation(0, 0);
  if (Preferences::sharedPreferences()->isInExamMode()) {
    // Reset switches states to press-to-test current parameter.
    m_tempPressToTestParams = Preferences::sharedPreferences()->pressToTestParams();
  } else {
    // Reset switches so that all features are enabled.
    m_tempPressToTestParams = Preferences::k_inactivePressToTest;
  }
}

Preferences::PressToTestParams PressToTestController::getPressToTestParams() {
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
      m_tempPressToTestParams.m_exactResultsAreForbidden = value;
      break;
    case I18n::Message::PressToTestEquationSolver:
      m_tempPressToTestParams.m_equationSolverIsForbidden = value;
      break;
    case I18n::Message::PressToTestInequalityGraphing:
      m_tempPressToTestParams.m_inequalityGraphingIsForbidden = value;
      break;
    case I18n::Message::PressToTestImplicitPlots:
      m_tempPressToTestParams.m_implicitPlotsAreForbidden = value;
      break;
    case I18n::Message::PressToTestElements:
      m_tempPressToTestParams.m_elementsAppIsForbidden = value;
      break;
    case I18n::Message::PressToTestStatDiagnostics:
      m_tempPressToTestParams.m_statsDiagnosticsAreForbidden = value;
      break;
    case I18n::Message::PressToTestVectors:
      m_tempPressToTestParams.m_vectorsAreForbidden = value;
      break;
    case I18n::Message::PressToTestLogBaseA:
      m_tempPressToTestParams.m_basedLogarithmIsForbidden = value;
      break;
    case I18n::Message::PressToTestSum:
      m_tempPressToTestParams.m_sumIsForbidden = value;
      break;
    default:
      assert(false);
  }
}

bool PressToTestController::getParamAtIndex(int index) {
  switch (LabelAtIndex(index)) {
    case I18n::Message::PressToTestExactResults:
      return m_tempPressToTestParams.m_exactResultsAreForbidden;
    case I18n::Message::PressToTestEquationSolver:
      return m_tempPressToTestParams.m_equationSolverIsForbidden;
    case I18n::Message::PressToTestInequalityGraphing:
      return m_tempPressToTestParams.m_inequalityGraphingIsForbidden;
    case I18n::Message::PressToTestImplicitPlots:
      return m_tempPressToTestParams.m_implicitPlotsAreForbidden;
    case I18n::Message::PressToTestElements:
      return m_tempPressToTestParams.m_elementsAppIsForbidden;
    case I18n::Message::PressToTestStatDiagnostics:
      return m_tempPressToTestParams.m_statsDiagnosticsAreForbidden;
    case I18n::Message::PressToTestVectors:
      return m_tempPressToTestParams.m_vectorsAreForbidden;
    case I18n::Message::PressToTestLogBaseA:
      return m_tempPressToTestParams.m_basedLogarithmIsForbidden;
    case I18n::Message::PressToTestSum:
      return m_tempPressToTestParams.m_sumIsForbidden;
    default:
      assert(false);
      return false;
  }
}

void PressToTestController::setMessages() {
  if (Preferences::sharedPreferences()->isInExamMode()) {
    assert(Preferences::sharedPreferences()->examMode() == Preferences::ExamMode::PressToTest);
    m_topMessageView.setMessage(I18n::Message::PressToTestActiveIntro);
    m_view.setBottomView(&m_bottomMessageView);
  } else {
    m_topMessageView.setMessage(I18n::Message::PressToTestIntro1);
    m_view.setBottomView(nullptr);
  }
}

bool PressToTestController::handleEvent(Ion::Events::Event & event) {
  int row = selectedRow();
  if ((event == Ion::Events::OK || event == Ion::Events::EXE) && typeAtIndex(row) == k_switchCellType && !Preferences::sharedPreferences()->isInExamMode()) {
    assert(row >= 0 && row < k_numberOfSwitchCells);
    setParamAtIndex(row, !getParamAtIndex(row));
    /* Memoization isn't resetted here because changing a switch state does not
     * alter the cell's height. */
    m_selectableTableView.reloadCellAtLocation(selectedColumn(), row);
    return true;
  }
  if (event == Ion::Events::Left || event == Ion::Events::Back) {
    // Deselect table because select cell will change anyway
    m_selectableTableView.deselectTable();
    if (!Preferences::sharedPreferences()->isInExamMode() && !(m_tempPressToTestParams == Preferences::k_inactivePressToTest)) {
      // Scroll to validation cell if m_confirmPopUpController is discarded.
      selectCellAtLocation(0, numberOfRows() - 1);
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

void PressToTestController::didBecomeFirstResponder() {
  Container::activeApp()->setFirstResponder(&m_selectableTableView);
}

void PressToTestController::didEnterResponderChain(Responder * previousFirstResponder) {
  // Reset selection and params only if exam mode has been activated.
  if (Preferences::sharedPreferences()->isInExamMode()) {
    resetController();
  }
  assert(selectedRow() >= 0);
  setMessages();
  resetMemoization();
  m_view.reload();
}

int PressToTestController::numberOfRows() const {
  return k_numberOfSwitchCells + (Preferences::sharedPreferences()->isInExamMode() ? 0 : 1);
}

int PressToTestController::typeAtIndex(int index) const {
  assert(index >= 0 && index <= k_numberOfSwitchCells);
  return index < k_numberOfSwitchCells ? k_switchCellType : k_buttonCellType;
}

HighlightCell * PressToTestController::reusableCell(int index, int type) {
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

void PressToTestController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  if (typeAtIndex(index) == k_buttonCellType) {
    assert(!Preferences::sharedPreferences()->isInExamMode());
    return;
  }
  PressToTestSwitch * myCell = static_cast<PressToTestSwitch *>(cell);
  // A true params means the feature is disabled,
  bool featureIsDisabled = getParamAtIndex(index);
  myCell->setMessage(LabelAtIndex(index));
  myCell->setTextColor(Preferences::sharedPreferences()->isInExamMode() && featureIsDisabled ? Palette::GrayDark : KDColorBlack);
  myCell->setSubLabelMessage(SubLabelAtIndex(index));
  // Switch is toggled if the feature must stay activated.
  myCell->setState(!featureIsDisabled);
  myCell->setDisplayImage(Preferences::sharedPreferences()->isInExamMode());
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
    I18n::Message::PressToTestSum
  };
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

}

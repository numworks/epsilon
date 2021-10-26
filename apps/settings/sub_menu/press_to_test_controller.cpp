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
  GenericSubController(parentResponder),
  m_switchCells{},
  m_tempPressToTestParams{},
  m_activateButton{&m_selectableTableView, I18n::Message::ActivateTestMode, Invocation([](void * context, void * sender) {
    AppsContainer::sharedAppsContainer()->displayPressToTestPopUp(static_cast<PressToTestController *>(context)->getPressToTestParams());
    return true; }, this)}
{
  resetSwitches();
}

void PressToTestController::resetSwitches() {
  // Reset switches states to press-to-test current parameter.
  m_tempPressToTestParams = GlobalPreferences::sharedGlobalPreferences()->pressToTestParams();
}

GlobalPreferences::PressToTestParams PressToTestController::getPressToTestParams() {
  return m_tempPressToTestParams;
}

void PressToTestController::setParamAtIndex(int index, bool value) {
  switch (index) {
    case k_equationSolverIndex:
      m_tempPressToTestParams.equationSolver = value;
      break;
    case k_inequalityGraphingIndex:
      m_tempPressToTestParams.inequalityGraphing = value;
      break;
    case k_implicitPlotsIndex:
      m_tempPressToTestParams.implicitPlots = value;
      break;
    case k_statDiagnosticIndex:
      m_tempPressToTestParams.statDiagnostic = value;
      break;
    case k_vectorsIndex:
      m_tempPressToTestParams.vectors = value;
      break;
    case k_basedLogarithmIndex:
      m_tempPressToTestParams.basedLogarithm = value;
      break;
    default:
      assert(index == k_sumIndex);
      m_tempPressToTestParams.sum = value;
      break;
  }
}

bool PressToTestController::getParamAtIndex(int index) {
  switch (index) {
    case k_equationSolverIndex:
      return m_tempPressToTestParams.equationSolver;
    case k_inequalityGraphingIndex:
      return m_tempPressToTestParams.inequalityGraphing;
    case k_implicitPlotsIndex:
      return m_tempPressToTestParams.implicitPlots;
    case k_statDiagnosticIndex:
      return m_tempPressToTestParams.statDiagnostic;
    case k_vectorsIndex:
      return m_tempPressToTestParams.vectors;
    case k_basedLogarithmIndex:
      return m_tempPressToTestParams.basedLogarithm;
    default:
      assert(index == k_sumIndex);
      return m_tempPressToTestParams.sum;
  }
}

bool PressToTestController::handleEvent(Ion::Events::Event event) {
  if ((event == Ion::Events::OK || event == Ion::Events::EXE) && typeAtIndex(selectedRow()) == k_switchCellType && !GlobalPreferences::sharedGlobalPreferences()->isInExamMode()) {
    assert(selectedRow() >= 0 && selectedRow() < k_numberOfSwitchCells);
    setParamAtIndex(selectedRow(), !getParamAtIndex(selectedRow()));
    resetMemoization();
    m_selectableTableView.reloadData();
    return true;
  }
  return GenericSubController::handleEvent(event);
}

void PressToTestController::didEnterResponderChain(Responder * previousFirstResponder) {
  /* When a pop-up is dismissed, the exam mode status might have changed. We
   * reload the selection as the number of rows might have also changed. We
   * force to reload the entire data because they might have changed. */
  // TODO Hugo : Fix visible artefact issue after activating exam mode
  selectCellAtLocation(0, initialSelectedRow());
  m_selectableTableView.reloadData();
  // TODO Hugo : Add a message when the mode exam is on
}

int PressToTestController::numberOfRows() const {
  return k_numberOfSwitchCells + (GlobalPreferences::sharedGlobalPreferences()->isInExamMode() ? 0 : 1);
}

int PressToTestController::typeAtIndex(int index) {
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
    assert(!GlobalPreferences::sharedGlobalPreferences()->isInExamMode());
    return;
  }
  Escher::MessageTableCellWithMessageWithSwitch * myCell = static_cast<Escher::MessageTableCellWithMessageWithSwitch *>(cell);
  myCell->setMessage(LabelAtIndex(index));
  myCell->setSubLabelMessage(SubLabelAtIndex(index));
  // TODO Hugo : Set switch layout according to exam mode status.
  // if (GlobalPreferences::sharedGlobalPreferences()->isInExamMode()) {
  // }
  myCell->setState(getParamAtIndex(index));
}

I18n::Message PressToTestController::LabelAtIndex(int i) {
  // TODO Hugo : Add Sigma symbol in PressToTestSum
  // TODO Hugo : Translate the texts
  assert(i >= 0 && i < k_numberOfSwitchCells);
  constexpr I18n::Message labels[k_numberOfSwitchCells] = {
    I18n::Message::PressToTestEquationSolver,
    I18n::Message::PressToTestInequalityGraphing,
    I18n::Message::PressToTestImplicitPlots,
    I18n::Message::PressToTestStatDiagnostics,
    I18n::Message::PressToTestVectors,
    I18n::Message::PressToTestLogBaseA,
    I18n::Message::PressToTestSum
  };
  return labels[i];
}

I18n::Message PressToTestController::SubLabelAtIndex(int i) {
  assert(i >= 0 && i < k_numberOfSwitchCells);
  constexpr I18n::Message subLabels[k_numberOfSwitchCells] = {
    I18n::Message::Default,
    I18n::Message::Default,
    I18n::Message::Default,
    I18n::Message::PressToTestStatDiagnosticsDescription,
    I18n::Message::PressToTestVectorsDescription,
    I18n::Message::Default,
    I18n::Message::Default
  };
  return subLabels[i];
}

}

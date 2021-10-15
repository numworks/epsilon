#include "press_to_test_controller.h"
#include "../../global_preferences.h"
#include "../../apps_container.h"
#include "../../exam_mode_configuration.h"
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
  m_tempSwitchState{}
{
  // TODO Hugo : Initialize m_tempSwitchState
}

bool PressToTestController::handleEvent(Ion::Events::Event event) {
  // TODO Hugo : Use type here
  // TODO Hugo : Uncomment this with implemented button
  // if ((event == Ion::Events::OK || event == Ion::Events::EXE) && selectedRow() == numberOfRows() - 1 && !GlobalPreferences::sharedGlobalPreferences()->isInExamMode()) {
  //   AppsContainer::sharedAppsContainer()->displayExamModePopUp(GlobalPreferences::ExamMode::USTestMode); // examMode()
  //   return true;
  // }
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    assert(selectedRow() >= 0 && selectedRow() < k_numberOfSwitchCells);
    m_tempSwitchState[selectedRow()] = !m_tempSwitchState[selectedRow()];
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
  selectCellAtLocation(0, initialSelectedRow());
  // m_content->reload();
  // TODO Hugo : Add a message when the mode exam is on
}

int PressToTestController::numberOfRows() const {
  // TODO Hugo : Add Button (GlobalPreferences::sharedGlobalPreferences()->isInExamMode() ? 0 : 1) +
  return k_numberOfSwitchCells;
}

HighlightCell * PressToTestController::reusableCell(int index, int type) {
  // TODO Hugo : Use more types
  assert(type == 0);
  assert(index >= 0 && index < k_numberOfReusableSwitchCells);
  return &m_switchCells[index];
}

int PressToTestController::reusableCellCount(int type) {
  return k_numberOfReusableSwitchCells;
}

void PressToTestController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  Escher::MessageTableCellWithMessageWithSwitch * myCell = static_cast<Escher::MessageTableCellWithMessageWithSwitch *>(cell);
  myCell->setMessage(LabelAtIndex(index));
  myCell->setSubLabelMessage(SubLabelAtIndex(index));
  SwitchView * switchView = (SwitchView *)myCell->accessoryView();
  switchView->setState(m_tempSwitchState[index]);
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

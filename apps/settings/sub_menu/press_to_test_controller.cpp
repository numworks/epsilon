#include "press_to_test_controller.h"
#include "../../apps_container.h"
#include <apps/i18n.h>
#include <assert.h>
#include <cmath>

using namespace Poincare;
using namespace Shared;
using namespace Escher;

namespace Settings {

/* PressToTestView */

void PressToTestView::reload() {
  m_contentView.m_table->reloadData();
  layoutSubviews();
}

void PressToTestView::tableViewDidChangeSelectionAndDidScroll(SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY, bool withinTemporarySelection) {
  // Scroll to correct location
  int row = m_contentView.m_table->selectedRow(), col = m_contentView.m_table->selectedColumn();
  if (row >= 0 && col >= 0) {
    KDRect cellFrame = KDRect(
        m_tableDataSource->cumulatedWidthFromIndex(col),
        m_contentView.tableOrigin()
            + m_tableDataSource->cumulatedHeightFromIndex(row),
        m_tableDataSource->columnWidth(col), m_tableDataSource->rowHeight(row));
    /* Include the message in the first or last row cells to force scrolling
     * enough to display it */
    if (row == 0) {
      cellFrame = cellFrame.unionedWith(KDRect(cellFrame.x(), 0, cellFrame.width(), cellFrame.height()));
    } else if (row + 1 == m_contentView.m_table->numberOfDisplayableRows()) {
      cellFrame = cellFrame.unionedWith(KDRect(cellFrame.x(), cellFrame.y(), cellFrame.width(), m_contentView.totalHeight() - cellFrame.y()));
    }
    scrollToContentRect(cellFrame);
  }
}

/* PressToTestView::ContentView */

PressToTestView::ContentView::ContentView(SelectableTableView * table) :
    m_table(table),
    m_topMessageView(KDFont::SmallFont, I18n::Message::Default, KDContext::k_alignCenter, KDContext::k_alignCenter, Palette::GrayDark, Palette::WallScreen),
    m_bottomMessageView(KDFont::SmallFont, I18n::Message::Default, KDContext::k_alignCenter, KDContext::k_alignCenter, KDColorBlack, Palette::WallScreen) {
  // Margins between the table and messages
  m_table->setMargins(topTableMargin(), 0, bottomTableMargin(), 0);
}

void PressToTestView::ContentView::setMessages(I18n::Message top, I18n::Message bottom) {
  m_topMessageView.setMessage(top);
  m_bottomMessageView.setMessage(bottom);
  // Bottom message might have (dis)appeared, reset table's margins
  m_table->setMargins(topTableMargin(), 0, bottomTableMargin(), 0);
}

void PressToTestView::layoutSubviews(bool force) {
  /* Content view must be given a width, so that it can give its
   * SelectableTableView a width and compute it height. */
  if (m_contentView.bounds().isEmpty()) {
    m_contentView.setFrame(KDRect(KDPointZero, KDSize(visibleContentRect().width(), 0)), force);
  } else {
    assert(m_contentView.bounds().width() == visibleContentRect().width());
  }
  ScrollView::layoutSubviews(force);
}

KDCoordinate PressToTestView::ContentView::tableOrigin() const {
  return m_topMessageView.minimalSizeForOptimalDisplay().height() + topTableMargin();
}

KDCoordinate PressToTestView::ContentView::totalHeight() const {
  return minimalSizeForOptimalDisplay().height();
}

Escher::View * PressToTestView::ContentView::subviewAtIndex(int i) {
  switch (i) {
  case 0:
    return &m_topMessageView;
  case 1:
    return m_table;
  default:
    assert(i == numberOfSubviews() - 1);
    return &m_bottomMessageView;
  }
}

/* PressToTestController */

PressToTestController::PressToTestController(Responder * parentResponder) :
  ViewController(parentResponder),
  m_selectableTableView(this, this, this, &m_view),
  m_view(&m_selectableTableView, this),
  m_switchCells{},
  m_tempPressToTestParams{},
  m_activateButton{&m_selectableTableView, I18n::Message::ActivateTestMode, Invocation([](void * context, void * sender) {
    AppsContainer::sharedAppsContainer()->displayExamModePopUp(GlobalPreferences::ExamMode::PressToTest, static_cast<PressToTestController *>(context)->getPressToTestParams());
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

KDCoordinate PressToTestController::nonMemoizedRowHeight(int j) {
  if (typeAtIndex(j) == k_buttonCellType) {
    return heightForCellAtIndex(&m_activateButton, j);
  }
  assert(typeAtIndex(j) == k_switchCellType);
  PressToTestSwitch tempCell;
  // tempCell needs a width so that we can compute its height.
  tempCell.setSize(KDSize(cellWidth(), tempCell.bounds().height()));
  return heightForCellAtIndex(&tempCell, j);
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

void PressToTestController::setMessages() {
  if (GlobalPreferences::sharedGlobalPreferences()->isInExamMode()) {
    assert(GlobalPreferences::sharedGlobalPreferences()->examMode() == GlobalPreferences::ExamMode::PressToTest);
    return m_view.setMessages(I18n::Message::PressToTestActiveIntro, I18n::Message::ToDeactivatePressToTest1);
  }
  return m_view.setMessages(I18n::Message::PressToTestIntro1, I18n::Message::Default);
}

void PressToTestController::viewDidDisappear() {
  m_selectableTableView.deselectTable();
}

bool PressToTestController::handleEvent(Ion::Events::Event event) {
  int row = selectedRow();
  if ((event == Ion::Events::OK || event == Ion::Events::EXE) && typeAtIndex(row) == k_switchCellType && !GlobalPreferences::sharedGlobalPreferences()->isInExamMode()) {
    assert(row >= 0 && row < k_numberOfSwitchCells);
    setParamAtIndex(row, !getParamAtIndex(row));
    /* Memoization isn't resetted here because changing a switch state does not
     * alter the cell's height. */
    m_selectableTableView.reloadData();
    return true;
  }
  if (event == Ion::Events::Left) {
    ((StackViewController *)parentResponder())->pop();
    return true;
  }
  return false;
}

void PressToTestController::didBecomeFirstResponder() {
  Container::activeApp()->setFirstResponder(&m_selectableTableView);
}

void PressToTestController::didEnterResponderChain(Responder * previousFirstResponder) {
  /* When a pop-up is dismissed, the exam mode status might have changed. We
   * reload the selection as the number of rows might have also changed. We
   * force to reload the entire data because they might have changed. */
  selectCellAtLocation(0, 0);
  setMessages();
  resetMemoization();
  m_view.reload();
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
  PressToTestSwitch * myCell = static_cast<PressToTestSwitch *>(cell);
  myCell->setMessage(LabelAtIndex(index));
  myCell->setTextColor(GlobalPreferences::sharedGlobalPreferences()->isInExamMode() && !getParamAtIndex(index) ? Palette::GrayDark : KDColorBlack);
  myCell->setSubLabelMessage(SubLabelAtIndex(index));
  myCell->setState(getParamAtIndex(index));
  myCell->setDisplayImage(GlobalPreferences::sharedGlobalPreferences()->isInExamMode());
}

I18n::Message PressToTestController::LabelAtIndex(int i) {
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

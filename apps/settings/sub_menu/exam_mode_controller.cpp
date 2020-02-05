#include "exam_mode_controller.h"
#include "../../global_preferences.h"
#include "../../apps_container.h"
#include <apps/i18n.h>
#include <assert.h>
#include <cmath>

using namespace Poincare;
using namespace Shared;

namespace Settings {

ExamModeController::ContentView::ContentView(SelectableTableView * selectableTableView) :
  m_selectableTableView(selectableTableView),
  m_deactivateLine1(KDFont::SmallFont, I18n::Message::ToDeactivateExamMode1, 0.5f, 0.5f, KDColorBlack, Palette::WallScreen),
  m_deactivateLine2(KDFont::SmallFont, I18n::Message::ToDeactivateExamMode2, 0.5f, 0.5f, KDColorBlack, Palette::WallScreen),
  m_deactivateLine3(KDFont::SmallFont, I18n::Message::ToDeactivateExamMode3, 0.5f, 0.5f, KDColorBlack, Palette::WallScreen)
{
}

void ExamModeController::ContentView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(bounds(), Palette::WallScreen);
}

View * ExamModeController::ContentView::subviewAtIndex(int index) {
  assert(index >= 0 && index < 4);
  if (index == 0) {
    return m_selectableTableView;
  }
  assert(shouldDisplayDeactivateMessages());
  if (index == 1) {
    return &m_deactivateLine1;
  }
  if (index == 2) {
    return &m_deactivateLine2;
  }
  return &m_deactivateLine3;
}

void ExamModeController::ContentView::layoutSubviews(bool force) {
  // Layout the table view
  KDCoordinate tableHeight = m_selectableTableView->minimalSizeForOptimalDisplay().height();
  m_selectableTableView->setFrame(KDRect(0, 0, bounds().width(), tableHeight), force);

  if (!shouldDisplayDeactivateMessages()) {
    return;
  }

  // Layout the text
  KDCoordinate textHeight = KDFont::SmallFont->glyphSize().height();
  KDCoordinate defOrigin = tableHeight + (bounds().height() - tableHeight - 3*textHeight - Metric::CommonTopMargin)/2;

  m_deactivateLine1.setFrame(KDRect(0, defOrigin, bounds().width(), textHeight), force);
  m_deactivateLine2.setFrame(KDRect(0, defOrigin + textHeight, bounds().width(), textHeight), force);
  m_deactivateLine3.setFrame(KDRect(0, defOrigin + 2 * textHeight, bounds().width(), textHeight), force);
}

bool ExamModeController::ContentView::shouldDisplayDeactivateMessages() const {
  return GlobalPreferences::sharedGlobalPreferences()->isInExamMode();
}

ExamModeController::ExamModeController(Responder * parentResponder) :
  GenericSubController(parentResponder),
  m_contentView(&m_selectableTableView),
  m_cell{MessageTableCell(I18n::Message::ExamModeActive, KDFont::LargeFont), MessageTableCell(I18n::Message::ActivateDutchExamMode, KDFont::LargeFont)}
{
}

bool ExamModeController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    GlobalPreferences::ExamMode mode = GlobalPreferences::ExamMode::Standard;
    if (GlobalPreferences::sharedGlobalPreferences()->isInExamMode()) {
      // If the exam mode is already on, this re-activate the same exam mode
      mode = GlobalPreferences::sharedGlobalPreferences()->examMode();
    } else if (m_messageTreeModel->children(selectedRow())->label() == I18n::Message::ActivateDutchExamMode) {
      mode = GlobalPreferences::ExamMode::Dutch;
    }
    AppsContainer::sharedAppsContainer()->displayExamModePopUp(mode);
    return true;
  }
  return GenericSubController::handleEvent(event);
}

void ExamModeController::didEnterResponderChain(Responder * previousFirstResponder) {
  /* When a pop-up is dismissed, the exam mode status might have changed. We
   * reload the selection as the number of rows might have also changed. We
   * force to reload the entire data because they might have changed. */
  selectCellAtLocation(0, initialSelectedRow());
  m_selectableTableView.reloadData();
  // We add a message when the mode exam is on
  m_contentView.layoutSubviews(true);
}

int ExamModeController::numberOfRows() const {
  if (GlobalPreferences::sharedGlobalPreferences()->language() != I18n::Language::EN || GlobalPreferences::sharedGlobalPreferences()->isInExamMode()) {
    return 1;
  }
  return GenericSubController::numberOfRows();
}

HighlightCell * ExamModeController::reusableCell(int index, int type) {
  assert(type == 0);
  assert(index >= 0 && index < k_maxNumberOfCells);
  return &m_cell[index];
}

int ExamModeController::reusableCellCount(int type) {
  return k_maxNumberOfCells;
}

void ExamModeController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  GenericSubController::willDisplayCellForIndex(cell, index);
  if (GlobalPreferences::sharedGlobalPreferences()->isInExamMode()) {
    MessageTableCell * myCell = (MessageTableCell *)cell;
    myCell->setMessage(I18n::Message::ExamModeActive);
  }
}

int ExamModeController::initialSelectedRow() const {
  int row = selectedRow();
  if (row >= numberOfRows()) {
    return numberOfRows()-1;
  } else if (row < 0) {
    return 0;
  }
  return row;
}

}

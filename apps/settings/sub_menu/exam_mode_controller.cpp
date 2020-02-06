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
  m_selectableTableView(selectableTableView)
{
  for (int i = 0; i < k_maxNumberOfLines; i++) {
    m_messageLines[i].setFont(KDFont::SmallFont);
    m_messageLines[i].setAlignment(0.5f, 0.5f);
    m_messageLines[i].setBackgroundColor(Palette::WallScreen);
  }
}

void ExamModeController::ContentView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(bounds(), Palette::WallScreen);
}

void ExamModeController::ContentView::setMessages(I18n::Message m[k_maxNumberOfLines]) {
  for (int i = 0; i < k_maxNumberOfLines; i++) {
    m_messageLines[i].setMessage(m[i]);
  }
  layoutSubviews();
}

View * ExamModeController::ContentView::subviewAtIndex(int index) {
  assert(index >= 0 && index < numberOfSubviews());
  if (index == 0) {
    return m_selectableTableView;
  }
  return &m_messageLines[index-1];
}

void ExamModeController::ContentView::layoutSubviews(bool force) {
  // Layout the table view
  KDCoordinate tableHeight = m_selectableTableView->minimalSizeForOptimalDisplay().height();
  m_selectableTableView->setFrame(KDRect(0, 0, bounds().width(), tableHeight), force);

  int nbOfMessages = numberOfMessages();

  // Layout the text
  KDCoordinate textHeight = KDFont::SmallFont->glyphSize().height();
  KDCoordinate defOrigin = tableHeight + (bounds().height() - tableHeight - nbOfMessages*textHeight - Metric::CommonTopMargin)/2;

  for (int i = 0; i < nbOfMessages; i++) {
    m_messageLines[i].setFrame(KDRect(0, defOrigin, bounds().width(), textHeight), force);
    defOrigin += textHeight;
  }
  for (int i = nbOfMessages; i < k_maxNumberOfLines; i++) {
    m_messageLines[i].setFrame(KDRectZero, force);
  }
}

int ExamModeController::ContentView::numberOfMessages() const {
  return GlobalPreferences::sharedGlobalPreferences()->isInExamMode() ? k_numberOfDeactivationMessageLines : k_maxNumberOfLines;
}

ExamModeController::ExamModeController(Responder * parentResponder) :
  GenericSubController(parentResponder),
  m_contentView(&m_selectableTableView),
  m_cell{MessageTableCell(I18n::Message::ExamModeActive, KDFont::LargeFont), MessageTableCell(I18n::Message::ActivateDutchExamMode, KDFont::LargeFont)}
{
}

bool ExamModeController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    AppsContainer::sharedAppsContainer()->displayExamModePopUp(examMode());
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
  if (GlobalPreferences::sharedGlobalPreferences()->isInExamMode()) {
    I18n::Message deactivateMessages[] = {I18n::Message::ToDeactivateExamMode1, I18n::Message::ToDeactivateExamMode2, I18n::Message::ToDeactivateExamMode3, I18n::Message::Default, I18n::Message::Default, I18n::Message::Default};
    m_contentView.setMessages(deactivateMessages);
  } else {
    I18n::Message warningMessages[] = {I18n::Message::ExamModeWarning1, I18n::Message::ExamModeWarning2, I18n::Message::ExamModeWarning3, I18n::Message::ExamModeWarning4, I18n::Message::ExamModeWarning5, I18n::Message::ExamModeWarning6};
    m_contentView.setMessages(warningMessages);
  }
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

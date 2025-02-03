#include "controller.h"

#include <apps/apps_container.h>
#include <apps/home/apps_layout.h>
#include <ion/keyboard/layout_events.h>
#include <poincare/preferences.h>

#include "app.h"

extern "C" {
#include <assert.h>
}

using namespace Escher;
using namespace Poincare;

namespace Home {

Controller::ContentView::ContentView(
    Controller* controller, SelectableTableViewDataSource* selectionDataSource)
    : m_selectableTableView(controller, controller, selectionDataSource,
                            controller) {
  m_selectableTableView.setVerticalCellOverlap(0);
  m_selectableTableView.setMargins(k_margins);
  m_selectableTableView.setBackgroundColor(KDColorWhite);
  m_selectableTableView.decorator()->setVerticalMargins(
      {k_indicatorMargin, k_indicatorMargin - k_bottomMargin});
}

SelectableTableView* Controller::ContentView::selectableTableView() {
  return &m_selectableTableView;
}

void Controller::ContentView::reloadBottomRow(
    SimpleTableViewDataSource* dataSource, int lastIconColumn) {
  /* We mark the missing icons on the last row as dirty. */
  for (int i = lastIconColumn + 1; i < dataSource->numberOfColumns(); i++) {
    markRectAsDirty(KDRect(dataSource->columnWidth(0) * i,
                           dataSource->rowHeight(0), dataSource->columnWidth(0),
                           dataSource->rowHeight(0)));
  }
}

View* Controller::ContentView::subviewAtIndex(int index) {
  assert(index == 0);
  return &m_selectableTableView;
}

void Controller::ContentView::layoutSubviews(bool force) {
  /* Selectable table view's bottom margin is used for vertical scroll: we
   * scroll to the selected cell's farthest pixel + bottom margin. For home app,
   * instead of having a bottom bargin, we reduce the frame of the selectable
   * table view and ContentView::drawRect will take care of drawing the white
   * pixels at the bottom of the screen. */
  KDRect frame =
      KDRect(KDPointZero, bounds().width(), bounds().height() - k_bottomMargin);
  setChildFrame(&m_selectableTableView, frame, force);
}

Controller::Controller(Responder* parentResponder,
                       SelectableTableViewDataSource* selectionDataSource)
    : ViewController(parentResponder), m_view(this, selectionDataSource) {}

bool Controller::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    switchToSelectedApp();
    return true;
  }
  if (event == Ion::Events::Home || event == Ion::Events::Back) {
    m_view.selectableTableView()->selectCellAtLocation(0, 0);
    return true;
  }
  if (event == Ion::Events::Right &&
      selectionDataSource()->selectedRow() < numberOfRows() - 1) {
    m_view.selectableTableView()->selectCellAtLocation(
        0, selectionDataSource()->selectedRow() + 1);
    return true;
  }
  if (event == Ion::Events::Left && selectionDataSource()->selectedRow() > 0) {
    m_view.selectableTableView()->selectCellAtLocation(
        k_numberOfColumns - 1, selectionDataSource()->selectedRow() - 1);
    return true;
  }
  // Handle keys from 0 to 9
  char eventText[Ion::Events::EventData::k_maxDataSize] = {0};
  size_t length = Ion::Events::copyText(static_cast<uint8_t>(event), eventText,
                                        Ion::Events::EventData::k_maxDataSize);
  if (length == 1 && eventText[0] >= '0' && eventText[0] <= '9') {
    int iconIndex;
    if (eventText[0] == '0') {
      // Settings app is the last visible builtin app
      iconIndex = numberOfIcons() - 1 -
                  AppsContainer::sharedAppsContainer()->numberOfExternalApps();
    } else {
      iconIndex = eventText[0] - '1';
    }
    int col = columnOfIconAtIndex(iconIndex);
    int row = rowOfIconAtIndex(iconIndex);
    if (col == m_view.selectableTableView()->selectedColumn() &&
        row == m_view.selectableTableView()->selectedRow()) {
      // We were already on the selected app
      switchToSelectedApp();
      return true;
    }
    m_view.selectableTableView()->selectCellAtLocation(col, row);
    return true;
  }
  // Handle Down when less than 3 icons at last row
  if (event == Ion::Events::Down &&
      selectionDataSource()->selectedRow() == numberOfRows() - 2) {
    // Otherwise would have been handled by SelectableTableView
    assert(selectionDataSource()->selectedColumn() > columnOfLastIcon());
    m_view.selectableTableView()->selectCellAtLocation(columnOfLastIcon(),
                                                       numberOfRows() - 1);
    return true;
  }
  return false;
}

void Controller::handleResponderChainEvent(ResponderChainEvent event) {
  if (event.type == ResponderChainEventType::HasBecomeFirst) {
    if (selectionDataSource()->selectedRow() == -1) {
      selectionDataSource()->selectCellAtLocation(0, 0);
    }
    App::app()->setFirstResponder(m_view.selectableTableView());
  } else {
    ViewController::handleResponderChainEvent(event);
  }
}

HighlightCell* Controller::reusableCell(int index) {
  assert(0 <= index && index < reusableCellCount());
  return &m_reusableCells[index];
}

void Controller::fillCellForLocation(HighlightCell* cell, int column, int row) {
  AppCell* appCell = static_cast<AppCell*>(cell);
  AppsContainer* container = AppsContainer::sharedAppsContainer();
  int appIdx = indexOfAppAtColumnAndRow(column, row);
  if (appIdx >= container->numberOfApps()) {
    appCell->setVisible(false);
  } else {
    appCell->setVisible(true);
    if (appIdx < container->numberOfBuiltinApps()) {
      const ::App::Descriptor* descriptor =
          container->appSnapshotAtIndex(PermutedAppSnapshotIndex(appIdx))
              ->descriptor();
      appCell->setBuiltinAppDescriptor(descriptor);
    } else {
      Ion::ExternalApps::App a = container->externalAppAtIndex(
          appIdx - container->numberOfBuiltinApps());
      appCell->setExternalApp(a);
    }
  }
}

int Controller::numberOfIcons() const {
  AppsContainer* container = AppsContainer::sharedAppsContainer();
  assert(container->numberOfApps() > 0);
  return container->numberOfApps() - 1;
}

void Controller::tableViewDidChangeSelectionAndDidScroll(
    SelectableTableView* t, int previousSelectedCol, int previousSelectedRow,
    KDPoint previousOffset, bool withinTemporarySelection) {
  assert(t == m_view.selectableTableView());
  /* If the number of icons is != 3*n, when we display the lowest row, no icons
   * are redrawn on the last cells, so the cells are not cleaned. We need to
   * redraw a white rect on these cells to hide the leftover icons. Ideally, we
   * would have redrawn all the background in white and then redraw visible
   * cells. However, the redrawing takes time and is visible at scrolling. Here,
   * we avoid the background complete redrawing but the code is a bit clumsy. */
  if (!withinTemporarySelection && t->selectedRow() == numberOfRows() - 1) {
    m_view.reloadBottomRow(this, columnOfLastIcon());
  }
}

SelectableTableViewDataSource* Controller::selectionDataSource() const {
  return App::app()->snapshot();
}

void Controller::switchToSelectedApp() {
  AppsContainer* container = AppsContainer::sharedAppsContainer();
  int appIdx = indexOfAppAtColumnAndRow(selectionDataSource()->selectedColumn(),
                                        selectionDataSource()->selectedRow());
  if (appIdx < container->numberOfBuiltinApps()) {
    ::App::Snapshot* selectedSnapshot =
        container->appSnapshotAtIndex(PermutedAppSnapshotIndex(appIdx));
    if (appIsForbidden(selectedSnapshot->descriptor()->name())) {
      App::app()->displayWarning(forbiddenAppMessage());
    } else {
      container->switchToBuiltinApp(selectedSnapshot);
    }
  } else {
    assert(Preferences::SharedPreferences()->examMode().ruleset() !=
           ExamMode::Ruleset::Off);
    m_view.reload();
    Ion::ExternalApps::App a = container->externalAppAtIndex(
        appIdx - container->numberOfBuiltinApps());
    container->switchToExternalApp(a);
  }
}

bool Controller::appIsForbidden(I18n::Message appName) const {
  ExamMode examMode = Preferences::SharedPreferences()->examMode();
  return (appName == I18n::Message::CodeApp && examMode.forbidCodeApp()) ||
         (appName == I18n::Message::ElementsApp &&
          examMode.forbidElementsApp()) ||
         (appName == I18n::Message::SolverApp && examMode.forbidSolverApp());
}

I18n::Message Controller::forbiddenAppMessage() const {
  ExamMode mode = Preferences::SharedPreferences()->examMode();
  if (mode.ruleset() == ExamMode::Ruleset::PressToTest) {
    return I18n::Message::ForbiddenAppInPressToTestMode;
  }
  assert(mode.isActive());
  return I18n::Message::ForbiddenAppInExamMode;
}

}  // namespace Home

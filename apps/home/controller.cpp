#include "controller.h"
#include "app.h"
#include <apps/home/apps_layout.h>
#include "../apps_container.h"
#include "../exam_mode_configuration.h"
#include <ion/keyboard/layout_events.h>
#include <poincare/preferences.h>

extern "C" {
#include <assert.h>
}

using namespace Escher;

namespace Home {

Controller::ContentView::ContentView(Controller * controller, SelectableTableViewDataSource * selectionDataSource) :
  m_selectableTableView(controller, controller, selectionDataSource, controller)
{
  m_selectableTableView.setVerticalCellOverlap(0);
  m_selectableTableView.setMargins(0, k_sideMargin, k_bottomMargin, k_sideMargin);
  m_selectableTableView.setBackgroundColor(KDColorWhite);
  m_selectableTableView.decorator()->setVerticalMargins(k_indicatorMargin, k_indicatorMargin);
}

SelectableTableView * Controller::ContentView::selectableTableView() {
  return &m_selectableTableView;
}

void Controller::ContentView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(bounds(), KDColorWhite);
}

void Controller::ContentView::reload() {
  markRectAsDirty(bounds());
}

void Controller::ContentView::reloadBottomRow(SimpleTableViewDataSource * dataSource, int numberOfIcons, int numberOfColumns) {
  if (numberOfIcons % numberOfColumns) {
    /* We mark the missing icons on the last row as dirty. */
    for (int i = 0; i < numberOfColumns; i++) {
      if (i >= numberOfIcons % numberOfColumns) {
        markRectAsDirty(KDRect(dataSource->columnWidth(0)*i, dataSource->rowHeight(0), dataSource->columnWidth(0), dataSource->rowHeight(0)));
      }
    }
  }
}

int Controller::ContentView::numberOfSubviews() const {
  return 1;
}

View * Controller::ContentView::subviewAtIndex(int index) {
  assert(index == 0);
  return &m_selectableTableView;
}

void Controller::ContentView::layoutSubviews(bool force) {
  m_selectableTableView.setFrame(bounds(), force);
}

Controller::Controller(Responder * parentResponder, SelectableTableViewDataSource * selectionDataSource) :
  ViewController(parentResponder),
  m_view(this, selectionDataSource)
{
}

bool Controller::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    switchToSelectedApp();
    return true;
  }

  if (event == Ion::Events::Home || event == Ion::Events::Back) {
    return m_view.selectableTableView()->selectCellAtLocation(0, 0);
  }

  if (event == Ion::Events::Right && selectionDataSource()->selectedRow() < numberOfRows() - 1) {
    return m_view.selectableTableView()->selectCellAtLocation(0, selectionDataSource()->selectedRow() + 1);
  }
  if (event == Ion::Events::Left && selectionDataSource()->selectedRow() > 0) {
    return m_view.selectableTableView()->selectCellAtLocation(numberOfColumns() - 1, selectionDataSource()->selectedRow() - 1);
  }

  char eventText[Ion::Events::EventData::k_maxDataSize] = {0};
  size_t length = Ion::Events::copyText(static_cast<uint8_t>(event), eventText, Ion::Events::EventData::k_maxDataSize);
  if (length == 1 && eventText[0] >= '0' && eventText[0] <= '9') { // Handle keys from 0 to 9
    int appIndex = eventText[0] == '0' ? numberOfIcons() - 1 : eventText[0] - '1';
    int i = appIndex % k_numberOfColumns;
    int j = appIndex / k_numberOfColumns;
    if (i == m_view.selectableTableView()->selectedColumn() && j == m_view.selectableTableView()->selectedRow()) {
      // We were already on the selected app
      switchToSelectedApp();
      return true;
    }
    return m_view.selectableTableView()->selectCellAtLocation(i, j);
  }

  return false;
}

void Controller::didBecomeFirstResponder() {
  if (selectionDataSource()->selectedRow() == -1) {
    selectionDataSource()->selectCellAtLocation(0, 0);
  }
  Container::activeApp()->setFirstResponder(m_view.selectableTableView());
}

View * Controller::view() {
  return &m_view;
}

int Controller::numberOfRows() const {
  return ((numberOfIcons() - 1) / k_numberOfColumns) + 1;
}

int Controller::numberOfColumns() const {
  return k_numberOfColumns;
}

KDCoordinate Controller::defaultRowHeight() {
  return k_cellHeight;
}

KDCoordinate Controller::defaultColumnWidth() {
  return k_cellWidth;
}

HighlightCell * Controller::reusableCell(int index) {
  return &m_cells[index];
}

int Controller::reusableCellCount() const {
  return k_maxNumberOfCells;
}

void Controller::willDisplayCellAtLocation(HighlightCell * cell, int i, int j) {
  AppCell * appCell = static_cast<AppCell *>(cell);
  AppsContainer * container = AppsContainer::sharedAppsContainer();
  int appIndex = (j * k_numberOfColumns + i) + 1;
  if (appIndex >= container->numberOfApps()) {
    appCell->setVisible(false);
  } else {
    appCell->setVisible(true);
    if (appIndex < container->numberOfBuiltinApps()) {
      const ::App::Descriptor * descriptor = container->appSnapshotAtIndex(PermutedAppSnapshotIndex(appIndex))->descriptor();
      appCell->setBuiltinAppDescriptor(descriptor);
    } else {
      Ion::ExternalApps::App a = container->externalAppAtIndex(appIndex - container->numberOfBuiltinApps());
      appCell->setExternalApp(a);
    }
  }
}

int Controller::numberOfIcons() const {
  AppsContainer * container = AppsContainer::sharedAppsContainer();
  assert(container->numberOfApps() > 0);
  return container->numberOfApps() - 1;
}

void Controller::tableViewDidChangeSelection(SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY, bool withinTemporarySelection) {
  if (withinTemporarySelection) {
    return;
  }
  /* To prevent the selectable table view to select cells that are unvisible,
   * we reselect the previous selected cell as soon as the selected cell is
   * unvisible. This trick does not create an endless loop as we ensure not to
   * stay on a unvisible cell and to initialize the first cell on a visible one
   * (so the previous one is always visible). */
  int appIndex = (t->selectedColumn() + t->selectedRow() * k_numberOfColumns) + 1;
  if (appIndex >= AppsContainer::sharedAppsContainer()->numberOfApps()) {
    t->selectCellAtLocation(previousSelectedCellX, previousSelectedCellY);
  }
}

void Controller::tableViewDidChangeSelectionAndDidScroll(SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY, bool withinTemporarySelection) {
  if (withinTemporarySelection) {
    return;
  }
  /* If the number of apps (including home) is != 3*n+1, when we display the
   * lowest icons, the other(s) are empty. As no icon is thus redrawn on the
   * previous ones, the cell is not cleaned. We need to redraw a white rect on
   * the cells to hide the leftover icons. Ideally, we would have redrawn all
   * the background in white and then redraw visible cells. However, the
   * redrawing takes time and is visible at scrolling. Here, we avoid the
   * background complete redrawing but the code is a bit
   * clumsy. */
  if (t->selectedRow() == numberOfRows() - 1) {
    m_view.reloadBottomRow(this, AppsContainer::sharedAppsContainer()->numberOfApps() - 1, k_numberOfColumns);
  }
}

SelectableTableViewDataSource * Controller::selectionDataSource() const {
  return App::app()->snapshot();
}

void Controller::switchToSelectedApp() {
  AppsContainer * container = AppsContainer::sharedAppsContainer();
  int appIndex = selectionDataSource()->selectedRow() * k_numberOfColumns + selectionDataSource()->selectedColumn() + 1;
  Poincare::Preferences::ExamMode examMode = Poincare::Preferences::sharedPreferences()->examMode();
  if (appIndex < container->numberOfBuiltinApps()) {
    ::App::Snapshot * selectedSnapshot = container->appSnapshotAtIndex(PermutedAppSnapshotIndex(appIndex));
    if (ExamModeConfiguration::appIsForbidden(selectedSnapshot->descriptor()->name())) {
      App::app()->displayWarning(ExamModeConfiguration::forbiddenAppMessage(examMode, 0), ExamModeConfiguration::forbiddenAppMessage(examMode, 1));
    } else {
      container->switchToBuiltinApp(selectedSnapshot);
    }
  } else {
    if (examMode != Poincare::Preferences::ExamMode::Off) {
      App::app()->displayWarning(ExamModeConfiguration::forbiddenAppMessage(examMode, 0), ExamModeConfiguration::forbiddenAppMessage(examMode, 1));
    } else {
      m_view.reload();
      Ion::ExternalApps::App a = container->externalAppAtIndex(appIndex - container->numberOfBuiltinApps());
      container->switchToExternalApp(a);
    }
  }
}

}

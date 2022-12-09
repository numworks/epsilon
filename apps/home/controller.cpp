#include "controller.h"
#include "app.h"
#include <apps/home/apps_layout.h>
#include "../apps_container.h"
#include "../global_preferences.h"
#include "../exam_mode_configuration.h"

extern "C" {
#include <assert.h>
}

#ifdef HOME_DISPLAY_EXTERNALS
#include "../external/external_icon.h"
#include "../external/archive.h"
#include <string.h>
#endif

namespace Home {

Controller::ContentView::ContentView(Controller * controller, SelectableTableViewDataSource * selectionDataSource) :
  m_selectableTableView(controller, controller, &m_backgroundView, selectionDataSource, controller),
  m_backgroundView()
{
  m_selectableTableView.setVerticalCellOverlap(0);
  m_selectableTableView.setMargins(0, k_sideMargin, k_bottomMargin, k_sideMargin);
  static_cast<ScrollView::BarDecorator *>(m_selectableTableView.decorator())->verticalBar()->setMargin(k_indicatorMargin);
}

SelectableTableView * Controller::ContentView::selectableTableView() {
  return &m_selectableTableView;
}

void Controller::ContentView::drawRect(KDContext * ctx, KDRect rect) const {
  m_selectableTableView.drawRect(ctx, rect);
}

void Controller::ContentView::reloadBottomRow(SimpleTableViewDataSource * dataSource, int numberOfIcons, int numberOfColumns) {
  if (numberOfIcons % numberOfColumns) {
    /* We mark the missing icons on the last row as dirty. */
    for (int i = 0; i < numberOfColumns; i++) {
      if (i >= numberOfIcons % numberOfColumns) {
        markRectAsDirty(KDRect(dataSource->cellWidth()*i, dataSource->cellHeight(), dataSource->cellWidth(), dataSource->cellHeight()));
      }
    }
  }
}

BackgroundView * Controller::ContentView::backgroundView() {
  return &m_backgroundView;
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
  m_backgroundView.setFrame(KDRect(0, Metric::TitleBarHeight, Ion::Display::Width, Ion::Display::Height-Metric::TitleBarHeight), force);
  m_backgroundView.updateDataValidity();
}

Controller::Controller(Responder * parentResponder, SelectableTableViewDataSource * selectionDataSource, ::App * app) :
  ViewController(parentResponder),
  m_view(this, selectionDataSource)
{
  m_app = app;
  for (int i = 0; i < k_maxNumberOfCells; i++) {
    m_cells[i].setBackgroundView(m_view.backgroundView());
  }

  m_view.backgroundView()->setDefaultColor(Palette::HomeBackground);


#ifdef HOME_DISPLAY_EXTERNALS
    int index = External::Archive::indexFromName("wallpaper.obm");
    if(index > -1) {
      External::Archive::File image;
      External::Archive::fileAtIndex(index, image);
      m_view.backgroundView()->setBackgroundImage(image.data);
    }
#endif
}

static constexpr Ion::Events::Event home_fast_navigation_events[] = {
    Ion::Events::Seven, Ion::Events::Eight, Ion::Events::Nine,
    Ion::Events::Four, Ion::Events::Five, Ion::Events::Six,
    Ion::Events::One, Ion::Events::Two, Ion::Events::Three,
    Ion::Events::Zero, Ion::Events::Dot, Ion::Events::EE
};

bool Controller::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    AppsContainer * container = AppsContainer::sharedAppsContainer();

    int index = selectionDataSource()->selectedRow()*k_numberOfColumns+selectionDataSource()->selectedColumn()+1;
#ifdef HOME_DISPLAY_EXTERNALS
    if (index >= container->numberOfApps()) {
      if (GlobalPreferences::sharedGlobalPreferences()->examMode() == GlobalPreferences::ExamMode::Dutch || GlobalPreferences::sharedGlobalPreferences()->examMode() == GlobalPreferences::ExamMode::NoSymNoText || GlobalPreferences::sharedGlobalPreferences()->examMode() == GlobalPreferences::ExamMode::NoSym) {
        App::app()->displayWarning(I18n::Message::ForbiddenAppInExamMode1, I18n::Message::ForbiddenAppInExamMode2);
      } else {
        External::Archive::File executable;
        if (External::Archive::executableAtIndex(index - container->numberOfApps(), executable)) {
          uint32_t res = External::Archive::executeFile(executable.name, ((App *)m_app)->heap(), ((App *)m_app)->heapSize());
          ((App*)m_app)->redraw();
          switch(res) {
            case 0:
              break;
            case 1:
              Container::activeApp()->displayWarning(I18n::Message::ExternalAppApiMismatch);
              break;
            case 2:
              Container::activeApp()->displayWarning(I18n::Message::StorageMemoryFull1);
              break;
            default:
              Container::activeApp()->displayWarning(I18n::Message::ExternalAppExecError);
              break;
          }
          return true;
        }
      }
    } else {
#endif
    ::App::Snapshot * selectedSnapshot = container->appSnapshotAtIndex(index);
    if (ExamModeConfiguration::appIsForbiddenInExamMode(selectedSnapshot->descriptor()->examinationLevel(), GlobalPreferences::sharedGlobalPreferences()->examMode())) {
      App::app()->displayWarning(I18n::Message::ForbiddenAppInExamMode1, I18n::Message::ForbiddenAppInExamMode2);
    } else {
      bool switched = container->switchTo(selectedSnapshot);
      assert(switched);
      (void) switched; // Silence compilation warning about unused variable.
    }
#ifdef HOME_DISPLAY_EXTERNALS
    }
#endif
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

  // Handle fast home navigation
  for(int i = 0; i < std::min((int) (sizeof(home_fast_navigation_events) / sizeof(Ion::Events::Event)), this->numberOfIcons()); i++) {
    if (event == home_fast_navigation_events[i]) {
      int row = i / k_numberOfColumns;
      int column = i % k_numberOfColumns;
      // Get if app is already selected
      if (selectionDataSource()->selectedRow() == row && selectionDataSource()->selectedColumn() == column) {
        // If app is already selected, launch it
        return handleEvent(Ion::Events::OK);
      }
      // Else, select the app
      return m_view.selectableTableView()->selectCellAtLocation(column, row);
    }
  }

  return false;
}

void Controller::didBecomeFirstResponder() {
  if (selectionDataSource()->selectedRow() == -1) {
    selectionDataSource()->selectCellAtLocation(0, 0);
  }
  Container::activeApp()->setFirstResponder(m_view.selectableTableView());
}

void Controller::viewWillAppear() {
  KDIonContext::sharedContext()->zoomInhibit = true;
  KDIonContext::sharedContext()->updatePostProcessingEffects();
}

void Controller::viewDidDisappear() {
  KDIonContext::sharedContext()->zoomInhibit = false;
  KDIonContext::sharedContext()->updatePostProcessingEffects();
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

KDCoordinate Controller::cellHeight() {
  return k_cellHeight;
}

KDCoordinate Controller::cellWidth() {
  return k_cellWidth;
}

HighlightCell * Controller::reusableCell(int index) {
  return &m_cells[index];
}

int Controller::reusableCellCount() const {
  return k_maxNumberOfCells;
}

void Controller::willDisplayCellAtLocation(HighlightCell * cell, int i, int j) {
  AppCell * appCell = (AppCell *)cell;
  AppsContainer * container = AppsContainer::sharedAppsContainer();
  int appIndex = (j * k_numberOfColumns + i) + 1;
  if (appIndex >= container->numberOfApps()) {
#ifdef HOME_DISPLAY_EXTERNALS
    External::Archive::File app_file;


    if (External::Archive::executableAtIndex(appIndex - container->numberOfApps(), app_file)) {
      char temp_name_buffer[100];
      strlcpy(temp_name_buffer, app_file.name, 94);
      strlcat(temp_name_buffer, ".icon", 99);

      int img_index = External::Archive::indexFromName(temp_name_buffer);

      if (img_index != -1) {
        External::Archive::File image_file;
        if (External::Archive::fileAtIndex(img_index, image_file)) {
          // const Image* img = new Image(55, 56, image_file.data, image_file.dataLength);
          appCell->setExtAppDescriptor(app_file.name, image_file.data, image_file.dataLength);
        } else {
          appCell->setExtAppDescriptor(app_file.name, ImageStore::ExternalIcon);
        }
      } else {
        appCell->setExtAppDescriptor(app_file.name, ImageStore::ExternalIcon);
      }

      appCell->setVisible(true);
    } else {
      appCell->setVisible(false);
    }
#else
    appCell->setVisible(false);
#endif
  } else {
    appCell->setVisible(true);
    ::App::Descriptor * descriptor = container->appSnapshotAtIndex(PermutedAppSnapshotIndex(appIndex))->descriptor();
    appCell->setAppDescriptor(descriptor);
  }
}

int Controller::numberOfIcons() const {
  AppsContainer * container = AppsContainer::sharedAppsContainer();
  assert(container->numberOfApps() > 0);
#ifdef HOME_DISPLAY_EXTERNALS
  return container->numberOfApps() - 1 + External::Archive::numberOfExecutables();
#else
  return container->numberOfApps() - 1;
#endif
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
  int appIndex = (t->selectedColumn()+t->selectedRow()*k_numberOfColumns)+1;
  if (appIndex >= this->numberOfIcons()+1) {
    t->selectCellAtLocation((this->numberOfIcons()%3)-1, (this->numberOfIcons() / k_numberOfColumns));
  }
}

void Controller::tableViewDidChangeSelectionAndDidScroll(SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY, bool withinTemporarySelection) {
  /* If the number of apps (including home) is != 3*n+1, when we display the
   * lowest icons, the other(s) are empty. As no icon is thus redrawn on the
   * previous ones, the cell is not cleaned. We need to redraw a white rect on
   * the cells to hide the leftover icons. Ideally, we would have redrawn all
   * the background in white and then redraw visible cells. However, the
   * redrawing takes time and is visible at scrolling. Here, we avoid the
   * background complete redrawing but the code is a bit
   * clumsy. */
  if (t->selectedRow() == numberOfRows()-1) {
    m_view.reloadBottomRow(this, this->numberOfIcons(), k_numberOfColumns);
  }
}

SelectableTableViewDataSource * Controller::selectionDataSource() const {
  return App::app()->snapshot();
}

}

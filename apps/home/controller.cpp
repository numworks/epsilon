#include "controller.h"
#include "../apps_container.h"
extern "C" {
#include <assert.h>
}

namespace Home {

Controller::ContentView::ContentView(Controller * controller, SelectableTableViewDataSource * selectionDataSource) :
  m_selectableTableView(controller, controller, 0, 0, 0, k_sideMargin, 0, k_sideMargin, selectionDataSource, controller, true, false,
    KDColorBlack, k_indicatorThickness, Palette::GreyDark, Palette::GreyMiddle, k_indicatorMargin)
{
}

SelectableTableView * Controller::ContentView::selectableTableView() {
  return &m_selectableTableView;
}

void Controller::ContentView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(bounds(), KDColorWhite);
}

void Controller::ContentView::reloadBottomRightCorner(SimpleTableViewDataSource * dataSource) {
  /* We mark the bottom right corner (where an empty space can be) as dirty. */
  markRectAsDirty(KDRect(dataSource->cellWidth()*2, dataSource->cellHeight(), dataSource->cellWidth(), dataSource->cellHeight()));
}

int Controller::ContentView::numberOfSubviews() const {
  return 1;
}

View * Controller::ContentView::subviewAtIndex(int index) {
  assert(index == 0);
  return &m_selectableTableView;
}

void Controller::ContentView::layoutSubviews() {
  m_selectableTableView.setFrame(bounds());
}

Controller::Controller(Responder * parentResponder, ::AppsContainer * container, SelectableTableViewDataSource * selectionDataSource) :
  ViewController(parentResponder),
  m_container(container),
  m_view(this, selectionDataSource),
  m_selectionDataSource(selectionDataSource)
{
}

bool Controller::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    m_container->switchTo(m_container->appSnapshotAtIndex(m_selectionDataSource->selectedRow()*k_numberOfColumns+m_selectionDataSource->selectedColumn()+1));
    return true;
  }

  if (event == Ion::Events::Home || event == Ion::Events::Back) {
    return m_view.selectableTableView()->selectCellAtLocation(0,0);
  } 

  return false;
}

void Controller::didBecomeFirstResponder() {
  if (m_selectionDataSource->selectedRow() == -1) {
    m_selectionDataSource->selectCellAtLocation(0, 0);
  } else {
    m_selectionDataSource->selectCellAtLocation(m_selectionDataSource->selectedColumn(), m_selectionDataSource->selectedRow());
  }
  app()->setFirstResponder(m_view.selectableTableView());
}

void Controller::viewWillAppear() {
}

View * Controller::view() {
  return &m_view;
}

int Controller::numberOfRows() {
  return ((numberOfIcons()-1)/k_numberOfColumns)+1;
}

int Controller::numberOfColumns() {
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

int Controller::reusableCellCount() {
  return k_maxNumberOfCells;
}

void Controller::willDisplayCellAtLocation(HighlightCell * cell, int i, int j) {
  AppCell * appCell = (AppCell *)cell;
  int appIndex = (j*k_numberOfColumns+i)+1;
  if (appIndex >= m_container->numberOfApps()) {
    appCell->setVisible(false);
  } else {
    appCell->setVisible(true);
    ::App::Descriptor * descriptor = m_container->appSnapshotAtIndex((j*k_numberOfColumns+i)+1)->descriptor();
    appCell->setAppDescriptor(descriptor);
  }
}

int Controller::numberOfIcons() {
  assert(m_container->numberOfApps() > 0);
  return m_container->numberOfApps() - 1;
}

void Controller::tableViewDidChangeSelection(SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY) {
  /* If the number of apps (including home) is odd, when we display the
   * rightest icon, the icon below is empty. As no icon is thus redrawn on the
   * previous one, the cell is not cleaned. We need to redraw a white rect on
   * the cell to hide the dirtyness below. Ideally, we would have redrawn all
   * the background in white and then redraw visible cells. However, the
   * redrawing takes time and is visible at scrolling. Here, we avoid the
   * background complete redrawing but the code is a bit
   * clumsy. */
  if (m_container->numberOfApps()%2 == 0 && t->selectedColumn() == k_numberOfColumns -1) {
    m_view.reloadBottomRightCorner(this);
  }
  /* To prevent the selectable table view to select cells that are unvisible,
   * we reselect the previous selected cell as soon as the selected cell is
   * unvisible. This trick does not create an endless loop as we ensure not to
   * stay on a unvisible cell and to initialize the first cell on a visible one
   * (so the previous one is always visible). */
  int appIndex = (t->selectedRow()*k_numberOfColumns+t->selectedColumn())+1;
  if (appIndex >= m_container->numberOfApps()) {
    t->selectCellAtLocation(previousSelectedCellX, previousSelectedCellY);
  }
}

}

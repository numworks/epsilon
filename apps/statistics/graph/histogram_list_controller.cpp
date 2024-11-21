#include "histogram_list_controller.h"

#include "../app.h"
#include "statistics/graph/histogram_view.h"

namespace Statistics {

Escher::SolidColorCell makeColorCell(size_t index) {
  return Escher::SolidColorCell();
}

HistogramListController::HistogramListController(
    Escher::Responder* parentResponder, Store* store,
    Shared::CurveViewRange* histogramRange)
    : Escher::SelectableListViewController<Escher::ListViewDataSource>(
          parentResponder, this),
      m_displayCells({HistogramCell(HistogramView(store, 0, histogramRange)),
                      HistogramCell(HistogramView(store, 1, histogramRange)),
                      HistogramCell(HistogramView(store, 2, histogramRange)),
                      HistogramCell(HistogramView(store, 3, histogramRange))}),
      m_store(store),
      m_histogramRange(store) {}

Escher::HighlightCell* HistogramListController::reusableCell(int index,
                                                             int type) {
  assert(type == 0);
  assert(index >= 0 && index < std::size(m_displayCells));
  return &m_displayCells[index];
}

void HistogramListController::fillCellForRow(Escher::HighlightCell* cell,
                                             int row) {
  assert(row >= 0 && row < numberOfRows());
  HistogramCell* histogramCell = static_cast<HistogramCell*>(cell);
  histogramCell->setSeries(row);
  histogramCell->reload();
}

bool HistogramListController::handleEvent(Ion::Events::Event event) {
  // Handle left/right navigation inside a histogram cell
  if (event == Ion::Events::Left || event == Ion::Events::Right) {
    // The following function will set a new seriesIndex.
    moveSelectionHorizontally(event.direction());
    return true;
  }

  if (!m_selectableListView.handleEvent(event)) {
    return false;
  }

  if (hasSelectedCell()) {
    /* If the SelectableListView handled the event by selecting a new cell,
     * then it took the firstResponder ownership. However we want
     * HistogramMainController to be the first responder, because the banner
     * view need to be updated as well. So the firstResponder ownership is
     * given back to HistogramMainController, which is the parent responder of
     * HistogramListController. */
    Escher::App::app()->setFirstResponder(parentResponder());

    /* Because SelectableListView lost the firstResponder ownership, the
     * SelectableTableView::willExitResponderChain function was called. This
     * function unhighlights the selected cell, so we need to set it
     * highlighted again. */
    setSelectedCellHighlight(true);

    // Set the current series and index in the snaphot
    setSelectedSeries(m_selectableListView.selectedRow());
    /* The series index of the new selected cell is computed to be close to its
     * previous location in the neighbouring cell */
    setSelectedSeriesIndex(
        sanitizeSelectedIndex(selectedSeries(), selectedSeriesIndex()));

    m_histogramRange.scrollToSelectedBarIndex(selectedSeries(),
                                              selectedSeriesIndex());
  }

  return true;
}

void HistogramListController::selectFirstCell() {
  /* Three actions are needed: selecting the first row in SelectableListView,
   * highlighting the selected cell, and updating the selected series in the
   * snapshot */
  m_selectableListView.selectFirstRow();
  setSelectedCellHighlight(true);
  // Set the current series and index in the snaphot
  setSelectedSeries(m_selectableListView.selectedRow());
  setSelectedSeriesIndex(0);
}

std::size_t HistogramListController::selectedSeries() const {
  int series = *App::app()->snapshot()->selectedSeries();
  assert(0 <= series && series <= m_store->numberOfActiveSeries());
  return static_cast<std::size_t>(series);
}

void HistogramListController::setSelectedSeries(std::size_t selectedSeries) {
  assert(selectedSeries <= m_store->numberOfActiveSeries());
  *App::app()->snapshot()->selectedSeries() = selectedSeries;
}

std::size_t HistogramListController::selectedSeriesIndex() const {
  int index = *App::app()->snapshot()->selectedIndex();
  assert(0 <= index);
  // TODO: check the index upper bound
  return static_cast<std::size_t>(index);
}

void HistogramListController::setSelectedSeriesIndex(
    std::size_t selectedIndex) {
  // TODO: check the index upper bound
  *App::app()->snapshot()->selectedIndex() = selectedIndex;
}

bool HistogramListController::moveSelectionHorizontally(
    OMG::HorizontalDirection direction) {
  int numberOfBars = m_store->numberOfBars(selectedSeries());
  int newSelectedBarIndex = selectedSeriesIndex();
  do {
    newSelectedBarIndex += direction.isRight() ? 1 : -1;
  } while (newSelectedBarIndex >= 0 && newSelectedBarIndex < numberOfBars &&
           m_store->heightOfBarAtIndex(selectedSeries(), newSelectedBarIndex) ==
               0);

  if (newSelectedBarIndex >= 0 && newSelectedBarIndex < numberOfBars &&
      selectedSeriesIndex() != newSelectedBarIndex) {
    setSelectedSeriesIndex(newSelectedBarIndex);

    // TODO: handle histogram bar highlight
    return true;
  }
  return false;
}

std::size_t HistogramListController::sanitizeSelectedIndex(
    std::size_t selectedSeries, std::size_t previousIndex) const {
  assert(m_store->seriesIsActive(selectedSeries));

  std::size_t selectedIndex = previousIndex;

  if (m_store->heightOfBarAtIndex(selectedSeries, selectedIndex) != 0) {
    return selectedIndex;
  }
  int numberOfBars = m_store->numberOfBars(selectedSeries);
  // search a bar with non null height left of the selected one
  while (m_store->heightOfBarAtIndex(selectedSeries, selectedIndex) == 0 &&
         selectedIndex >= 0) {
    selectedIndex -= 1;
  }
  if (selectedIndex < 0) {
    // search a bar with non null height right of the selected one
    selectedIndex = previousIndex + 1;
    while (m_store->heightOfBarAtIndex(selectedSeries, selectedIndex) == 0 &&
           selectedIndex < numberOfBars) {
      selectedIndex += 1;
    }
  }
  assert(selectedIndex < numberOfBars);
  return selectedIndex;
}

}  // namespace Statistics

#include "histogram_list_controller.h"

#include "../app.h"
#include "statistics/graph/histogram_view.h"

namespace Statistics {

HistogramListController::HistogramListController(
    Escher::Responder* parentResponder, Store* store,
    HistogramRange* histogramRange)
    : Escher::SelectableListViewController<Escher::ListViewDataSource>(
          parentResponder, this),
      m_displayCells({HistogramCell(HistogramView(store, histogramRange)),
                      HistogramCell(HistogramView(store, histogramRange)),
                      HistogramCell(HistogramView(store, histogramRange)),
                      HistogramCell(HistogramView(store, histogramRange))}),
      m_store(store),
      m_histogramRange(histogramRange) {
  m_selectableListView.resetMargins();
}

Escher::HighlightCell* HistogramListController::reusableCell(int index,
                                                             int type) {
  assert(type == 0);
  assert(index >= 0 && index < m_displayCells.size());
  return &m_displayCells[index];
}

void HistogramListController::fillCellForRow(Escher::HighlightCell* cell,
                                             int row) {
  assert(row >= 0 && row < numberOfRows());
  HistogramCell* histogramCell = static_cast<HistogramCell*>(cell);
  histogramCell->setSeries(m_store->seriesIndexFromActiveSeriesIndex(row));
}

bool HistogramListController::handleEvent(Ion::Events::Event event) {
  // Handle left/right navigation inside a histogram cell
  if (event == Ion::Events::Left || event == Ion::Events::Right) {
    moveSelectionHorizontally(event.direction());
    return true;
  }

  int previousSelectedRow = selectedRow();
  if (!m_selectableListView.handleEvent(event)) {
    return false;
  }
  if (selectedRow() != previousSelectedRow) {
    /* If the SelectableListView handled the event by selecting a new row,
     * then it took the firstResponder ownership. However we want the main
     * controller to be the first responder, because the banner view needs to
     * be updated as well. So the firstResponder ownership is given back to
     * the main controller, which is the parent responder of
     * HistogramListController. */
    Escher::App::app()->setFirstResponder(parentResponder());

    // Set the current series and index in the snapshot
    size_t previousSelectedSeries = selectedSeries();
    setSelectedSeries(selectedRow());
    /* The series index of the new selected cell is computed to be close to
     * its previous location in the neighboring cell */
    setSelectedBarIndex(barIndexAfterSelectingNewSeries(
        previousSelectedSeries, selectedSeries(), unsafeSelectedBarIndex()));

    // Update row and bar highlights
    highlightRow(selectedSeries());
    scrollAndHighlightHistogramBar(selectedRow(), selectedBarIndex());
  }

  return true;
}

void HistogramListController::processSeriesAndBarSelection() {
  if (!hasSelectedSeries()) {
    setSelectedSeries(0);
    setSelectedBarIndex(0);
  }

  /* If the number of histogram bars has been changed by the user and there are
   * less bars, the selected bar index can become out of range. We need to clamp
   * this index to the last bar. */
  size_t numberOfBars = m_store->numberOfBars(selectedSeries());
  if (unsafeSelectedBarIndex() >= numberOfBars) {
    setSelectedBarIndex(numberOfBars - 1);
  }

  /* Sanitize selected index so that the selected bar is never empty */
  setSelectedBarIndex(
      sanitizeSelectedIndex(selectedSeries(), selectedBarIndex()));

#if defined(ASSERTIONS)
  // Check that selectedSeries() and selectedBarIndex() do not throw an assert
  selectedSeries();
  selectedBarIndex();
#endif
}

void HistogramListController::highlightRow(size_t selectedSeries) {
  int rowOfSelectedSeries =
      m_store->activeSeriesIndexFromSeriesIndex(selectedSeries);
  assert(0 <= rowOfSelectedSeries &&
         rowOfSelectedSeries <= m_selectableListView.totalNumberOfRows());

  if (m_selectableListView.selectedCell()) {
    /* If a cell is already selected in the list, assert it is the row
     * corresponding to the selected series */
    assert(selectedRow() == rowOfSelectedSeries);
  } else {
    // Set the cell to "selected" state in the SelectedListView
    m_selectableListView.selectCell(rowOfSelectedSeries);
    /* The SelectableListView took the firstResponder ownership when selecting
     * the cell. However we want the main controller to be the first
     * responder, because the banner view needs to be updated as well. So the
     * firstResponder ownership is given back to the main controller, which
     * is the parent responder of HistogramListController. */
    Escher::App::app()->setFirstResponder(parentResponder());
  }

  /* Highlight the selected cell. Not that the cell could be selected in the
   * list but not highlighted */
  m_selectableListView.selectedCell()->setHighlighted(true);
}

void HistogramListController::scrollAndHighlightHistogramBar(size_t row,
                                                             size_t barIndex) {
  assert(0 <= row && row <= m_store->numberOfActiveSeries());

  int seriesAtRow = m_store->seriesIndexFromActiveSeriesIndex(row);

  assert(0 <= barIndex && barIndex < m_store->numberOfBars(seriesAtRow));

  /* Update the histogram x-axis range to adapt to the bar index. WARNING: the
   * range update must be done before setting the bar highlight, because the bar
   * has to be visible when calling setBarHighlight. */
  if (m_histogramRange->scrollToSelectedBarIndex(seriesAtRow, barIndex)) {
    m_selectableListView.selectedCell()->reloadCell();
  }

  /* The following function will set the bar highlight in the HistogramView
   * owned by the cell */
  static_cast<HistogramCell*>(m_selectableListView.cell(row))
      ->setBarHighlight(m_store->startOfBarAtIndex(seriesAtRow, barIndex),
                        m_store->endOfBarAtIndex(seriesAtRow, barIndex));
}

size_t HistogramListController::selectedSeries() const {
  /* The selectedSeries() method from the snapshot returns the index of the
   * selected series considering ACTIVE series only */
  int selectedActiveSeries = *App::app()->snapshot()->selectedSeries();
  assert(0 <= selectedActiveSeries &&
         selectedActiveSeries < m_store->numberOfActiveSeries());
  int series = m_store->seriesIndexFromActiveSeriesIndex(selectedActiveSeries);
  assert(0 <= series && series < Store::k_numberOfSeries);
  return static_cast<size_t>(series);
}

void HistogramListController::setSelectedSeries(size_t activeSelectedSeries) {
  assert(activeSelectedSeries < m_store->numberOfActiveSeries());
  *App::app()->snapshot()->selectedSeries() = activeSelectedSeries;
}

size_t HistogramListController::unsafeSelectedBarIndex() const {
  int barIndex = *App::app()->snapshot()->selectedIndex();
  assert(barIndex >= 0);
  return static_cast<size_t>(barIndex);
}

size_t HistogramListController::selectedBarIndex() const {
  size_t barIndex = unsafeSelectedBarIndex();
  assert(barIndex < m_store->numberOfBars(selectedSeries()));
  return barIndex;
}

void HistogramListController::setSelectedBarIndex(size_t barIndex) {
  assert(barIndex < m_store->numberOfBars(selectedSeries()));
  *App::app()->snapshot()->selectedIndex() = barIndex;
}

bool HistogramListController::hasSelectedSeries() const {
  return *App::app()->snapshot()->selectedSeries() > -1;
}

bool HistogramListController::moveSelectionHorizontally(
    OMG::HorizontalDirection direction) {
  int numberOfBars = m_store->numberOfBars(selectedSeries());

  int newBarIndex = selectedBarIndex();
  do {
    newBarIndex += direction.isRight() ? 1 : -1;
    if (newBarIndex < 0) {
      return size_t{0};
    }
    if (newBarIndex >= numberOfBars) {
      return false;
    }
  } while (m_store->heightOfBarAtIndex(selectedSeries(), newBarIndex) == 0);

  assert(newBarIndex != selectedBarIndex());
  setSelectedBarIndex(static_cast<size_t>(newBarIndex));
  scrollAndHighlightHistogramBar(selectedRow(), selectedBarIndex());

  return true;
}

size_t HistogramListController::sanitizeSelectedIndex(
    size_t selectedSeries, size_t previousIndex) const {
  assert(m_store->seriesIsActive(selectedSeries));

  size_t selectedIndex = previousIndex;

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

size_t HistogramListController::barIndexAfterSelectingNewSeries(
    size_t previousSelectedSeries, size_t currentSelectedSeries,
    size_t previousSelectedBarIndex) const {
  /* In the simple following case, when all bars are aligned, the selected
   * index should not change:
   *           _ _ _ _
   * series1: | | | | |
   *             ^ selected index = 1
   *           _ _ _ _
   * series2: | | | | |
   *             ^ select index 1 when moving down
   *
   * But in the case where bars do not start on the same spot, selected index
   * should be offsetted so that you always select the bar just above or under
   * the previously selected one:
   *           _ _ _ _
   * series1: | | | | |
   *             ^ selected index = 1
   *             _ _ _
   * series2:   | | | |
   *             ^ select index 0 when moving down
   *
   * At the end of this method, the selected index should be sanitized so that
   * an empty bar is never selected:
   *           _ _ _ _
   * series1: | | | | |
   *             ^ selected index = 1
   *           _     _
   * series2: | |_ _| |
   *           ^ select index 0 when moving down
   * */
  double startDifference =
      m_store->startOfBarAtIndex(previousSelectedSeries, 0) -
      m_store->startOfBarAtIndex(currentSelectedSeries, 0);
  size_t newSelectedBarIndex =
      (previousSelectedBarIndex +
       static_cast<int>(startDifference / m_store->barWidth()));
  newSelectedBarIndex =
      std::max(std::min(static_cast<int>(newSelectedBarIndex),
                        m_store->numberOfBars(currentSelectedSeries) - 1),
               0);
  return sanitizeSelectedIndex(currentSelectedSeries, newSelectedBarIndex);
}

}  // namespace Statistics

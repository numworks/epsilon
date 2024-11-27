#include "histogram_list_controller.h"

#include "../app.h"
#include "statistics/graph/histogram_view.h"

namespace Statistics {

HistogramListController::HistogramListController(
    Escher::Responder* parentResponder, Store* store,
    HistogramRange* histogramRange)
    : Escher::SelectableListViewController<Escher::ListViewDataSource>(
          parentResponder, this),
      m_displayCells({HistogramCell(HistogramView(store, 0, histogramRange)),
                      HistogramCell(HistogramView(store, 1, histogramRange)),
                      HistogramCell(HistogramView(store, 2, histogramRange)),
                      HistogramCell(HistogramView(store, 3, histogramRange))}),
      m_store(store),
      m_histogramRange(histogramRange) {
  m_selectableListView.resetMargins();
}

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
    setSelectedBarIndex(moveSelectionHorizontally(
        selectedBarIndex(), selectedSeries(), event.direction()));
    highlightHistogramBar(selectedSeries(), selectedBarIndex());
    return true;
  }

  if (!m_selectableListView.handleEvent(event)) {
    return false;
  }

  if (selectedRow() != selectedSeries()) {
    /* If the SelectableListView handled the event by selecting a new row,
     * then it took the firstResponder ownership. However we want the main
     * controller to be the first responder, because the banner view needs to be
     * updated as well. So the firstResponder ownership is given back to the
     * main controller, which is the parent responder of
     * HistogramListController. */
    Escher::App::app()->setFirstResponder(parentResponder());

    // Set the current series and index in the snaphot
    std::size_t previousSelectedSeries = selectedSeries();
    setSelectedSeries(m_selectableListView.selectedRow());
    /* The series index of the new selected cell is computed to be close to its
     * previous location in the neighbouring cell */
    setSelectedBarIndex(barIndexAfterSelectingNewSeries(
        previousSelectedSeries, selectedSeries(), unsafeSelectedBarIndex()));

    highlightRow(selectedSeries());
    highlightHistogramBar(selectedSeries(), selectedBarIndex());
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
  std::size_t numberOfBars = m_store->numberOfBars(selectedSeries());
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

void HistogramListController::highlightRow(std::size_t row) {
  assert(0 <= row && row <= m_selectableListView.totalNumberOfRows());

  if (!m_selectableListView.selectedCell()) {
    // Set the cell to "selected" state in the SelectedListView
    m_selectableListView.selectCell(row);
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

void HistogramListController::highlightHistogramBar(std::size_t row,
                                                    std::size_t barIndex) {
  assert(0 <= row && row <= m_store->numberOfActiveSeries());
  assert(0 <= barIndex && barIndex < m_store->numberOfBars(row));
  static_cast<HistogramCell*>(m_selectableListView.cell(row))
      ->setBarHighlight(m_store->startOfBarAtIndex(row, barIndex),
                        m_store->endOfBarAtIndex(row, barIndex));
}

std::size_t HistogramListController::selectedSeries() const {
  int series = *App::app()->snapshot()->selectedSeries();
  assert(0 <= series && series < m_store->numberOfActiveSeries());
  return static_cast<std::size_t>(series);
}

void HistogramListController::setSelectedSeries(std::size_t selectedSeries) {
  assert(selectedSeries < m_store->numberOfActiveSeries());
  *App::app()->snapshot()->selectedSeries() = selectedSeries;
}

std::size_t HistogramListController::unsafeSelectedBarIndex() const {
  int barIndex = *App::app()->snapshot()->selectedIndex();
  assert(barIndex >= 0);
  return static_cast<std::size_t>(barIndex);
}

std::size_t HistogramListController::selectedBarIndex() const {
  std::size_t barIndex = unsafeSelectedBarIndex();
  assert(barIndex < m_store->numberOfBars(selectedSeries()));
  return barIndex;
}

void HistogramListController::setSelectedBarIndex(std::size_t barIndex) {
  assert(barIndex < m_store->numberOfBars(selectedSeries()));
  // update value in snapshot
  *App::app()->snapshot()->selectedIndex() = barIndex;
  // update histogram range
  if (m_histogramRange->scrollToSelectedBarIndex(selectedSeries(), barIndex)) {
    m_selectableListView.cell(selectedSeries())->reloadCell();
  }
}

bool HistogramListController::hasSelectedSeries() const {
  return *App::app()->snapshot()->selectedSeries() > -1;
}

std::size_t HistogramListController::moveSelectionHorizontally(
    std::size_t previousBarIndex, std::size_t selectedSeries,
    OMG::HorizontalDirection direction) const {
  int newBarIndex = previousBarIndex;

  do {
    newBarIndex += direction.isRight() ? 1 : -1;
    if (newBarIndex < 0) {
      return std::size_t{0};
    }
    if (newBarIndex >= m_store->numberOfBars(selectedSeries)) {
      return static_cast<std::size_t>(m_store->numberOfBars(selectedSeries) -
                                      1);
    }
  } while (m_store->heightOfBarAtIndex(selectedSeries, newBarIndex) == 0);
  return static_cast<std::size_t>(newBarIndex);
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

std::size_t HistogramListController::barIndexAfterSelectingNewSeries(
    std::size_t previousSelectedSeries, std::size_t currentSelectedSeries,
    std::size_t previousSelectedBarIndex) const {
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
  std::size_t newSelectedBarIndex =
      (previousSelectedBarIndex +
       static_cast<int>(startDifference / m_store->barWidth()));
  newSelectedBarIndex =
      std::max(std::min(static_cast<int>(newSelectedBarIndex),
                        m_store->numberOfBars(currentSelectedSeries) - 1),
               0);
  return sanitizeSelectedIndex(currentSelectedSeries, newSelectedBarIndex);
}

}  // namespace Statistics

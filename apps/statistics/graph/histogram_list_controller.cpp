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

void HistogramListController::setMainControllerFirstResponder() const {
  /* The banner view, which is owned by the main controller needs to be updated
   * at the same time as the histogram list view. To ensure this, the
   * firstResponder ownership is given back to the main controller, which is the
   * parent responder of HistogramListController. */
  Escher::App::app()->setFirstResponder(parentResponder());
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
    /* If the SelectableListView selected a new row, then it took the
     * firstResponder ownership. We need to manually restore it. */
    setMainControllerFirstResponder();

    // Set the current series and index in the snapshot
    int8_t previousSelectedSeries = selectedSeries();
    setSelectedSeries(static_cast<int8_t>(
        m_store->seriesIndexFromActiveSeriesIndex(selectedRow())));
    /* The series index of the new selected cell is computed to be close to
     * its previous location in the neighboring cell */
    setSelectedBarIndex(barIndexAfterSelectingNewSeries(
        previousSelectedSeries, selectedSeries(), unsafeSelectedBarIndex()));

    // Update row and bar highlights
    highlightSelectedSeries();
    scrollAndHighlightHistogramBar(selectedRow(), selectedBarIndex());
  }

  return true;
}

void HistogramListController::processSeriesAndBarSelection() const {
  if (!hasSelectedSeries()) {
    setSelectedSeries(
        static_cast<int8_t>(m_store->seriesIndexFromActiveSeriesIndex(0)));
    setSelectedBarIndex(0);
  }

  /* If the number of histogram bars has been changed by the user and there are
   * less bars, the selected bar index can become out of range. We need to clamp
   * this index to the last bar. */
  assert(m_store->numberOfBars(selectedSeries()) < INT16_MAX);
  int numberOfBars = m_store->numberOfBars(selectedSeries());
  if (unsafeSelectedBarIndex() >= numberOfBars) {
    setSelectedBarIndex(static_cast<int16_t>(numberOfBars - 1));
  }

  /* Sanitize selected index so that the selected bar is never empty */
  setSelectedBarIndex(
      sanitizedSelectedIndex(selectedSeries(), selectedBarIndex()));

#if ASSERTIONS
  // Check that selectedSeries() and selectedBarIndex() do not throw an assert
  selectedSeries();
  selectedBarIndex();
#endif
}

void HistogramListController::highlightSelectedSeries() {
  assert(hasSelectedSeries());

  /* The series could be selected in the snapshot but the corresponding row in
   * the list could be unselected yet.  */
  if (selectedRow() >= 0) {
    assert(m_store->seriesIndexFromActiveSeriesIndex(selectedRow()) ==
           selectedSeries());
  } else {
    m_selectableListView.selectCell(
        m_store->activeSeriesIndexFromSeriesIndex(selectedSeries()));
    /* The SelectableListView took the firstResponder ownership when selecting
     * the cell. We need to manually restore it. */
    setMainControllerFirstResponder();
  }

  /* The cell corresponding to the selected series could be selected in the list
   * but not highlighted */
  m_selectableListView.selectedCell()->setHighlighted(true);
}

void HistogramListController::scrollAndHighlightHistogramBar(int row,
                                                             int16_t barIndex) {
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

int8_t HistogramListController::selectedSeries() const {
  int8_t series = App::app()->snapshot()->selectedSeries();
  assert(0 <= series && series < Store::k_numberOfSeries);
  return series;
}

void HistogramListController::setSelectedSeries(int8_t series) const {
  assert(series < m_store->k_numberOfSeries);
  App::app()->snapshot()->setSelectedSeries(series);
}

int16_t HistogramListController::unsafeSelectedBarIndex() const {
  return App::app()->snapshot()->selectedIndex();
}

int16_t HistogramListController::selectedBarIndex() const {
  int16_t barIndex = unsafeSelectedBarIndex();
  assert(barIndex < m_store->numberOfBars(selectedSeries()));
  return barIndex;
}

void HistogramListController::setSelectedBarIndex(int16_t barIndex) const {
  assert(barIndex < m_store->numberOfBars(selectedSeries()));
  App::app()->snapshot()->setSelectedIndex(barIndex);
}

bool HistogramListController::hasSelectedSeries() const {
  return App::app()->snapshot()->selectedSeries() > -1;
}

bool HistogramListController::moveSelectionHorizontally(
    OMG::HorizontalDirection direction) {
  int numberOfBars = m_store->numberOfBars(selectedSeries());

  int newBarIndex = selectedBarIndex();
  do {
    newBarIndex += direction.isRight() ? 1 : -1;
    if ((newBarIndex < 0) || (newBarIndex >= numberOfBars)) {
      return false;
    }
  } while (m_store->heightOfBarAtIndex(selectedSeries(), newBarIndex) == 0.0);

  assert(newBarIndex != selectedBarIndex());
  setSelectedBarIndex(static_cast<int16_t>(newBarIndex));
  scrollAndHighlightHistogramBar(selectedRow(), selectedBarIndex());

  return true;
}

int16_t HistogramListController::sanitizedSelectedIndex(
    int8_t selectedSeries, int16_t previousIndex) const {
  assert(m_store->seriesIsActive(selectedSeries));

  int16_t selectedIndex = previousIndex;

  if (m_store->heightOfBarAtIndex(selectedSeries, selectedIndex) != 0.0) {
    return selectedIndex;
  }
  int numberOfBars = m_store->numberOfBars(selectedSeries);
  // search a bar with non null height left of the selected one
  while (m_store->heightOfBarAtIndex(selectedSeries, selectedIndex) == 0.0 &&
         selectedIndex >= 0) {
    selectedIndex -= 1;
  }
  if (selectedIndex < 0) {
    // search a bar with non null height right of the selected one
    selectedIndex = previousIndex + 1;
    while (m_store->heightOfBarAtIndex(selectedSeries, selectedIndex) == 0.0 &&
           selectedIndex < numberOfBars) {
      selectedIndex += 1;
    }
  }
  assert(selectedIndex < numberOfBars);
  return selectedIndex;
}

int16_t HistogramListController::barIndexAfterSelectingNewSeries(
    int8_t previousSelectedSeries, int8_t currentSelectedSeries,
    int16_t previousSelectedBarIndex) const {
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
  int16_t newSelectedBarIndex =
      (previousSelectedBarIndex +
       static_cast<int16_t>(startDifference / m_store->barWidth()));
  newSelectedBarIndex =
      std::max(std::min(newSelectedBarIndex,
                        static_cast<int16_t>(
                            m_store->numberOfBars(currentSelectedSeries) - 1)),
               int16_t{0});
  return sanitizedSelectedIndex(currentSelectedSeries, newSelectedBarIndex);
}

}  // namespace Statistics

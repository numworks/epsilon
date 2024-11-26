#ifndef STATISTICS_HISTOGRAM_LIST_CONTROLLER_H
#define STATISTICS_HISTOGRAM_LIST_CONTROLLER_H

#include <escher/list_view_data_source.h>
#include <escher/selectable_list_view_controller.h>

#include "../store.h"
#include "histogram_range.h"
#include "histogram_view.h"

namespace Statistics {

class HistogramListController
    : public Escher::SelectableListViewController<Escher::ListViewDataSource>,
      public Escher::SelectableListViewDelegate {
 public:
  HistogramListController(Escher::Responder* parentResponder, Store* store,
                          HistogramRange* histogramRange);

  static constexpr KDCoordinate k_listRowHeight = 75;

  KDCoordinate histogramHeight() const {
    return numberOfRows() == 1 ? m_selectableListView.bounds().height() - 1
                               : k_listRowHeight;
  }

  // Escher::TableViewDataSource
  int numberOfRows() const override { return m_store->numberOfActiveSeries(); };

  // Escher::ListViewDataSource
  void fillCellForRow(Escher::HighlightCell* cell, int row) override;
  int typeAtRow(int row) const override { return 0; }
  Escher::HighlightCell* reusableCell(int index, int type) override;
  int reusableCellCount(int type) const override {
    return m_displayCells.size();
    ;
  }

  // Escher::Responder
  bool handleEvent(Ion::Events::Event event) override;

  // Public API that can be used from the main controller

  // Check if one of the statistics series is selected in the Snapshot
  bool hasSelectedSeries() const;
  // Select the first series in the Snapshot
  void selectFirstSeriesAndBar() {
    setSelectedSeries(0);
    setSelectedBarIndex(0);
  }

  /* Highlight the row corresponding to a certain series */
  void highlightRow(std::size_t row) {
    assert(0 <= row && row <= m_selectableListView.totalNumberOfRows());

    // Set the cell to "selected" state in the SelectedListView
    m_selectableListView.selectCell(row);
    /* The SelectableListView took the firstResponder ownership when selecting
     * the cell. However we want the main controller to be the first
     * responder, because the banner view need to be updated as well. So the
     * firstResponder ownership is given back to the main controller, which
     * is the parent responder of HistogramListController. */
    Escher::App::app()->setFirstResponder(parentResponder());

    // Highlight the selected cell
    m_selectableListView.selectedCell()->setHighlighted(true);
  }

  void highlightHistogramBar(std::size_t row, std::size_t barIndex) {
    assert(0 <= row && row <= m_store->numberOfActiveSeries());
    assert(0 <= barIndex && barIndex < m_store->numberOfBars(row));
    static_cast<HistogramCell*>(m_selectableListView.cell(row))
        ->setBarHighlight(m_store->startOfBarAtIndex(row, barIndex),
                          m_store->endOfBarAtIndex(row, barIndex));
  }

  // Unhighlight the entire list
  void unhighlightList() { m_selectableListView.deselectTable(); }

  // TODO: private; rename to clampSelectedBarIndex
  /* If the number of histogram bars has been changed by the user and there are
   * less bars, the selected bar index can become out of range. We need to
   * sanitize it by setting this index to the last bar. */
  void sanitizeSelectedBarIndex() {
    if (!hasSelectedSeries()) {
      // Do nothing if no series is selected.
      return;
    }
    std::size_t numberOfBars = m_store->numberOfBars(selectedSeries());
    if (selectedBarIndex() >= numberOfBars) {
      setSelectedBarIndex(numberOfBars - 1);
    }
  }

  // Get the selected series or index from the Snapshot
  std::size_t selectedSeries() const;
  std::size_t selectedBarIndex() const;

 private:
  // Escher::TableViewDataSource
  // TODO: Escher::TableViewDataSource::nonMemoizedRowHeight should be const
  KDCoordinate nonMemoizedRowHeight(int row) override {
    return histogramHeight();
  }

  // Set the selected series or index in the Snapshot
  void setSelectedSeries(std::size_t selectedSeries);
  void setSelectedBarIndex(std::size_t selectedIndex);

  // Navigation inside and between the histogram cells
  bool moveSelectionHorizontally(OMG::HorizontalDirection direction);
  std::size_t sanitizeSelectedIndex(std::size_t selectedSeries,
                                    std::size_t initialSelectedIndex) const;
  std::size_t barIndexAfterSelectingNewSeries(
      std::size_t previousSelectedSeries, std::size_t currentSelectedSeries,
      std::size_t previousSelectedBarIndex) const;

  // Maximum number of histograms displayed on the same screen
  constexpr static std::size_t k_displayedHistograms = 4;
  // SelectableList cells
  std::array<HistogramCell, k_displayedHistograms> m_displayCells;

  // Model
  Store* m_store;
  HistogramRange* m_histogramRange;
};

}  // namespace Statistics

#endif

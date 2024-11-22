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
                          Shared::CurveViewRange* histogramRange);

  static constexpr KDCoordinate k_rowHeight = 75;

  // Escher::TableViewDataSource
  int numberOfRows() const override { return m_store->numberOfActiveSeries(); };

  // Escher::ListViewDataSource
  void fillCellForRow(Escher::HighlightCell* cell, int row) override;
  int typeAtRow(int row) const override { return 0; }
  Escher::HighlightCell* reusableCell(int index, int type) override;
  int reusableCellCount(int type) const override {
    return std::size(m_displayCells);
    ;
  }

  bool handleEvent(Ion::Events::Event event) override;

  // Helpers that can be used from the main controller
  void selectAndHighlightFirstCell();

  /* TODO: hasSelectedCell() should be const when
   * SelectableListView::selectedCell() provides a const version */
  bool hasSelectedCell() {
    return m_selectableListView.selectedCell() != nullptr;
  }
  /* The selected cell in the SelectableListView can be highlighted or not. */
  void setSelectedCellHighlight(bool isHighlighted);

  // Get the selected series or index from the Snapshot
  std::size_t selectedSeries() const;
  std::size_t selectedBarIndex() const;

 private:
  // Escher::TableViewDataSource
  // TODO: Escher::TableViewDataSource::nonMemoizedRowHeight should be const
  KDCoordinate nonMemoizedRowHeight(int row) override { return k_rowHeight; };

  /* Set the global highlight of a series on and highlight a certain histogram
   * bar  */
  void highlightSeriesAndBar(std::size_t selectedSeries,
                             std::size_t selectedBarIndex);

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
  HistogramRange m_histogramRange;
};

}  // namespace Statistics

#endif

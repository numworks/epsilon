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
  void selectFirstCell();

  /* TODO: hasSelectedCell() should be const when
   * SelectableListView::selectedCell() provides a const version */
  bool hasSelectedCell() {
    return m_selectableListView.selectedCell() != nullptr;
  }
  /* The selected cell in the SelectableListView can be highlighted or not. */
  void setSelectedCellHighlight(bool isHighlighted) {
    assert(hasSelectedCell());
    m_selectableListView.selectedCell()->setHighlighted(isHighlighted);
  }

  // Get the selected series or index from the Snapshot
  std::size_t selectedSeries() const;
  std::size_t selectedSeriesIndex() const;

 private:
  // Escher::TableViewDataSource
  // TODO: Escher::TableViewDataSource::nonMemoizedRowHeight should be const
  KDCoordinate nonMemoizedRowHeight(int row) override { return 75; };

  // Set the selected series or index in the Snapshot
  void setSelectedSeries(std::size_t selectedSeries);
  void setSelectedSeriesIndex(std::size_t selectedIndex);

  // Navigation inside and between the histogram cells
  bool moveSelectionHorizontally(OMG::HorizontalDirection direction);
  std::size_t sanitizeSelectedIndex(std::size_t selectedSeries,
                                    std::size_t initialSelectedIndex) const;

  // Maximum number of histograms displayed on the same screen
  constexpr static std::size_t k_displayedHistograms = 4;
  // SelectableList cells
  // TODO: replace with HistogramCells
  std::array<HistogramCell, k_displayedHistograms> m_displayCells;

  // Model
  Store* m_store;
  HistogramRange m_histogramRange;
};

}  // namespace Statistics

#endif

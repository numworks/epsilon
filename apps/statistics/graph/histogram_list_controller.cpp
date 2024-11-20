#include "histogram_list_controller.h"

#include "../app.h"

namespace Statistics {

Escher::SolidColorCell makeColorCell(size_t index) {
  return Escher::SolidColorCell();
}

HistogramListController::HistogramListController(
    Escher::Responder* parentResponder, Store* store, uint32_t* storeVersion)
    : Escher::SelectableListViewController<Escher::ListViewDataSource>(
          parentResponder, this),
      m_displayCells({makeColorCell(0), makeColorCell(1), makeColorCell(2)}),
      m_store(store),
      m_storeVersion(storeVersion),
      m_histogramRange(store) {}

Escher::SolidColorCell* HistogramListController::reusableCell(int index,
                                                              int type) {
  assert(type == 0);
  assert(index >= 0 && index < std::size(m_displayCells));
  return &m_displayCells[index];
}

void HistogramListController::fillCellForRow(Escher::HighlightCell* cell,
                                             int row) {
  assert(row >= 0 && row < numberOfRows());
  Escher::SolidColorCell* colorCell =
      static_cast<Escher::SolidColorCell*>(cell);
  colorCell->setColor(Store::colorOfSeriesAtIndex(row));
  colorCell->setHighlightColor(Store::colorLightOfSeriesAtIndex(row));
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

}  // namespace Statistics

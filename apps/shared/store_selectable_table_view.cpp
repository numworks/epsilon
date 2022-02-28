#include "store_selectable_table_view.h"

using namespace Escher;

namespace Shared {

StoreSelectableTableView::StoreSelectableTableView(DoublePairStore * store, Responder * parentResponder, TableViewDataSource * dataSource, SelectableTableViewDataSource * selectionDataSource, SelectableTableViewDelegate * delegate) :
  SelectableTableView(parentResponder, dataSource, selectionDataSource, delegate),
  m_store(store)
{
}

bool StoreSelectableTableView::handleEvent(Ion::Events::Event event) {
  int step = Ion::Events::longPressFactor();
  if (event == Ion::Events::Down) {
    return selectNonHiddenCellAtClippedLocation(selectedColumn(), selectedRow() + step);
  }
  if (event == Ion::Events::Up) {
    return selectNonHiddenCellAtClippedLocation(selectedColumn(), selectedRow() - step);
  }
  if (event == Ion::Events::Left) {
    return selectNonHiddenCellAtClippedLocation(selectedColumn() - step, selectedRow());
  }
  if (event == Ion::Events::Right) {
    return selectNonHiddenCellAtClippedLocation(selectedColumn() + step, selectedRow());
  }
  return false;
}

bool StoreSelectableTableView::selectNonHiddenCellAtClippedLocation(int i, int j) {
  // Clip i to retrieve a valid seriesIndex
  if (i < 0) {
    i = 0;
  } else if (i >= dataSource()->numberOfColumns()) {
    i = dataSource()->numberOfColumns() - 1;
  }
  int seriesIndex = i / DoublePairStore::k_numberOfColumnsPerSeries;
  int numberOfValuesOfCurrentColumn = m_store->numberOfValuesOfColumn(seriesIndex, i);
  if (j > 1 + numberOfValuesOfCurrentColumn) {
    j = 1 + numberOfValuesOfCurrentColumn;
  }
  // if negative, j will be clipped in selectCellAtClippedLocation
  return selectCellAtClippedLocation(i, j);
}

}

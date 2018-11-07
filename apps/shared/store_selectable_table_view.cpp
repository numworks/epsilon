#include "store_selectable_table_view.h"

namespace Shared {

StoreSelectableTableView::StoreSelectableTableView(DoublePairStore * store, Responder * parentResponder, TableViewDataSource * dataSource, SelectableTableViewDataSource * selectionDataSource, SelectableTableViewDelegate * delegate) :
  SelectableTableView(parentResponder, dataSource, selectionDataSource, delegate),
  m_store(store)
{
}

bool StoreSelectableTableView::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Down) {
    return selectNonHiddenCellAtLocation(selectedColumn(), selectedRow()+1);
  }
  if (event == Ion::Events::Up) {
    return selectNonHiddenCellAtLocation(selectedColumn(), selectedRow()-1);
  }
  if (event == Ion::Events::Left) {
    return selectNonHiddenCellAtLocation(selectedColumn()-1, selectedRow());
  }
  if (event == Ion::Events::Right) {
    return selectNonHiddenCellAtLocation(selectedColumn()+1, selectedRow());
  }
  return false;
}

bool StoreSelectableTableView::selectNonHiddenCellAtLocation(int i, int j) {
  if (i < 0 || i >= dataSource()->numberOfColumns()) {
    return false;
  }
  if (j < 0 || j >= dataSource()->numberOfRows()) {
    return false;
  }
  int seriesIndex = i/DoublePairStore::k_numberOfColumnsPerSeries;
  int numberOfPairsOfCurrentSeries = m_store->numberOfPairsOfSeries(seriesIndex);
  if (j > 1 + numberOfPairsOfCurrentSeries) {
    return selectCellAtLocation(i, 1 + numberOfPairsOfCurrentSeries);
  }
  return selectCellAtLocation(i, j);
}

}

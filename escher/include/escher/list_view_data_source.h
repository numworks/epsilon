#ifndef ESCHER_LIST_VIEW_DATA_SOURCE_H
#define ESCHER_LIST_VIEW_DATA_SOURCE_H

#include <escher/table_view_data_source.h>
#include <escher/table_view.h>
#include <escher/highlight_cell.h>

namespace Escher {

class ListViewDataSource : public TableViewDataSource {
public:
  void initCellSize(TableView * view) override;
  // ListViewDataSource has only one column
  int numberOfColumns() const override { return 1; }
  int indexFromCumulatedWidth(KDCoordinate offsetX) override { return 0; }
  KDCoordinate cumulatedWidthFromIndex(int index) override { return index == 1 ? cellWidth() : 0; }

  // Simplified APIs
  virtual KDCoordinate cellWidth() { return 0; }
  KDCoordinate columnWidth(int index) override { return cellWidth(); }

  virtual void willDisplayCellForIndex(HighlightCell * cell, int index) {}
  void willDisplayCellAtLocation(HighlightCell * cell, int x, int y) override { willDisplayCellForIndex(cell, y); }

  virtual int typeAtIndex(int index) { return 0; }
  int typeAtLocation(int i, int j) override { assert(i==0); return typeAtIndex(j); }
  /* cellHeight have a default implementation for specific simple
   * lists. Most implementations should override them.*/
  virtual KDCoordinate rowHeight(int index) override;
  // Used to easily override nonMemoizedRowHeight
  KDCoordinate heightForCellAtIndexWithWidthInit(HighlightCell * cell, int index);
protected:
  KDCoordinate heightForCellAtIndex(HighlightCell * cell, int index);
};

}

#endif

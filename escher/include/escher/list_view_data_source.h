#ifndef ESCHER_LIST_VIEW_DATA_SOURCE_H
#define ESCHER_LIST_VIEW_DATA_SOURCE_H

#include <escher/table_view_data_source.h>
#include <escher/highlight_cell.h>

namespace Escher {

class ListViewDataSource : public TableViewDataSource {
public:
  // < TEMP
  KDCoordinate rowHeight(int j) override;
  void prepareCellForHeightCalculation(HighlightCell * cell, int index);
  int reusableCellCount(int type) override { return numberOfRows(); }
  virtual int typeAtIndex(int index) { return 0; }
  // TEMP />


  // ListViewDataSource has only one column
  int numberOfColumns() const override { return 1; }
  int indexFromCumulatedWidth(KDCoordinate offsetX) override { return 0; }
  KDCoordinate cumulatedWidthFromIndex(int index) override { return index == 1 ? cellWidth() : 0; }

  // Simplified APIs
  virtual KDCoordinate cellWidth() { return 0; }
  KDCoordinate columnWidth(int index) override { return cellWidth(); }

  virtual void willDisplayCellForIndex(HighlightCell * cell, int index) {}
  void willDisplayCellAtLocation(HighlightCell * cell, int x, int y) override { willDisplayCellForIndex(cell, y); }

  // virtual int typeAtIndex(int index); TODO : remove temp and uncomment it
  int typeAtLocation(int i, int j) override { return typeAtIndex(j); }
};

}

#endif

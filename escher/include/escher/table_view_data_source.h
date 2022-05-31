#ifndef ESCHER_TABLE_VIEW_DATA_SOURCE_H
#define ESCHER_TABLE_VIEW_DATA_SOURCE_H

#include <escher/view.h>
#include <escher/highlight_cell.h>

namespace Escher {

class TableView;

class TableViewDataSource {
public:
  virtual void initCellSize(TableView * view) {}
  virtual int numberOfRows() const = 0;
  virtual int numberOfColumns() const = 0;
  virtual void willDisplayCellAtLocation(HighlightCell * cell, int i, int j);
  virtual KDCoordinate columnWidth(int i) = 0;
  virtual KDCoordinate rowHeight(int j) = 0;
  /* return the number of pixels to include in offset to display the column i at
  the top */
  virtual KDCoordinate cumulatedWidthFromIndex(int i);
  virtual KDCoordinate cumulatedHeightFromIndex(int j);
  /* return the number of columns (starting with first ones) that can be fully
   * displayed in offsetX pixels.
   * Caution: if the offset is exactly the size of n columns, the function
   * returns n-1. */
  virtual int indexFromCumulatedWidth(KDCoordinate offsetX);
  virtual int indexFromCumulatedHeight(KDCoordinate offsetY);
  virtual HighlightCell * reusableCell(int index, int type) = 0;
  virtual int reusableCellCount(int type) = 0;
  virtual int typeAtLocation(int i, int j) = 0;
};

}
#endif

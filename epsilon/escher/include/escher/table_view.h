#pragma once

#include <escher/highlight_cell.h>
#include <escher/scroll_view.h>
#include <escher/table_grid_view.h>
#include <escher/table_view_data_source.h>

namespace Escher {

class TableView : public ScrollView {
 public:
  enum class GridStyle : bool { None, Allowed };

  TableView(TableViewDataSource* dataSource,
            ScrollViewDataSource* scrollDataSource,
            Escher::ScrollViewDelegate* scrollViewDelegate = nullptr,
            GridStyle gridStyle = GridStyle::None);

  void setHorizontalCellOverlap(KDCoordinate o) {
    m_contentView.setHorizontalCellOverlap(o);
  }
  void setVerticalCellOverlap(KDCoordinate o) {
    m_contentView.setVerticalCellOverlap(o);
  }

  int firstDisplayedRow() const { return m_contentView.rowsScrollingOffset(); }
  int firstDisplayedColumn() const {
    return m_contentView.columnsScrollingOffset();
  }
  int numberOfDisplayableRows() const {
    return m_contentView.numberOfDisplayableRows();
  }
  int numberOfDisplayableColumns() const {
    return m_contentView.numberOfDisplayableColumns();
  }
  int totalNumberOfRows() const { return dataSource()->numberOfRows(); }
  int totalNumberOfColumns() const { return dataSource()->numberOfColumns(); }
  const TableViewDataSource* dataSource() const {
    return m_contentView.dataSource();
  }

  const HighlightCell* cellAtRelativeLocation(int relativeColumn,
                                              int relativeRow) const {
    return m_contentView.cellAtRelativeLocation(relativeColumn, relativeRow);
  }
  HighlightCell* cellAtRelativeLocation(int relativeColumn, int relativeRow) {
    return m_contentView.cellAtRelativeLocation(relativeColumn, relativeRow);
  }

  /* This method computes the minimal scrolling needed to properly display the
   * requested cell. */
  void scrollToCell(int col, int row) {
    assert(row >= 0);
    scrollToContentRect(m_contentView.cellFrame(col, row));
  }
  void scrollToBottom();
  const HighlightCell* cellAtLocation(int col, int row) const {
    return m_contentView.cellAtLocation(col, row);
  }
  HighlightCell* cellAtLocation(int col, int row) {
    return m_contentView.cellAtLocation(col, row);
  }
  void reloadCellAtLocation(int col, int row, bool forceSetFrame = false) {
    m_contentView.reloadCellAtLocation(col, row, forceSetFrame);
  }
  void reloadVisibleCellsAtColumn(int column);
  void hideScrollBars() { m_decorator.setVisibility(false); }
  BarDecorator* decorator() override { return &m_decorator; }

  void resetSizeAndOffsetMemoization();
  void resetMemoizedColumnAndRowOffsets() {
    m_contentView.resetMemoizedColumnAndRowOffsets();
  }

  KDCoordinate invisibleHeight() {
    return std::max(contentOffset().y() - margins()->top(), 0);
  }
  KDCoordinate invisibleWidth() {
    return std::max(contentOffset().x() - margins()->left(), 0);
  }

 protected:
#if ESCHER_VIEW_LOGGING
  const char* className() const override { return "TableView"; }
#endif
  TableViewDataSource* dataSource() { return m_contentView.dataSource(); }
  int numberOfDisplayableCells() {
    return m_contentView.numberOfDisplayableCells();
  }
  HighlightCell* reusableCellAtIndex(int index) {
    return m_contentView.reusableCellAtIndex(index);
  }
  void layoutSubviews(bool force = false) override;
  // Ensure that cells are properly filled after scrolling
  bool alwaysForceRelayoutOfContentView() const override { return true; }

  class ContentView : public View {
   public:
    ContentView(TableView* tableView, TableViewDataSource* dataSource,
                KDCoordinate horizontalCellOverlap,
                KDCoordinate verticalCellOverlap,
                TableView::GridStyle gridStyle);
    KDSize minimalSizeForOptimalDisplay() const override {
      return KDSize(width(), height());
    }
    void drawRect(KDContext* ctx, KDRect rect) const override;

    void setHorizontalCellOverlap(KDCoordinate o) {
      m_horizontalCellOverlap = o;
    }
    void setVerticalCellOverlap(KDCoordinate o) { m_verticalCellOverlap = o; }

    void reloadCellAtLocation(int col, int row, bool forceSetFrame);

    HighlightCell* cellAtLocation(int row, int col);
    const HighlightCell* cellAtLocation(int row, int col) const;

    TableViewDataSource* dataSource() { return m_dataSource; }
    const TableViewDataSource* dataSource() const { return m_dataSource; }
    KDCoordinate invisibleHeight() const {
      return m_tableView->invisibleHeight();
    }
    KDCoordinate invisibleWidth() const {
      return m_tableView->invisibleWidth();
    }
    int rowsScrollingOffset() const;
    int columnsScrollingOffset() const;
    void resetMemoizedColumnAndRowOffsets() {
      m_rowsScrollingOffset = -1;
      m_columnsScrollingOffset = -1;
    }
    int numberOfDisplayableRows() const;
    int numberOfDisplayableColumns() const;
    KDRect cellFrame(int col, int row) const;
    void layoutSubviews(bool force) override;
    int numberOfDisplayableCells() const {
      return numberOfDisplayableRows() * numberOfDisplayableColumns();
    }
    HighlightCell* reusableCellAtIndex(int index);

    HighlightCell* cellAtRelativeLocation(int relativeColumn, int relativeRow) {
      return cellAtLocation(relativeColumn + columnsScrollingOffset(),
                            relativeRow + rowsScrollingOffset());
    }
    const HighlightCell* cellAtRelativeLocation(int relativeColumn,
                                                int relativeRow) const {
      return cellAtLocation(relativeColumn + columnsScrollingOffset(),
                            relativeRow + rowsScrollingOffset());
    }

   protected:
#if ESCHER_VIEW_LOGGING
    const char* className() const override { return "TableView::ContentView"; }
#endif
   private:
    KDCoordinate height() const {
      return m_dataSource->cumulatedHeightBeforeRow(
                 m_dataSource->numberOfRows()) +
             m_verticalCellOverlap;
    }
    KDCoordinate width() const;

    // The grid view is the last subview, after the cells
    int gridViewIndex() const { return numberOfDisplayableCells(); }

    bool hasGrid() const;
    int numberOfSubviews() const override {
      return numberOfDisplayableCells() + static_cast<int>(hasGrid());
    }
    View* subviewAtIndex(int index) override;

    /* These two methods transform a positive index (of subview for instance)
     * into coordinates that refer to the data source entire table */
    int absoluteColumnFromSubviewIndex(int index) const {
      assert(index != gridViewIndex());
      int nb = numberOfDisplayableColumns();
      assert(nb > 0);
      return (index % nb) + columnsScrollingOffset();
    }
    int absoluteRowFromSubviewIndex(int index) const {
      assert(index != gridViewIndex());
      int nb = numberOfDisplayableColumns();
      assert(nb > 0);
      return (index / nb) + rowsScrollingOffset();
    }
    int typeOfSubviewAtIndex(int index) const;
    /* This method transforms an index (of subview for instance) into an index
     * referring to the set of cells of type "type". */
    int typeIndexFromSubviewIndex(int index, int type) const;

    enum SeparatorType : bool { Column, Row };
    /* Separator rectangles are drawn to fill a gap between two rows or two
     * columns, when these two rows or two columns are apart from each other. */
    void drawSeparators(KDContext* ctx, SeparatorType type) const;

    TableView* m_tableView;
    TableViewDataSource* m_dataSource;
    /* The table grid needs to be the last subview of TableView, so that it its
     * drawn on top of the cells */
    TableGridView m_grid;
    bool m_canHaveGrid;
    KDCoordinate m_horizontalCellOverlap;
    KDCoordinate m_verticalCellOverlap;
    mutable int m_rowsScrollingOffset;
    mutable int m_columnsScrollingOffset;
  };
  BarDecorator m_decorator;
  ContentView m_contentView;
};

}  // namespace Escher

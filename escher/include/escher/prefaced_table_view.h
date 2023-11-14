#ifndef ESCHER_PREFACED_TABLE_VIEW_H
#define ESCHER_PREFACED_TABLE_VIEW_H

#include <escher/container.h>
#include <escher/scroll_view.h>
#include <escher/selectable_table_view.h>

/* This class is used to add a "preface" to a selectable table : one of its
 * row will appear on top of the screen if it should go out of frame.
 * It uses a secondary table view, which it syncs up to the selectable table
 * by intercepting callbacks to its delegate. */

namespace Escher {

class PrefacedTableViewDelegate {
 public:
  virtual KDCoordinate maxRowPrefaceHeight() const = 0;
  virtual int columnToFreeze() = 0;
};

class PrefacedTableView : public View,
                          public Responder,
                          public SelectableTableViewDelegate {
 public:
  PrefacedTableView(
      int prefaceRow, Responder* parentResponder,
      SelectableTableView* mainTableView, TableViewDataSource* cellsDataSource,
      SelectableTableViewDelegate* delegate = nullptr,
      PrefacedTableViewDelegate* prefacedTableViewDelegate = nullptr);

  // Responder
  void didBecomeFirstResponder() override {
    App::app()->setFirstResponder(m_mainTableView);
  }

  // SelectableTableViewDelegate
  void tableViewDidChangeSelectionAndDidScroll(
      SelectableTableView* t, int previousSelectedCol, int previousSelectedRow,
      KDPoint previousOffset, bool withinTemporarySelection = false) override;
  KDPoint offsetToRestoreAfterReload(
      const SelectableTableView* t) const override {
    return m_virtualOffset;
  }

  SelectableTableView* selectableTableView() { return m_mainTableView; }
  virtual void setMargins(KDMargins m);
  virtual void setBackgroundColor(KDColor color);
  virtual void setCellOverlap(KDCoordinate horizontal, KDCoordinate vertical);

  void setPrefaceDelegate(PrefacedTableViewDelegate* delegate) {
    m_prefacedDelegate = delegate;
  }

  virtual void resetSizeAndOffsetMemoization();

 protected:
  class IntermediaryDataSource : public TableViewDataSource,
                                 public ScrollViewDataSource {
   public:
    IntermediaryDataSource(TableViewDataSource* mainDataSource)
        : m_mainDataSource(mainDataSource) {}

    // TableViewDataSource
    int numberOfRows() const override {
      return m_mainDataSource->numberOfRows();
    }
    int numberOfColumns() const override {
      return m_mainDataSource->numberOfColumns();
    }
    void fillCellForLocation(HighlightCell* cell, int column,
                             int row) override {
      m_mainDataSource->fillCellForLocation(
          cell, columnInMainDataSource(column), rowInMainDataSource(row));
    }
    HighlightCell* reusableCell(int index, int type) override;
    int reusableCellCount(int type) override {
      return m_mainDataSource->reusableCellCount(type);
    }
    int typeAtLocation(int column, int row) override {
      return m_mainDataSource->typeAtLocation(columnInMainDataSource(column),
                                              rowInMainDataSource(row));
    }
    KDCoordinate separatorBeforeColumn(int column) override {
      return m_mainDataSource->separatorBeforeColumn(
          columnInMainDataSource(column));
    }
    KDCoordinate separatorBeforeRow(int row) override {
      return m_mainDataSource->separatorBeforeRow(rowInMainDataSource(row));
    }

   protected:
    KDCoordinate nonMemoizedColumnWidth(int column) override final;
    KDCoordinate nonMemoizedRowHeight(int row) override final;

    KDCoordinate nonMemoizedCumulatedWidthBeforeColumn(int column) override;
    KDCoordinate nonMemoizedCumulatedHeightBeforeRow(int row) override;

    /* WARNING: This method works only if columnInMainDataSource(i) == i.
     * Else, it should be overriden.*/
    int nonMemoizedColumnAfterCumulatedWidth(KDCoordinate offsetX) override;
    /* WARNING: This method works only if rowInMainDataSource(j) == j.
     * Else, it should be overriden.*/
    int nonMemoizedRowAfterCumulatedHeight(KDCoordinate offsetY) override;

    virtual int columnInMainDataSource(int i) { return i; }
    virtual int rowInMainDataSource(int j) { return j; }

    TableViewDataSource* m_mainDataSource;
  };

  class RowPrefaceDataSource : public IntermediaryDataSource {
   public:
    RowPrefaceDataSource(int prefaceRow, TableViewDataSource* mainDataSource)
        : IntermediaryDataSource(mainDataSource),
          m_prefaceRow(prefaceRow),
          m_rowHeigthManager(this) {}

    KDCoordinate cumulatedHeightAtPrefaceRow(bool after) const;
    int numberOfRows() const override { return 1; }
    KDCoordinate separatorBeforeRow(int row) override { return 0; }
    KDCoordinate separatorAfterPrefaceRow() {
      assert(m_prefaceRow >= 0);
      return m_mainDataSource->separatorBeforeRow(m_prefaceRow + 1);
    }

   private:
    KDCoordinate nonMemoizedCumulatedHeightBeforeRow(int row) override;
    int nonMemoizedRowAfterCumulatedHeight(KDCoordinate offsetY) override;

    int rowInMainDataSource(int j) override {
      assert(j == 0 || j == 1);
      assert(m_prefaceRow >= 0);
      return m_prefaceRow + j;
    }

    TableSize1DManager* rowHeightManager() override {
      return &m_rowHeigthManager;
    }

    const int m_prefaceRow;
    MemoizedRowHeightManager<1> m_rowHeigthManager;
  };

  virtual void layoutSubviewsInRect(KDRect rect, bool force);
  void layoutScrollbars(bool force);
  void updateVirtualOffset();
  virtual void resetContentOffset();

  RowPrefaceDataSource m_rowPrefaceDataSource;
  TableView m_rowPrefaceView;
  ScrollView::BarDecorator m_barDecorator;
  SelectableTableView* m_mainTableView;
  PrefacedTableViewDelegate* m_prefacedDelegate;
  KDCoordinate m_mainTableViewTopMargin;

 private:
  // View
  int numberOfSubviews() const override { return 4; }
  void layoutSubviews(bool force = false) override;
  View* subviewAtIndex(int index) override;
  virtual KDPoint marginToAddForVirtualOffset() const {
    return KDPoint(
        0, m_mainTableViewTopMargin - m_mainTableView->margins()->top());
  }
  KDCoordinate verticalScrollToAddToHidePrefacesInMainTable(
      bool hideRowPreface) const;
  virtual KDCoordinate horizontalScrollToAddToHidePrefacesInMainTable(
      bool hideColumnPreface) const {
    return 0;
  }

  SelectableTableViewDelegate* m_mainTableDelegate;
  KDPoint m_virtualOffset;
};

}  // namespace Escher

#endif

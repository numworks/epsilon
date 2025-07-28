#ifndef ESCHER_PREFACED_TABLE_VIEW_H
#define ESCHER_PREFACED_TABLE_VIEW_H

#include <escher/preface_data_source.h>

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

  // SelectableTableViewDelegate
  void tableViewDidChangeSelectionAndDidScroll(
      SelectableTableView* t, int previousSelectedCol, int previousSelectedRow,
      KDPoint previousOffset, bool withinTemporarySelection = false) override;
  KDPoint offsetToRestoreAfterReload(
      const SelectableTableView* t) const override {
    return m_virtualOffset;
  }
  int numberOfRowsAtColumn(const SelectableTableView* t, int column) override;

  SelectableTableView* selectableTableView() { return m_mainTableView; }
  virtual void setMargins(KDMargins m);
  virtual void setBackgroundColor(KDColor color);
  virtual void setCellOverlap(KDCoordinate horizontal, KDCoordinate vertical);

  void setPrefaceDelegate(PrefacedTableViewDelegate* delegate) {
    m_prefacedDelegate = delegate;
  }

  virtual void resetSizeAndOffsetMemoization();

 protected:
  virtual void layoutSubviewsInRect(KDRect rect, bool force);
  virtual void resetContentOffset();

  RowPrefaceDataSource m_rowPrefaceDataSource;
  TableView m_rowPrefaceView;
  ScrollView::BarDecorator m_barDecorator;
  SelectableTableView* m_mainTableView;
  PrefacedTableViewDelegate* m_prefacedDelegate;
  KDCoordinate m_mainTableViewTopMargin;

 protected:
  // Responder
  void handleResponderChainEvent(Responder::ResponderChainEvent event) override;

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
  void updateVirtualOffset();
  void layoutScrollbars(bool force);

  SelectableTableViewDelegate* m_mainTableDelegate;
  KDPoint m_virtualOffset;
};

}  // namespace Escher

#endif

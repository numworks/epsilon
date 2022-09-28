#ifndef SHARED_PREFACED_TABLE_VIEW_H
#define SHARED_PREFACED_TABLE_VIEW_H

#include <escher/container.h>
#include <escher/selectable_table_view.h>

/* This class is used to add a "preface" to a selectable table : one of its
 * row will appear on the left of the screen if it should go out of frame.
 * It uses a secondary table view, which it syncs up to the selectable table
 * by intercepting callbacks to its delegate. */

namespace Shared {

class PrefacedTableView : public Escher::View, public Escher::Responder, public Escher::SelectableTableViewDelegate {
public:
  PrefacedTableView(int prefaceRow, Escher::Responder * parentResponder, Escher::SelectableTableView * mainTableView, Escher::TableViewDataSource * cellsDataSource, Escher::SelectableTableViewDelegate * delegate = nullptr);

  // Responder
  void didBecomeFirstResponder() override { Escher::Container::activeApp()->setFirstResponder(m_mainTableView); }

  // SelectableTableViewDelegate
  void tableViewDidChangeSelection(Escher::SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY, bool withinTemporarySelection = false) override;
  void tableViewDidChangeSelectionAndDidScroll(Escher::SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY, bool withinTemporarySelection = false) override;

  Escher::SelectableTableView * selectableTableView() { return m_mainTableView; }
  virtual void setMargins(KDCoordinate top, KDCoordinate right, KDCoordinate bottom, KDCoordinate left);
  virtual void setBackgroundColor(KDColor color);
  virtual void setCellOverlap(KDCoordinate horizontal, KDCoordinate vertical);

  class MarginDelegate {
  public:
    virtual KDCoordinate prefaceMargin(Escher::TableView * preface, Escher::TableViewDataSource * prefaceDataSource) = 0;
  };

  void setMarginDelegate(MarginDelegate * delegate) { m_marginDelegate = delegate; }

protected:
  class IntermediaryDataSource : public Escher::TableViewDataSource, public Escher::ScrollViewDataSource {
  public:
    IntermediaryDataSource(Escher::TableViewDataSource * mainDataSource) : m_mainDataSource(mainDataSource) {}

    // TableViewDataSource
    int numberOfRows() const override { return m_mainDataSource->numberOfRows(); }
    int numberOfColumns() const override { return m_mainDataSource->numberOfColumns(); }
    void willDisplayCellAtLocation(Escher::HighlightCell * cell, int i, int j) override { m_mainDataSource->willDisplayCellAtLocation(cell, relativeColumn(i), relativeRow(j)); }
    Escher::HighlightCell * reusableCell(int index, int type) override { return m_mainDataSource->reusableCell(index, type); }
    int reusableCellCount(int type) override { return m_mainDataSource->reusableCellCount(type); }
    int typeAtLocation(int i, int j) override { return m_mainDataSource->typeAtLocation(relativeColumn(i), relativeRow(j)); }

  protected:
    KDCoordinate nonMemoizedColumnWidth(int i) override;
    KDCoordinate nonMemoizedRowHeight(int j) override;

    KDCoordinate nonMemoizedCumulatedWidthFromIndex(int i) override;
    KDCoordinate nonMemoizedCumulatedHeightFromIndex(int j) override;

    /* WARNING: This method works only if relativeColumn(i) == i.
     * Else, it should be overriden.*/
    int nonMemoizedIndexFromCumulatedWidth(KDCoordinate offsetX) override;
    /* WARNING: This method works only if relativeRow(j) == j.
     * Else, it should be overriden.*/
    int nonMemoizedIndexFromCumulatedHeight(KDCoordinate offsetY) override;

    virtual int relativeColumn(int i) { return i; }
    virtual int relativeRow(int j) { return j; }

    Escher::TableViewDataSource * m_mainDataSource;
  };

  class PrefaceDataSource : public IntermediaryDataSource {
  public:
    PrefaceDataSource(int prefaceRow, Escher::TableViewDataSource * mainDataSource) : IntermediaryDataSource(mainDataSource), m_prefaceRow(prefaceRow) {}

    int prefaceRow() const { return m_prefaceRow; }
    bool prefaceFullyInFrame(int offset);
    int numberOfRows() const override { return 1; }

  private:
    KDCoordinate nonMemoizedCumulatedHeightFromIndex(int j) override;
    int nonMemoizedIndexFromCumulatedHeight(KDCoordinate offsetY) override;

    int relativeRow(int j) override { assert(j == 0 || j == 1); return m_prefaceRow + j; }

    const int m_prefaceRow;
  };

  void layoutSubviewsInRect(KDRect rect, bool force);

  PrefaceDataSource m_prefaceDataSource;
  Escher::TableView m_prefaceView;
  Escher::SelectableTableView * m_mainTableView;
  MarginDelegate * m_marginDelegate;

private:
  // View
  int numberOfSubviews() const override { return 2; }
  Escher::View * subviewAtIndex(int index) override { return index == 0 ? m_mainTableView : &m_prefaceView; }
  void layoutSubviews(bool force = false) override;

  Escher::SelectableTableViewDelegate * m_mainTableDelegate;
  KDCoordinate m_storedMargin;
};

}

#endif

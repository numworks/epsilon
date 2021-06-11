#ifndef SHARED_PREFACED_TABLE_VIEW_H
#define SHARED_PREFACED_TABLE_VIEW_H

#include <escher/container.h>
#include <escher/selectable_table_view.h>

/* This class is used to add a "preface" to a selectable table : one of its
 * column will appear on the left of the screen if it should go out of frame.
 * It uses a secondary table view, which it syncs up to the selectable table
 * by intercepting callbacks to its delegate. */

namespace Shared {

class PrefacedTableView : public Escher::View, public Escher::Responder, public Escher::SelectableTableViewDelegate {
public:
  PrefacedTableView(int prefaceColumn, Escher::Responder * parentResponder, Escher::SelectableTableView * mainTableView, Escher::TableViewDataSource * cellsDataSource, Escher::SelectableTableViewDelegate * delegate = nullptr);

  // Responder
  void didBecomeFirstResponder() override { Escher::Container::activeApp()->setFirstResponder(m_mainTableView); }

  // SelectableTableViewDelegate
  void tableViewDidChangeSelection(Escher::SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY, bool withinTemporarySelection = false) override;
  void tableViewDidChangeSelectionAndDidScroll(Escher::SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY, bool withinTemporarySelection = false) override;

  Escher::SelectableTableView * selectableTableView() { return m_mainTableView; }
  void setMargins(KDCoordinate top, KDCoordinate right, KDCoordinate bottom, KDCoordinate left);
  void setBackgroundColor(KDColor color);
  void setCellOverlap(KDCoordinate horizontal, KDCoordinate vertical);

  class MarginDelegate {
  public:
    virtual KDCoordinate prefaceMargin(Escher::TableView * preface) = 0;
  };

  void setMarginDelegate(MarginDelegate * delegate) { m_marginDelegate = delegate; }

private:
  class PrefaceDataSource : public Escher::TableViewDataSource, public Escher::ScrollViewDataSource {
  public:
    PrefaceDataSource(int prefaceColumn, Escher::TableViewDataSource * mainDataSource) : m_mainDataSource(mainDataSource), m_prefaceColumn(prefaceColumn) {}

    bool prefaceFullyInFrame(int offset) const { return offset <= m_mainDataSource->cumulatedWidthFromIndex(m_prefaceColumn); }

    // TableViewDataSource
    int numberOfRows() const override { return m_mainDataSource->numberOfRows(); }
    int numberOfColumns() const override { return 1; }
    void willDisplayCellAtLocation(Escher::HighlightCell * cell, int i, int j) override { assert(i == 0); return m_mainDataSource->willDisplayCellAtLocation(cell, m_prefaceColumn, j); }
    KDCoordinate columnWidth(int i) override { assert(i == 0); return m_mainDataSource->columnWidth(m_prefaceColumn); }
    KDCoordinate rowHeight(int j) override { return m_mainDataSource->rowHeight(j); }
    Escher::HighlightCell * reusableCell(int index, int type) override { return m_mainDataSource->reusableCell(index, type); }
    int reusableCellCount(int type) override { return m_mainDataSource->reusableCellCount(type); }
    int typeAtLocation(int i, int j) override { assert(i == 0); return m_mainDataSource->typeAtLocation(m_prefaceColumn, j); }

  private:
    Escher::TableViewDataSource * m_mainDataSource;
    const int m_prefaceColumn;
  };

  // View
  int numberOfSubviews() const override { return 2; }
  Escher::View * subviewAtIndex(int index) override { return index == 0 ? m_mainTableView : &m_prefaceView; }
  void layoutSubviews(bool force = false) override;

  PrefaceDataSource m_prefaceDataSource;
  Escher::TableView m_prefaceView;
  Escher::SelectableTableView * m_mainTableView;
  Escher::SelectableTableViewDelegate * m_mainTableDelegate;
  MarginDelegate * m_marginDelegate;
  KDCoordinate m_storedMargin;
};

}

#endif

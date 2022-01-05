#include "title_table_view.h"
#include <escher/palette.h>
#include <escher/metric.h>

using namespace Solver;
using namespace Escher;

TitleTableView::ContentView::ContentView(Escher::SelectableTableView * table,
                                                   I18n::Message titleMessage) :
      m_title(KDFont::SmallFont,
              titleMessage,
              KDContext::k_alignCenter,
              KDContext::k_alignCenter,
              Palette::GrayDark,
              Palette::WallScreen),
      m_table(table) {
  m_table->setMargins(k_marginAroundTitle, 0, 0, 0);
}

Escher::View * TitleTableView::ContentView::subviewAtIndex(int i) {
  assert(i < numberOfSubviews());
  if (i == 0) {
    return &m_title;
  } else {
    return m_table;
  }
}

void TitleTableView::layoutSubviews(bool force) {
  /* Content view must be given a width, so that it can give its
   * SelectableTableView a width and compute it height. */
  if (m_contentView.bounds().isEmpty()) {
    m_contentView.setFrame(KDRect(KDPointZero, KDSize(visibleContentRect().width(), 0)), force);
  } else {
    assert(m_contentView.bounds().width() == visibleContentRect().width());
  }
  ScrollView::layoutSubviews(true);
}


TitleTableView::TitleTableView(Escher::SelectableTableView * table,
                                      Escher::TableViewDataSource * tableDataSource,
                                      I18n::Message titleMessage) :
      ScrollView(&m_contentView, &m_scrollDataSource),
      m_contentView(table, titleMessage),
      m_tableDataSource(tableDataSource) {
  setMargins(ContentView::k_marginAroundTitle, Escher::Metric::CommonMargin, Metric::CommonBottomMargin, Escher::Metric::CommonMargin);
}

KDPoint TitleTableView::tableOrigin() {
  return KDPoint(0, ContentView::k_marginAroundTitle + KDFont::SmallFont->glyphSize().height());
}

void TitleTableView::tableViewDidChangeSelectionAndDidScroll(
    SelectableTableView * t,
    int previousSelectedCellX,
    int previousSelectedCellY,
    bool withinTemporarySelection) {
  // Scroll to correct location
  int row = m_contentView.m_table->selectedRow(), col = m_contentView.m_table->selectedColumn();
  if (row >= 0 && col >= 0) {
    KDRect cellFrame = KDRect(m_tableDataSource->cumulatedWidthFromIndex(col),
                              m_tableDataSource->cumulatedHeightFromIndex(row),
                              m_tableDataSource->columnWidth(col),
                              m_tableDataSource->rowHeight(row));
    cellFrame = cellFrame.translatedBy(tableOrigin());
    /* Include the title in the first row cells to force scrolling enough to
     * display it */
    if (row == 0) {
      cellFrame = cellFrame.unionedWith(KDRect(cellFrame.x(), 0, cellFrame.width(), cellFrame.height()));
    }
    scrollToContentRect(cellFrame);
  }
}

void TitleTableView::reload() {
  m_contentView.m_table->reloadData();
  layoutSubviews();
}
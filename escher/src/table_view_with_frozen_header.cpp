#include <escher/table_view_with_frozen_header.h>

namespace Escher {

TableViewWithFrozenHeader::TableViewWithFrozenHeader(
    SelectableTableView* selectableTableView, I18n::Message header)
    : m_headerView(KDFont::Size::Small, header, KDContext::k_alignCenter,
                   KDContext::k_alignCenter, Palette::GrayDark,
                   Palette::WallScreen),
      m_selectableTableView(selectableTableView) {
  // Remove selectable table top margin to control margin between text and table
  m_selectableTableView->setTopMargin(0);
  // Fit m_selectableTableView scroll to content size
  m_selectableTableView->decorator()->setVerticalMargins(
      0, Metric::CommonBottomMargin);
}

void TableViewWithFrozenHeader::drawRect(KDContext* ctx, KDRect rect) const {
  int tableHeight =
      m_selectableTableView->minimalSizeForOptimalDisplay().height();
  ctx->fillRect(
      KDRect(0, tableHeight, bounds().width(), bounds().height() - tableHeight),
      Palette::WallScreen);
}

View* TableViewWithFrozenHeader::subviewAtIndex(int index) {
  if (index == 0) {
    return &m_headerView;
  }
  return m_selectableTableView;
}

void TableViewWithFrozenHeader::layoutSubviews(bool force) {
  KDCoordinate titleHeight =
      KDFont::GlyphHeight(KDFont::Size::Small) + k_titleMargin;
  setChildFrame(&m_headerView, KDRect(0, 0, bounds().width(), titleHeight),
                force);
  /* SelectableTableView must be given a width before computing height. */
  m_selectableTableView->initSize(bounds());
  KDCoordinate tableHeight =
      m_selectableTableView->minimalSizeForOptimalDisplay().height();
  setChildFrame(m_selectableTableView,
                KDRect(0, titleHeight, bounds().width(), tableHeight)
                    .intersectedWith(bounds()),
                force);
}

}  // namespace Escher

#include "selectable_table_view_with_background.h"

SelectableTableViewWithBackground::SelectableTableViewWithBackground(Responder * parentResponder, TableViewDataSource * dataSource, BackgroundView * backgroundView, SelectableTableViewDataSource * selectionDataSource, SelectableTableViewDelegate * delegate) :
  SelectableTableView(parentResponder, dataSource, selectionDataSource, delegate),
  m_backgroundInnerView(this, backgroundView)
{

}

void SelectableTableViewWithBackground::BackgroundInnerView::drawRect(KDContext * ctx, KDRect rect) const {
    m_backgroundView->drawRect(ctx, rect);
}

void SelectableTableViewWithBackground::BackgroundInnerView::setBackgroundView(const uint8_t * data) {
    m_backgroundView->setBackgroundImage(data);
}
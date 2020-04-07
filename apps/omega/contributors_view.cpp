#include "contributors_view.h"
#include "apps/i18n.h"

namespace Omega {

ContributorsView::ContributorsView(SelectableTableView * selectableTableView) :
  View(),
  m_selectableTableView(selectableTableView)
{
}

void ContributorsView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(KDRect(0, 0, bounds().width(), bounds().height()), Palette::BackgroundApps);
}

void ContributorsView::reload() {
  markRectAsDirty(bounds());
}

int ContributorsView::numberOfSubviews() const {
  return 1;
}

View * ContributorsView::subviewAtIndex(int index) {
  assert(index == 0 || index == 1);
  return m_selectableTableView;
}

void ContributorsView::layoutSubviews(bool force) {
  m_selectableTableView->setFrame(KDRect(0, 0, bounds().width(), bounds().height()), force);
}

}

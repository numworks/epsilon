#include "title_bar_view.h"
extern "C" {
#include <assert.h>
}

TitleBarView::TitleBarView() :
  View(),
  m_titleView(KDText::FontSize::Small, nullptr, 0.5f, 0.5f, KDColorWhite, KDColorRed)
{
}

void TitleBarView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(bounds(), KDColorRed);
}

void TitleBarView::setTitle(const char * title) {
  m_titleView.setText(title);
}

int TitleBarView::numberOfSubviews() const {
  return 1;
}

View * TitleBarView::subviewAtIndex(int index) {
  return &m_titleView;
}

void TitleBarView::layoutSubviews() {
  m_titleView.setFrame(bounds());
}

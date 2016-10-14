#include "title_cell.h"
#include "../function.h"
#include <assert.h>

namespace Graph {

TitleCell::TitleCell() :
  EvenOddCell(),
  m_pointerTextView(nullptr, 0.5f, 0.5f)
{
}

void TitleCell::reloadCell() {
  EvenOddCell::reloadCell();
  m_pointerTextView.setBackgroundColor(backgroundColor());
}

void TitleCell::setText(const char * title) {
  m_pointerTextView.setText(title);
}

int TitleCell::numberOfSubviews() const {
  return 1;
}

View * TitleCell::subviewAtIndex(int index) {
  assert(index == 0);
  return &m_pointerTextView;
}

void TitleCell::layoutSubviews() {
  m_pointerTextView.setFrame(bounds());
}

void TitleCell::drawRect(KDContext * ctx, KDRect rect) const {
  EvenOddCell::drawRect(ctx, rect);
  // Write the "(x)"
  KDColor background = backgroundColor();
  KDSize textSize = KDText::stringSize("f");
  KDPoint origin(0.5f*(m_frame.width() - textSize.width()), 0.5f*(m_frame.height() - textSize.height()));
  ctx->drawString(Function::Parameter, origin.translatedBy(KDPoint(textSize.width(), 0)), KDColorBlack, background);
}

}

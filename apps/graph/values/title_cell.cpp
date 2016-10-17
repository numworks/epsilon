#include "title_cell.h"
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

void TitleCell::setText(const char * title, KDColor textColor) {
  m_pointerTextView.setText(title);
  m_pointerTextView.setTextColor(textColor);
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

}

#include <escher/scroll_view.h>
extern "C" {
#include <assert.h>
}

ScrollView::ScrollView(View * contentView) :
  View(),
  m_offset(KDPointZero),
  m_contentView(contentView)
{
  setSubview(m_contentView, 0);
  //setSubview(&m_verticalScrollIndicator, 1);
}

int ScrollView::numberOfSubviews() const {
  return 1;
}

const View * ScrollView::subview(int index) const {
  assert(index == 0);
  return m_contentView;
}

void ScrollView::storeSubviewAtIndex(View * view, int index) {
  assert(index == 0);
  m_contentView = view;
}

void ScrollView::layoutSubviews() {
  // Layout indicators
  // TODO
  // Layout contentview
  // FIXME: Use KDCoordinateMax
  m_contentView->setFrame({m_offset.x, m_offset.y, (KDCoordinate)9999, (KDCoordinate)9999});
}

#if ESCHER_VIEW_LOGGING
const char * ScrollView::className() const {
  return "ScrollView";
}

void ScrollView::logAttributes(std::ostream &os) const {
  View::logAttributes(os);
  os << " offset=\"" << (int)m_offset.x << "," << (int)m_offset.y << "\"";
}
#endif

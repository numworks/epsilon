#include <escher/stack_view.h>
#include <escher/palette.h>
extern "C" {
#include <assert.h>
}

StackView::StackView() :
  View(),
  m_textView(PointerTextView(nullptr, 0.5f, 0.5f, Palette::DesactiveTextColor))
{
}

int StackView::numberOfSubviews() const {
  return 1;
}

View * StackView::subviewAtIndex(int index) {
  assert(index == 0);
  return &m_textView;
}

void StackView::layoutSubviews() {
  m_textView.setFrame(bounds());
}

void StackView::setName(const char * name) {
  m_textView.setText(name);
}

void StackView::drawRect(KDContext * ctx, KDRect rect) const {
  KDCoordinate height = bounds().height();
  KDCoordinate width = bounds().width();
  ctx->fillRect(KDRect(0, 0, width, 1), Palette::LineColor);
  ctx->fillRect(KDRect(0, 1, width, height-2), KDColorWhite);
  ctx->fillRect(KDRect(0, height-1, width, 1), Palette::LineColor);
}

#if ESCHER_VIEW_LOGGING
const char * StackView::className() const {
  return "StackView";
}

void StackView::logAttributes(std::ostream &os) const {
  View::logAttributes(os);
  os << " name=\"" << m_name << "\"";
}
#endif

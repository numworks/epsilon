#include <escher/stack_view.h>
extern "C" {
#include <assert.h>
}

StackView::StackView() :
  View(),
  m_textView(PointerTextView(nullptr, 0.5f, 0.5f))
{
}

void StackView::setTextColor(KDColor textColor) {
  m_textView.setTextColor(textColor);
}

void StackView::setBackgroundColor(KDColor backgroundColor) {
  m_textView.setBackgroundColor(backgroundColor);
  m_backgroundColor = backgroundColor;
}

void StackView::setSeparatorColor(KDColor separatorColor) {
  m_separatorColor = separatorColor;
}

int StackView::numberOfSubviews() const {
  return 1;
}

View * StackView::subviewAtIndex(int index) {
  assert(index == 0);
  return &m_textView;
}

void StackView::layoutSubviews() {
  m_textView.setFrame(KDRect(0, 1,  bounds().width(), bounds().height()-2));
}

void StackView::setName(const char * name) {
  m_textView.setText(name);
}

void StackView::drawRect(KDContext * ctx, KDRect rect) const {
  KDCoordinate height = bounds().height();
  KDCoordinate width = bounds().width();
  ctx->fillRect(KDRect(0, 0, width, 1), m_separatorColor);
  ctx->fillRect(KDRect(0, 1, width, height-2), m_backgroundColor);
  ctx->fillRect(KDRect(0, height-1, width, 1), m_separatorColor);
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

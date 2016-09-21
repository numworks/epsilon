#include <escher/label_view.h>
#include <escher/table_view_cell.h>

LabelView::LabelView(const char * label, KDColor textColor, KDColor backgroundColor) :
  ChildlessView(),
  m_label(label),
  m_textColor(textColor),
  m_backgroundColor(backgroundColor)
{
}

void LabelView::setBackgroundColor(KDColor backgroundColor) {
  m_backgroundColor = backgroundColor;
}

void LabelView::setTextColor(KDColor textColor) {
  m_textColor = textColor;
}

void LabelView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->drawString(m_label, KDPointZero, m_textColor, m_backgroundColor);
}

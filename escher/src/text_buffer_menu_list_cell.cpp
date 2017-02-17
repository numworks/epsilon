#include <escher/text_buffer_menu_list_cell.h>
#include <assert.h>

TextBufferMenuListCell::TextBufferMenuListCell(char * accessoryText) :
  TableViewCell(),
  m_labelTextView(BufferTextView(KDText::FontSize::Large, 0.0f, 0.5f, KDColorBlack, KDColorWhite)),
  m_accessoryView(PointerTextView(KDText::FontSize::Small, accessoryText, 0.0f, 0.5f, Palette::GreyDark, KDColorWhite))
{
}

void TextBufferMenuListCell::setHighlighted(bool highlight) {
  TableViewCell::setHighlighted(highlight);
  KDColor backgroundColor = highlight? Palette::Select : KDColorWhite;
  m_labelTextView.setBackgroundColor(backgroundColor);
  m_accessoryView.setBackgroundColor(backgroundColor);
}

void TextBufferMenuListCell::setText(const char * text) {
  m_labelTextView.setText(text);
}

void TextBufferMenuListCell::setAccessoryText(const char * text) {
  m_accessoryView.setText(text);
}

void TextBufferMenuListCell::drawRect(KDContext * ctx, KDRect rect) const {
  KDCoordinate width = bounds().width();
  KDCoordinate height = bounds().height();
  KDColor backgroundColor = isHighlighted() ? Palette::Select : KDColorWhite;

  ctx->fillRect(KDRect(k_separatorThickness, k_separatorThickness, width-2*k_separatorThickness, height-k_separatorThickness), backgroundColor);
  ctx->fillRect(KDRect(0, 0, width, k_separatorThickness), Palette::GreyBright);
  ctx->fillRect(KDRect(0, k_separatorThickness, k_separatorThickness, height-k_separatorThickness), Palette::GreyBright);
  ctx->fillRect(KDRect(width-k_separatorThickness, k_separatorThickness, k_separatorThickness, height-k_separatorThickness), Palette::GreyBright);
 }

int TextBufferMenuListCell::numberOfSubviews() const {
  return 2;
}

View * TextBufferMenuListCell::subviewAtIndex(int index) {
  assert(index == 0 || index == 1);
  if (index == 0) {
    return &m_labelTextView;
  }
  return &m_accessoryView;
}

void TextBufferMenuListCell::layoutSubviews() {
  KDCoordinate width = bounds().width();
  KDCoordinate height = bounds().height();
  m_labelTextView.setFrame(KDRect(k_separatorThickness, k_separatorThickness, width, height/2 - k_separatorThickness));
  m_accessoryView.setFrame(KDRect(k_separatorThickness, height/2, width, height/2 - k_separatorThickness));
}

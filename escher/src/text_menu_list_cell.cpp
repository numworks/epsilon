#include <escher/text_menu_list_cell.h>

TextMenuListCell::TextMenuListCell(char * label) :
  MenuListCell(label),
  m_contentView(BufferTextView(1.0f, 0.5f))
{
}

void TextMenuListCell::setText(const char * textBody) {
  m_contentView.setText(textBody);
}

const char * TextMenuListCell::textContent() {
  return m_contentView.text();
}

View * TextMenuListCell::contentView() const {
  return (View *)&m_contentView;
}

void TextMenuListCell::reloadCell() {
  MenuListCell::reloadCell();
  KDColor backgroundColor = isHighlighted()? Palette::FocusCellBackgroundColor : Palette::CellBackgroundColor;
  m_contentView.setBackgroundColor(backgroundColor);
}

void TextMenuListCell::setHighlighted(bool highlight) {
  MenuListCell::setHighlighted(highlight);
  KDColor backgroundColor = highlight? Palette::FocusCellBackgroundColor : Palette::CellBackgroundColor;
  m_contentView.setBackgroundColor(backgroundColor);
}
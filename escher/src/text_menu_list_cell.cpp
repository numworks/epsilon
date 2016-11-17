#include <escher/text_menu_list_cell.h>

TextMenuListCell::TextMenuListCell(char * label) :
  MenuListCell(label),
  m_accessoryView(BufferTextView(1.0f, 0.5f))
{
}

void TextMenuListCell::setAccessoryText(const char * textBody) {
  m_accessoryView.setText(textBody);
  reloadCell();
}

const char * TextMenuListCell::accessoryText() {
  return m_accessoryView.text();
}

View * TextMenuListCell::accessoryView() const {
  return (View *)&m_accessoryView;
}

void TextMenuListCell::reloadCell() {
  MenuListCell::reloadCell();
  KDColor backgroundColor = isHighlighted()? Palette::FocusCellBackgroundColor : Palette::CellBackgroundColor;
  m_accessoryView.setBackgroundColor(backgroundColor);
}

void TextMenuListCell::setHighlighted(bool highlight) {
  MenuListCell::setHighlighted(highlight);
  KDColor backgroundColor = highlight? Palette::FocusCellBackgroundColor : Palette::CellBackgroundColor;
  m_accessoryView.setBackgroundColor(backgroundColor);
}
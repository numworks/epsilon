#include <escher/text_table_view_cell.h>

TextTableViewCell::TextTableViewCell(char * label) :
  TableViewCell(label),
  m_contentView(BufferTextView(1.0f, 0.5f))
{
}

void TextTableViewCell::setText(const char * textBody) {
  m_contentView.setText(textBody);
}

const char * TextTableViewCell::textContent() {
  return m_contentView.text();
}

View * TextTableViewCell::contentView() const {
  return (View *)&m_contentView;
}

void TextTableViewCell::setHighlighted(bool highlight) {
  TableViewCell::setHighlighted(highlight);
  KDColor backgroundColor = highlight? Palette::FocusCellBackgroundColor : Palette::CellBackgroundColor;
  m_contentView.setBackgroundColor(backgroundColor);
}
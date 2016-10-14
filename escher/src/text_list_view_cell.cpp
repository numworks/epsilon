#include <escher/text_list_view_cell.h>

TextListViewCell::TextListViewCell(char * label) :
  ListViewCell(label),
  m_contentView(BufferTextView(1.0f, 0.5f))
{
}

void TextListViewCell::setText(const char * textBody) {
  m_contentView.setText(textBody);
}

const char * TextListViewCell::textContent() {
  return m_contentView.text();
}

View * TextListViewCell::contentView() const {
  return (View *)&m_contentView;
}

void TextListViewCell::setHighlighted(bool highlight) {
  ListViewCell::setHighlighted(highlight);
  KDColor backgroundColor = highlight? Palette::FocusCellBackgroundColor : Palette::CellBackgroundColor;
  m_contentView.setBackgroundColor(backgroundColor);
}
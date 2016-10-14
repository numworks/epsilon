#include <escher/float_table_view_cell.h>

FloatTableViewCell::FloatTableViewCell(char * label) :
  TableViewCell(label),
  m_contentView(FloatView())
{
}

void FloatTableViewCell::setFloat(float f) {
  m_contentView.setFloat(f);
}

char * FloatTableViewCell::stringFromFloat() {
  return m_contentView.buffer();
}


View * FloatTableViewCell::contentView() const {
  return (View *)&m_contentView;
}

void FloatTableViewCell::setHighlighted(bool highlight) {
  TableViewCell::setHighlighted(highlight);
  KDColor backgroundColor = highlight? Palette::FocusCellBackgroundColor : Palette::CellBackgroundColor;
  m_contentView.setBackgroundColor(backgroundColor);
}
#include <escher/message_table_cell_with_chevron_and_buffer.h>
#include <escher/palette.h>

MessageTableCellWithChevronAndBuffer::MessageTableCellWithChevronAndBuffer(const KDFont * labelFont, const KDFont * subAccessoryFont) :
  MessageTableCellWithChevron((I18n::Message)0, labelFont),
  m_subAccessoryView(subAccessoryFont, 1.0f, 0.5f, Palette::GrayDark)
{
}

View * MessageTableCellWithChevronAndBuffer::subAccessoryView() const {
  return (View *)&m_subAccessoryView;
}

void MessageTableCellWithChevronAndBuffer::setHighlighted(bool highlight) {
  MessageTableCellWithChevron::setHighlighted(highlight);
  KDColor backgroundColor = isHighlighted()? Palette::Select : KDColorWhite;
  m_subAccessoryView.setBackgroundColor(backgroundColor);
}

void MessageTableCellWithChevronAndBuffer::setAccessoryText(const char * textBody) {
  m_subAccessoryView.setText(textBody);
  layoutSubviews();
}

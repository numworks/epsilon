#include <escher/message_table_cell_with_chevron.h>

template<>
MessageTableCellWithChevron<MessageTextView>::MessageTableCellWithChevron(I18n::Message message, const KDFont * font) :
  MessageTableCell<MessageTextView>(message, font),
  m_accessoryView()
{
}

template<>
MessageTableCellWithChevron<SlideableMessageTextView>::MessageTableCellWithChevron(I18n::Message message, const KDFont * font) :
  MessageTableCell<SlideableMessageTextView>(message, font,TableCell::Layout::HorizontalRightOverlap),
  m_accessoryView()
{
}

template<class T>
View * MessageTableCellWithChevron<T>::accessoryView() const {
  return (View *)&m_accessoryView;
}

template class MessageTableCellWithChevron<MessageTextView>;
template class MessageTableCellWithChevron<SlideableMessageTextView>;


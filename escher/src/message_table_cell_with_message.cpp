#include <escher/message_table_cell_with_message.h>
#include <escher/palette.h>
#include <string.h>

template <class T>
MessageTableCellWithMessage<T>::MessageTableCellWithMessage(I18n::Message message, TableCell::Layout layout) :
  MessageTableCell<T>(message, KDFont::SmallFont, layout),
  m_accessoryView(KDFont::SmallFont, (I18n::Message)0, 0.0f, 0.5f)
{
  if (layout != TableCell::Layout::Vertical) {
    m_accessoryView.setAlignment(1.0f, 0.5f);
  }
}

template <class T>
void MessageTableCellWithMessage<T>::setAccessoryMessage(I18n::Message textBody) {
  m_accessoryView.setMessage(textBody);
  this->reloadCell();
}

template <class T>
View * MessageTableCellWithMessage<T>::accessoryView() const {
  if (strlen(m_accessoryView.text()) == 0) {
    return nullptr;
  }
  return (View *)&m_accessoryView;
}

template <class T>
void MessageTableCellWithMessage<T>::setHighlighted(bool highlight) {
  MessageTableCell<T>::setHighlighted(highlight);
  KDColor backgroundColor = this->isHighlighted()? Palette::ListCellBackgroundSelected : Palette::ListCellBackground;
  m_accessoryView.setBackgroundColor(backgroundColor);
}

template <class T>
void MessageTableCellWithMessage<T>::setTextColor(KDColor color) {
  m_accessoryView.setTextColor(color);
  MessageTableCell<T>::setTextColor(color);
}

template <class T>
void MessageTableCellWithMessage<T>::setAccessoryTextColor(KDColor color) {
  m_accessoryView.setTextColor(color);
}

template class MessageTableCellWithMessage<MessageTextView>;
template class MessageTableCellWithMessage<SlideableMessageTextView>;

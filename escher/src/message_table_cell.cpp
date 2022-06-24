#include <escher/message_table_cell.h>
#include <escher/palette.h>
#include <escher/slideable_message_text_view.h>
#include <assert.h>

template<class T>
MessageTableCell<T>::MessageTableCell(I18n::Message label, const KDFont * font, Layout layout) :
  TableCell(layout),
  m_messageTextView(font, label, 0, 0.5, Palette::PrimaryText, Palette::ListCellBackground),
  m_backgroundColor(KDColorWhite)
{
}

template<class T>
View * MessageTableCell<T>::labelView() const {
  return (View *)&m_messageTextView;
}

template<>
void MessageTableCell<SlideableMessageTextView>::setHighlighted(bool highlight) {
  HighlightCell::setHighlighted(highlight);
  KDColor backgroundColor = highlight? Palette::ListCellBackgroundSelected : Palette::ListCellBackground;
  m_messageTextView.setBackgroundColor(backgroundColor);
  static AnimationTimer s_animationTimer = AnimationTimer();
  if (highlight) {
    m_messageTextView.willStartAnimation();
    s_animationTimer.setAnimated(&m_messageTextView);
  } else {
    s_animationTimer.removeAnimated(&m_messageTextView);
    m_messageTextView.didStopAnimation();
  }
}

template<>
void MessageTableCell<MessageTextView>::setHighlighted(bool highlight) {
  HighlightCell::setHighlighted(highlight);
  KDColor backgroundColor = highlight? Palette::ListCellBackgroundSelected : Palette::ListCellBackground;
  m_messageTextView.setBackgroundColor(backgroundColor);
}

template<class T>
void MessageTableCell<T>::setMessage(I18n::Message text) {
  m_messageTextView.setMessage(text);
  layoutSubviews();
}

template<class T>
void MessageTableCell<T>::setTextColor(KDColor color) {
  m_messageTextView.setTextColor(color);
}

template<class T>
void MessageTableCell<T>::setMessageFont(const KDFont * font) {
  m_messageTextView.setFont(font);
  layoutSubviews();
}

template<class T>
void MessageTableCell<T>::setBackgroundColor(KDColor color) {
  m_backgroundColor = color;
  m_messageTextView.setBackgroundColor(color);
}

template class MessageTableCell<MessageTextView>;
template class MessageTableCell<SlideableMessageTextView>;

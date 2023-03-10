#ifndef ESCHER_MESSAGE_TEXT_VIEW_H
#define ESCHER_MESSAGE_TEXT_VIEW_H

#include <escher/i18n.h>
#include <escher/text_view.h>

namespace Escher {

class MessageTextView : public TextView {
 public:
  MessageTextView(I18n::Message message = (I18n::Message)0,
                  CellWidget::Type type = CellWidget::Type::Label)
      : TextView(type), m_message(message) {}

  MessageTextView(
      KDFont::Size font, I18n::Message message,
      float horizontalAlignment = k_defaultLabel.horizontalAlginment,
      float verticalAlignment = k_defaultAccessory.verticalAlignment,
      KDColor textColor = k_defaultLabel.textColor,
      KDColor backgroundColor = k_defaultLabel.backgroundColor)
      : TextView(font, horizontalAlignment, verticalAlignment, textColor,
                 backgroundColor),
        m_message(message) {}
  void setText(const char* text) override;
  void setMessage(I18n::Message message);
  const char* text() const override;
  KDSize minimalSizeForOptimalDisplay() const override;

 private:
  I18n::Message m_message;
};

}  // namespace Escher

#endif

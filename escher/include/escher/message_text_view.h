#ifndef ESCHER_MESSAGE_TEXT_VIEW_H
#define ESCHER_MESSAGE_TEXT_VIEW_H

#include <escher/cell_widget.h>
#include <escher/i18n.h>
#include <escher/text_view.h>

namespace Escher {

class MessageTextView : public TextView, public CellWidget {
 public:
  MessageTextView(KDFont::Size font = KDFont::Size::Large,
                  I18n::Message message = (I18n::Message)0,
                  float horizontalAlignment = 0.0f,
                  float verticalAlignment = 0.0f,
                  KDColor textColor = KDColorBlack,
                  KDColor backgroundColor = KDColorWhite);
  void setText(const char* text) override;
  void setMessage(I18n::Message message);
  const char* text() const override;
  KDSize minimalSizeForOptimalDisplay() const override;

  // CellWidget
  const View* view() const override { return this; }
  void setWidgetBackgroundColor(KDColor color) override {
    setBackgroundColor(color);
  }

 private:
  I18n::Message m_message;
};

}  // namespace Escher

#endif

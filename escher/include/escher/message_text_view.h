#ifndef ESCHER_MESSAGE_TEXT_VIEW_H
#define ESCHER_MESSAGE_TEXT_VIEW_H

#include <escher/cell_widget.h>
#include <escher/i18n.h>
#include <escher/palette.h>
#include <escher/text_view.h>

namespace Escher {

class MessageTextView : public TextView, public CellWidget {
 public:
  MessageTextView(I18n::Message message = (I18n::Message)0,
                  CellWidget::Type type = CellWidget::Type::Label)
      : m_message(message) {
    defaultInitialization(type);
  }

  MessageTextView(KDFont::Size font, I18n::Message message,
                  float horizontalAlignment, float verticalAlignment,
                  KDColor textColor = KDColorBlack,
                  KDColor backgroundColor = KDColorWhite)
      : TextView(font, horizontalAlignment, verticalAlignment, textColor,
                 backgroundColor),
        m_message(message) {}
  void setText(const char* text) override;
  void setMessage(I18n::Message message);
  const char* text() const override;
  KDSize minimalSizeForOptimalDisplay() const override;

  // CellWidget
  const View* view() const override { return this; }
  // TODO: Move this up in TextView when BufferTextView becomes a widget
  void defaultInitialization(CellWidget::Type type) override {
    bool isLabel = type == CellWidget::Type::Label;
    setFont(isLabel ? KDFont::Size::Large : KDFont::Size::Small);
    setTextColor(isLabel ? KDColorBlack : Palette::GrayDark);
    setBackgroundColor(KDColorWhite);
    setAlignment(KDContext::k_alignLeft,
                 isLabel ? KDContext::k_alignTop : KDContext::k_alignCenter);
  }
  void setWidgetBackgroundColor(KDColor color) override {
    setBackgroundColor(color);
  }

 private:
  I18n::Message m_message;
};

}  // namespace Escher

#endif

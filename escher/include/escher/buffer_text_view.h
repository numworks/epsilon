#ifndef ESCHER_BUFFER_TEXT_VIEW_H
#define ESCHER_BUFFER_TEXT_VIEW_H

#include <escher/i18n.h>
#include <escher/text_view.h>
#include <stdarg.h>

namespace Escher {

class BufferTextView : public TextView {
  friend class BufferTableCell;
  friend class BufferPopUpController;

 public:
  constexpr static size_t k_maxNumberOfChar = 256;
  BufferTextView(CellWidget::Type type = CellWidget::Type::Label)
      : TextView(type), m_maxDisplayedTextLength(k_maxNumberOfChar - 1) {}
  BufferTextView(KDFont::Size font, float horizontalAlignment,
                 float verticalAlignment, KDColor textColor = KDColorBlack,
                 KDColor backgroundColor = KDColorWhite,
                 size_t maxDisplayedTextLength = k_maxNumberOfChar - 1);

  // View
  KDSize minimalSizeForOptimalDisplay() const override;

  // TextView
  const char* text() const override;
  void setText(const char* text) override;

  void setMessageWithPlaceholders(I18n::Message message, ...);
  void appendText(const char* text);

 protected:
  void privateSetMessageWithPlaceholders(I18n::Message message, va_list args);
  char m_buffer[k_maxNumberOfChar];
  size_t m_maxDisplayedTextLength;
};

}  // namespace Escher

#endif

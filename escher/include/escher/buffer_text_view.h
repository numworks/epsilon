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
  BufferTextView(KDFont::Size font = KDFont::Size::Large,
                 float horizontalAlignment = KDContext::k_alignLeft,
                 float verticalAlignment = KDContext::k_alignCenter,
                 KDColor textColor = KDColorBlack,
                 KDColor backgroundColor = KDColorWhite,
                 size_t maxDisplayedTextLength = k_maxNumberOfChar - 1);
  void setText(const char * text) override;

  void setMessageWithPlaceholders(I18n::Message message, ...);
  const char * text() const override;
  void appendText(const char * text);
  KDSize minimalSizeForOptimalDisplay() const override;
protected:
  void privateSetMessageWithPlaceholders(I18n::Message message, va_list args);
  char m_buffer[k_maxNumberOfChar];
  size_t m_maxDisplayedTextLength;
};

}

#endif

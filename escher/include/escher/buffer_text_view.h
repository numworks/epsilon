#ifndef ESCHER_BUFFER_TEXT_VIEW_H
#define ESCHER_BUFFER_TEXT_VIEW_H

#include <escher/i18n.h>
#include <escher/text_view.h>

namespace Escher {

class BufferTextView : public TextView {
public:
  static constexpr size_t k_maxNumberOfChar = 256;
  BufferTextView(const KDFont * font = KDFont::LargeFont,
                 float horizontalAlignment = KDContext::k_alignLeft,
                 float verticalAlignment = KDContext::k_alignCenter,
                 KDColor textColor = KDColorBlack,
                 KDColor backgroundColor = KDColorWhite,
                 size_t maxDisplayedTextLength = k_maxNumberOfChar - 1);
  void setText(const char * text) override;

  /* This method only combine a message and 1 string because it is the most common case.
  * TODO : It should be rewritten to take a va_list to combine a message with more strings.
  */
  void setSimpleCustomText(I18n::Message message, const char * string = "");

  const char * text() const override;
  void appendText(const char * text);
  KDSize minimalSizeForOptimalDisplay() const override;
protected:
  char m_buffer[k_maxNumberOfChar];
  size_t m_maxDisplayedTextLength;
};

}

#endif

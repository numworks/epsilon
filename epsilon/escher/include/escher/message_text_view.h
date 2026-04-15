#pragma once

#include <escher/i18n.h>

#include "raw_color_glyphs_view.h"
#include "text_view.h"

namespace Escher {

class MessageTextView : public TextView {
 public:
  using TitleType = I18n::Message;

  MessageTextView(I18n::Message message = (I18n::Message)0,
                  const TextFormat& format = {}, KDCoordinate lineSpacing = 0)
      : TextView(format, lineSpacing), m_message(message) {}

  void setText(const char* text) override;
  void setMessage(I18n::Message message);
  const char* text() const override;

 private:
  I18n::Message m_message;
};

/* RawColorMessageTextView is a special version of MessageTextView which works
 * with raw glyph and background colors instead of ThemeComponent. */
class RawColorMessageTextView : public RawColor<MessageTextView> {
 public:
  RawColorMessageTextView(I18n::Message message = (I18n::Message)0,
                          const TextFormatWithoutColors& format = {},
                          const TextColors& colors = {},
                          KDCoordinate lineSpacing = 0)
      : RawColor<MessageTextView>(message, MakeTextFormat(format),
                                  lineSpacing) {
    this->m_rawColors = colors;
  }
};

}  // namespace Escher

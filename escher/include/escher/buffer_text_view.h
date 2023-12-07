#ifndef ESCHER_BUFFER_TEXT_VIEW_H
#define ESCHER_BUFFER_TEXT_VIEW_H

#include <escher/i18n.h>
#include <escher/text_view.h>
#include <poincare/print_float.h>
#include <stdarg.h>

namespace Escher {

class AbstractBufferTextView : public TextView {
  friend class BufferPopUpController;

 public:
  using TextView::TextView;

  // View
  KDSize minimalSizeForOptimalDisplay() const override;

  // TextView
  const char* text() const override {
    return const_cast<AbstractBufferTextView*>(this)->buffer();
  }
  void setText(const char* text) override;

  void setMessageWithPlaceholders(I18n::Message message, ...);
  bool unsafeSetMessageWithPlaceholders(I18n::Message message, ...);
  void appendText(const char* text);

 protected:
  bool privateSetMessageWithPlaceholders(I18n::Message message, va_list args);
  virtual char* buffer() = 0;
  virtual int maxTextSize() const = 0;
};

template <size_t BufferSize>
class BufferTextView : public AbstractBufferTextView {
 public:
  BufferTextView(KDGlyph::Format format = {}) : AbstractBufferTextView(format) {
    buffer()[0] = 0;
  }
  constexpr static size_t MaxTextSize() { return BufferSize; }

 protected:
  char* buffer() override { return m_buffer; }
  int maxTextSize() const override { return BufferSize; }
  char m_buffer[BufferSize];
};

template <KDFont::Size Font, int NumberOfLines>
using MultipleLinesBufferTextView = BufferTextView<
    NumberOfLines*(Ion::Display::Width / KDFont::GlyphWidth(Font)) + 1>;
/* TODO: NumberOfLines * (Ion::Display::Width / KDFont::GlyphWidth(Font)) is a
 * size in terms of glyphs which is different from the size in terms of chars */

template <KDFont::Size Font = KDFont::Size::Small>
using OneLineBufferTextView = MultipleLinesBufferTextView<Font, 1>;

template <int numberOfSignificantDigits =
              Poincare::PrintFloat::k_maxNumberOfSignificantDigits>
using FloatBufferTextView =
    BufferTextView<Poincare::PrintFloat::charSizeForFloatsWithPrecision(
        numberOfSignificantDigits)>;

}  // namespace Escher

#endif

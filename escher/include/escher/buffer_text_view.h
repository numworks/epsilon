#ifndef ESCHER_BUFFER_TEXT_VIEW_H
#define ESCHER_BUFFER_TEXT_VIEW_H

#include <escher/text_view.h>

class BufferTextView : public TextView {
public:
  BufferTextView(KDText::FontSize size = KDText::FontSize::Large, float horizontalAlignment = 0.5f, float verticalAlignment = 0.5f,
    KDColor textColor = KDColorBlack, KDColor backgroundColor = KDColorWhite);
  void setText(const char * text) override;
  const char * text() const override;
  void appendText(const char * text);
  static int maxNumberOfCharsInBuffer() { return k_maxNumberOfChar; }
private:
  static constexpr int k_maxNumberOfChar = 256;
  char m_buffer[k_maxNumberOfChar];
};

#endif

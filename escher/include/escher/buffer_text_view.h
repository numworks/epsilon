#ifndef ESCHER_BUFFER_TEXT_VIEW_H
#define ESCHER_BUFFER_TEXT_VIEW_H

#include <escher/text_view.h>

class BufferTextView : public TextView {
public:
  BufferTextView(float horizontalAlignment, float verticalAlignment,
    KDColor textColor = KDColorBlack, KDColor backgroundColor = KDColorWhite);
  void setText(const char * text);
  const char * text() const override;
private:
  static constexpr int k_maxNumberOfChar = 256;
  char m_buffer[k_maxNumberOfChar];
};

#endif

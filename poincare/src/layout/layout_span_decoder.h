#ifndef POINCARE_LAYOUT_LAYOUT_SPAN_DECODER_H
#define POINCARE_LAYOUT_LAYOUT_SPAN_DECODER_H

#include <omg/unicode_helper.h>
#include <omg/utf8_decoder.h>

#include "code_point_layout.h"
#include "layout_span.h"
#include "rack.h"

namespace Poincare::Internal {

/* LayoutSpanDecoder adds state over the LayoutSpan and is a
 * ForwardUnicodeDecoder (it can move forward but not backward)
 * It has to be copied to get back to a previous state.
 */
class LayoutSpanDecoder : public ForwardUnicodeDecoder {
 public:
  LayoutSpanDecoder(const Layout* start, size_t length)
      : ForwardUnicodeDecoder(0, length), m_layout(start), m_length(length) {}

  LayoutSpanDecoder(const Rack* rack, size_t initialPosition = 0,
                    size_t lastPosition = k_noSize)
      : LayoutSpanDecoder(rack->child(initialPosition),
                          (lastPosition == k_noSize ? rack->numberOfChildren()
                                                    : lastPosition) -
                              initialPosition) {}

  LayoutSpanDecoder(LayoutSpan span)
      : LayoutSpanDecoder(span.start, span.length) {}

  bool isEmpty() const { return m_length == 0; }

  const Layout* layout() const { return m_layout; }
  CodePoint codePoint() override {
    return m_length > 0 && m_layout->isCodePointLayout()
               ? CodePointLayout::GetCodePoint(m_layout)
               : UCodePointNull;
  }

  CodePoint nextCodePoint() override {
    CodePoint cp = codePoint();
    next();
    return cp;
  }

  bool nextLayoutIsCodePoint() {
    return m_length == 0 || m_layout->isCodePointLayout();
  }

  const Layout* nextLayout() {
    const Layout* result = layout();
    next();
    return result;
  }

  LayoutSpan toSpan() { return LayoutSpan{m_layout, m_length}; }

  void skip(int n) {
    while (n--) {
      next();
    }
  }

 private:
  void next() {
    // next will be called when m_length is 0 but normally only once
    if (m_length > 0) {
      m_layout = static_cast<const Layout*>(m_layout->nextTree());
      m_position++;
      m_length--;
    }
  }

  const Layout* m_layout;
  uint16_t m_length;
};

inline int CompareLayoutSpanWithNullTerminatedString(
    const Poincare::Internal::LayoutSpan a, const char* b) {
  LayoutSpanDecoder da(a.start, a.length);
  UTF8Decoder db(b);
  return OMG::CompareDecoders(&da, &db);
}

}  // namespace Poincare::Internal

#endif

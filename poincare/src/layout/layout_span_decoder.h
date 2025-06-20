#ifndef POINCARE_LAYOUT_LAYOUT_SPAN_DECODER_H
#define POINCARE_LAYOUT_LAYOUT_SPAN_DECODER_H

#include <omg/unicode_helper.h>
#include <omg/utf8_decoder.h>
#include <stdint.h>

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
      : ForwardUnicodeDecoder(0, length),
        m_layout(start),
        m_length(static_cast<uint16_t>(length)) {
    assert(m_length == m_end - m_position);
    /*  If a LayoutSpanDecoder is constructed with a very long length, it is
     * probably due to a size_t overflow */
    assert(length <= UINT16_MAX);
  }

  LayoutSpanDecoder(const Rack* rack, size_t initialPosition = 0,
                    size_t lastPosition = k_noSize)
      : LayoutSpanDecoder(
            initialPosition == 0 ? static_cast<const Layout*>(rack->nextNode())
            /* initialPosition is allowed to be lastPosition for
             * convenience, in that case m_layout is nullptr and
             * should not be dereferenced */
            : static_cast<int>(initialPosition) < rack->numberOfChildren()
                ? rack->child(initialPosition)
                : nullptr,
            (lastPosition == k_noSize ? rack->numberOfChildren()
                                      : lastPosition) -
                initialPosition) {
    /* Entering this constructor with lastPosition < initialPosition indicates
     * that something unexpected or wrong happened. */
    assert(lastPosition >= initialPosition);
  }

  explicit LayoutSpanDecoder(LayoutSpan span)
      : LayoutSpanDecoder(span.data(), span.size()) {}

  bool isEmpty() const override { return m_length == 0; }

  const Layout* layout() const {
    assert(m_length != k_outOfRange);
    return m_layout;
  }

  CodePoint codePoint() override {
    if (isEmpty()) {
      return UCodePointNull;
    }
    assert(m_layout != nullptr);
    assert(m_length != k_outOfRange);
    return (m_layout->isCodePointLayout() ||
            m_layout->isCombinedCodePointsLayout())
               ? CodePointLayout::GetCodePoint(m_layout)
               : UCodePointNull;
  }

  CodePoint combiningCodePoint() const {
    assert(m_layout != nullptr);
    assert(m_length != k_outOfRange);
    return m_length > 0 && m_layout->isCombinedCodePointsLayout()
               ? CodePointLayout::GetCombiningCodePoint(m_layout)
               : UCodePointNull;
  }

  CodePoint nextCodePoint() override {
    CodePoint cp = codePoint();
    next();
    return cp;
  }

  bool nextLayoutIsCodePoint() const {
    /* Return true if the decoder is empty for functions that are looping on
     * codepoints until they hit a null codepoints. */
    assert(m_layout != nullptr);
    assert(m_length != k_outOfRange);
    return m_length == 0 || m_layout->isCodePointLayout();
  }

  bool nextLayoutIsCombinedCodePoint() const {
    assert(m_layout != nullptr);
    assert(m_length != k_outOfRange);
    return m_layout->isCombinedCodePointsLayout();
  }

  const Layout* nextLayout() {
    const Layout* result = layout();
    next();
    return result;
  }

  LayoutSpan toSpan() const { return LayoutSpan{m_layout, m_length}; }

  void skip(int n) {
    while (n--) {
      next();
    }
  }

 private:
#if ASSERTIONS
  /* For optimization purposes, the next() function can be called when m_length
   * is 0, but only once. After this last call, m_layout becomes invalid as well
   */
  static constexpr uint16_t k_outOfRange = UINT16_MAX;
#endif

  void next();
  const Layout* m_layout;  // allowed to be a nullptr
  uint16_t m_length;
};

inline int CompareLayoutSpanWithNullTerminatedString(const LayoutSpan a,
                                                     const char* b) {
  LayoutSpanDecoder da(a.data(), a.size());
  UTF8Decoder db(b);
  return OMG::CompareDecoders(&da, &db);
}

}  // namespace Poincare::Internal

#endif

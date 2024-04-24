#ifndef POINCARE_LAYOUT_LAYOUT_SPAN_DECODER_H
#define POINCARE_LAYOUT_LAYOUT_SPAN_DECODER_H

#include <omg/unicode_helper.h>
#include <omg/utf8_decoder.h>

#include "code_point_layout.h"
#include "layout_span.h"
#include "rack.h"

namespace Poincare::Internal {

/* ideas/guidelines :
 *   use span by copy most of the time, especially in signatures, it is easier
 * to reason about
 *
 *   remove the default noSize argument on CPLayoutDecoder and spans since we
 *   always need to know the max there is no \0
 *
 *   either remove the need for previousCodePoint (probably useless) or use a
 * rack based indexing
 *
 *   copy the all decoder to restore the position ?
 *
 *  move end and position up from unicodedecoder to allow child to use the best
 * one
 *
 *  when an helper has to work on racks and str, take a unicodedecoder* and
 *  create wrappers as needed
 */

/* the decoder adds state over the span, it can move forward but not backward
 * copy it if you need to get back to a previous state,
 * check the codepoint before going forward if you are not sure you want to go
 * forward
 */

/* Unlike RackLayoutDecoder that iters children of a parent rack, this points to
 * a CodePointLayout and iter its siblings until a non-codepoint layout is
 * reached. Thus the API is more similar to const char *. */
/* TODO move to cpp */
class LayoutSpanDecoder : public UnicodeDecoder {
 public:
  LayoutSpanDecoder(const Layout* start, size_t length)
      : UnicodeDecoder(0, length), m_layout(start), m_length(length) {}

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
  CodePoint codePoint() const {
    if (m_length == 0) {
      return UCodePointNull;
    }
    return m_layout->isCodePointLayout()
               ? CodePointLayout::GetCodePoint(m_layout)
               : UCodePointNull;
  }

  CodePoint nextCodePoint() override {
    CodePoint cp = codePoint();
    next();
    return cp;
  }
  CodePoint previousCodePoint() override { assert(false); }

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

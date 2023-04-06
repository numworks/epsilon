#ifndef POINCARE_LINEAR_LAYOUT_DECODER_H
#define POINCARE_LINEAR_LAYOUT_DECODER_H

#include <ion/unicode/utf8_decoder.h>

#include "code_point_layout.h"
#include "horizontal_layout.h"

namespace Poincare {

/* This helper provides a codepoint string api to HorizontalLayout containing
 * only CodePointLayouts (used in layout fields in linear mode) */

class LinearLayoutDecoder : public UnicodeDecoder {
 public:
  LinearLayoutDecoder(const HorizontalLayout layout, size_t initialPosition = 0,
                      size_t layoutEnd = 0)
      : UnicodeDecoder(initialPosition,
                       layoutEnd ? layoutEnd : layout.numberOfChildren()),
        m_layout(layout) {
    assert(m_layout.isCodePointsString());
    assert(!m_layout.isUninitialized());
  }
  CodePoint nextCodePoint() { return codePointAt(m_position++); }
  CodePoint previousCodePoint() { return codePointAt(--m_position); }

 private:
  CodePoint codePointAt(size_t index) {
    if (index == m_end) {
      return UCodePointNull;
    }
    assert(0 <= index && index < m_end);
    Layout child = m_layout.childAtIndex(index);
    assert(child.type() == LayoutNode::Type::CodePointLayout);
    return static_cast<CodePointLayout&>(child).codePoint();
  }
  const HorizontalLayout m_layout;
};

}  // namespace Poincare

#endif

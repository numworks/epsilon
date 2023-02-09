#ifndef POINCARE_LINEAR_LAYOUT_DECODER_H
#define POINCARE_LINEAR_LAYOUT_DECODER_H

#include "horizontal_layout.h"
#include "code_point_layout.h"
#include <ion/unicode/utf8_decoder.h>

namespace Poincare {

class LayoutDecoder : public UnicodeDecoder {
public:
  LayoutDecoder(const HorizontalLayout layout, size_t initialPosition = 0, size_t layoutEnd = 0) :
    UnicodeDecoder(0, initialPosition, layoutEnd),
    m_layout(layout)
  {
    assert(!m_layout.isUninitialized());
  }
  CodePoint nextCodePoint() { return codePointAt(m_stringPosition++); }
  CodePoint previousCodePoint() { return codePointAt(--m_stringPosition); }
private:
  CodePoint codePointAt(size_t index) {
    assert(0 <= index && index < reinterpret_cast<size_t>(m_stringEnd));
    Layout child = m_layout.childAtIndex(index);
    assert(child.type() == LayoutNode::Type::CodePointLayout);
    return static_cast<CodePointLayout&>(child).codePoint();
  }
  const HorizontalLayout m_layout;
};

}

#endif

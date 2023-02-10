#ifndef POINCARE_LINEAR_LAYOUT_DECODER_H
#define POINCARE_LINEAR_LAYOUT_DECODER_H

#include "horizontal_layout.h"
#include "code_point_layout.h"

namespace Poincare {

class LayoutDecoder {
public:
  LayoutDecoder(const HorizontalLayout layout, int initialPosition = 0, int layoutEnd = 0) :
    m_layout(layout),
    m_layoutEnd(layoutEnd),
    m_position(initialPosition)
  {
    assert(!m_layout.isUninitialized());
  }
  CodePoint nextCodePoint() { return codePointAt(m_position++); }
  CodePoint previousCodePoint() { return codePointAt(--m_position); }
  int position() const { return m_position; }
private:
  CodePoint codePointAt(int index) {
    assert(0 <= index && index < m_layoutEnd);
    Layout child = m_layout.childAtIndex(index);
    assert(child.type() == LayoutNode::Type::CodePointLayout);
    return static_cast<CodePointLayout&>(child).codePoint();
  }
  const HorizontalLayout m_layout;
  int m_layoutEnd;
  int m_position;
};

}

#endif

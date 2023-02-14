#ifndef POINCARE_JUNIOR_LAYOUT_RACK_LAYOUT_DECODER_H
#define POINCARE_JUNIOR_LAYOUT_RACK_LAYOUT_DECODER_H

#include "rack_layout.h"
#include "code_point_layout.h"
#include <ion/unicode/utf8_decoder.h>

namespace PoincareJ {

class RackLayoutDecoder : public UnicodeDecoder {
public:
  RackLayoutDecoder(const Node layout, size_t initialPosition = 0, size_t layoutEnd = 0) :
    UnicodeDecoder(0, initialPosition, layoutEnd),
    m_layout(layout)
  {
    assert(layout.block()->type() == BlockType::RackLayout);
  }
  CodePoint nextCodePoint() { return codePointAt(m_stringPosition++); }
  CodePoint previousCodePoint() { return codePointAt(--m_stringPosition); }
private:
  CodePoint codePointAt(size_t index) {
    assert(0 <= index && index < reinterpret_cast<size_t>(m_stringEnd));
    return CodePointLayout::GetCodePoint(m_layout.childAtIndex(index));
  }
  const Node m_layout;
};

}

#endif

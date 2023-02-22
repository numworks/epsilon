#ifndef POINCARE_JUNIOR_LAYOUT_RACK_LAYOUT_DECODER_H
#define POINCARE_JUNIOR_LAYOUT_RACK_LAYOUT_DECODER_H

#include "rack_layout.h"
#include "code_point_layout.h"
#include <ion/unicode/utf8_decoder.h>

namespace PoincareJ {

/* If we decide to keep StringLayouts, the RackLayoutDecoder should probably
 * enter strings and yield its codepoints instead of yielding the layout */

class RackLayoutDecoder : public UnicodeDecoder {
public:
  RackLayoutDecoder(const Node layout, size_t initialPosition = 0, size_t layoutEnd = 0) :
    UnicodeDecoder(0, initialPosition, layoutEnd),
    m_layout(layout)
  {
    assert(layout.type() == BlockType::RackLayout);
  }
  const Node mainLayout() const { return m_layout; }
  Node nextLayout() { return layoutAt(m_stringPosition++); }
  bool nextLayoutIsCodePoint() { return m_stringPosition < m_stringEnd && m_layout.childAtIndex(m_stringPosition+1).type() == BlockType::CodePointLayout; }
  CodePoint nextCodePoint() { return codePointAt(m_stringPosition++); }
  CodePoint previousCodePoint() { return codePointAt(--m_stringPosition); }
  void setPosition(size_t index) {
    assert(0 <= index && index < reinterpret_cast<size_t>(m_stringEnd));
    m_stringPosition = index;
  }
  void setPosition(Node child) {
    m_stringPosition = m_layout.hasChild(child) ? m_layout.indexOfChild(child) : m_stringEnd;
  }
  Node layoutAt(size_t index) {
    // if (index == reinterpret_cast<size_t>(m_stringEnd)) {
      // return UCodePointNull;
    // }
    assert(0 <= index && index < reinterpret_cast<size_t>(m_stringEnd));
    return m_layout.childAtIndex(index);
  }
  CodePoint codePointAt(size_t index) const {
    if (index == reinterpret_cast<size_t>(m_stringEnd)) {
      return UCodePointNull;
    }
    assert(0 <= index && index < reinterpret_cast<size_t>(m_stringEnd));
    assert(m_layout.childAtIndex(index).type() == BlockType::CodePointLayout);
    return CodePointLayout::GetCodePoint(m_layout.childAtIndex(index));
  }
private:
  const Node m_layout;
};

}

#endif

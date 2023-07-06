#ifndef POINCARE_JUNIOR_LAYOUT_RACK_LAYOUT_DECODER_H
#define POINCARE_JUNIOR_LAYOUT_RACK_LAYOUT_DECODER_H

#include <ion/unicode/utf8_decoder.h>

#include "code_point_layout.h"
#include "rack_layout.h"

namespace PoincareJ {

/* TODO: If we decide to keep StringLayouts, the RackLayoutDecoder should
 * probably enter strings and yield its codepoints instead of yielding the
 * layout */

class RackLayoutDecoder : public UnicodeDecoder {
 public:
  RackLayoutDecoder(const Tree* layout, size_t initialPosition = 0,
                    size_t layoutEnd = 0)
      : UnicodeDecoder(initialPosition,
                       layoutEnd ? layoutEnd : layout->numberOfChildren()),
        m_layout(layout) {
    assert(layout->type() == BlockType::RackLayout);
    if (m_position > m_end) {
      m_position = m_end;
    }
  }
  const Tree* mainLayout() const { return m_layout; }
  const Tree* nextLayout() { return layoutAt(m_position++); }
  bool nextLayoutIsCodePoint() {
    return m_position == m_end ||
           (m_position < m_end && m_layout->childAtIndex(m_position)->type() ==
                                      BlockType::CodePointLayout);
  }
  CodePoint nextCodePoint() { return codePointAt(m_position++); }
  CodePoint previousCodePoint() { return codePointAt(--m_position); }
  void setPosition(size_t index) {
    assert(0 <= index && index <= m_end);
    m_position = index;
  }
  void setPosition(const Tree* child) {
    m_position =
        m_layout->hasChild(child) ? m_layout->indexOfChild(child) : m_end;
  }
  const Tree* layoutAt(size_t index) {
    assert(0 <= index && index < m_end);
    return m_layout->childAtIndex(index);
  }
  CodePoint codePointAt(size_t index) const {
    if (index == m_end) {
      return UCodePointNull;
    }
    assert(0 <= index && index < m_end);
    assert(m_layout->childAtIndex(index)->type() == BlockType::CodePointLayout);
    return CodePointLayout::GetCodePoint(m_layout->childAtIndex(index));
  }

 private:
  const Tree* m_layout;
};

}  // namespace PoincareJ

#endif

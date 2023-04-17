#include <poincare/code_point_layout.h>
#include <poincare/combined_code_points_layout.h>
#include <poincare/linear_layout_decoder.h>

namespace Poincare {

LinearLayoutDecoder::LinearLayoutDecoder(const HorizontalLayout layout,
                                         size_t initialPosition,
                                         size_t layoutEnd)
    : UnicodeDecoder(initialPosition,
                     layoutEnd ? layoutEnd : layout.numberOfChildren()),
      m_layout(layout),
      m_insideCombinedCodePoint(false) {
  assert(m_layout.isCodePointsString());
  assert(!m_layout.isUninitialized());
}

CodePoint LinearLayoutDecoder::nextCodePointAtDirection(
    OMG::HorizontalDirection direction) {
  if (direction == OMG::Direction::Left() && !m_insideCombinedCodePoint) {
    m_position--;
  }
  if (m_position == m_end) {
    return UCodePointNull;
  }
  assert(0 <= m_position && m_position < m_end);
  Layout child = m_layout.childAtIndex(m_position);
  CodePoint result = UCodePointNull;
  if (child.type() == LayoutNode::Type::CodePointLayout) {
    result = static_cast<CodePointLayout&>(child).codePoint();
  } else {
    assert(child.type() == LayoutNode::Type::CombinedCodePointsLayout);
    CombinedCodePointsLayout ccpl =
        static_cast<CombinedCodePointsLayout&>(child);
    result =
        (m_insideCombinedCodePoint == (direction == OMG::Direction::Right()))
            ? ccpl.combinedCodePoint()
            : ccpl.codePoint();
    m_insideCombinedCodePoint = !m_insideCombinedCodePoint;
  }
  if (direction == OMG::Direction::Right() && !m_insideCombinedCodePoint) {
    m_position++;
  }
  return result;
}

}  // namespace Poincare

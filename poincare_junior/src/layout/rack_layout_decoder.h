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
    assert(layout->isRackLayout());
    if (m_position > m_end) {
      m_position = m_end;
    }
  }
  const Tree* mainLayout() const { return m_layout; }
  const Tree* nextLayout() { return layoutAt(m_position++); }
  bool nextLayoutIsCodePoint() {
    return m_position == m_end ||
           (m_position < m_end &&
            m_layout->child(m_position)->isCodePointLayout());
  }
  CodePoint nextCodePoint() override { return codePointAt(m_position++); }
  CodePoint previousCodePoint() override { return codePointAt(--m_position); }
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
    return m_layout->child(index);
  }
  CodePoint codePointAt(size_t index) const {
    if (index == m_end) {
      return UCodePointNull;
    }
    assert(0 <= index && index < m_end);
    assert(m_layout->child(index)->isCodePointLayout());
    return CodePointLayout::GetCodePoint(m_layout->child(index));
  }

 private:
  const Tree* m_layout;
};

class CPL {
 public:
  static const CPL* FromRack(const Tree* rack, int start = 0) {
    // -> or better rack into begin() end() with CPLs
    return reinterpret_cast<const CPL*>(rack->child(start));
  }

  operator CodePoint() const {
    (void)m_content;
    const Tree* tree = reinterpret_cast<const Tree*>(this);
    if (!tree->isCodePointLayout()) {
      return UCodePointNull;
    }
    return CodePointLayout::GetCodePoint(tree);
  }

  bool operator==(const CodePoint& other) const {
    return static_cast<CodePoint>(*this) == other;
  }

 private:
  static constexpr int k_codePointLayoutSize =
      TypeBlock::NumberOfMetaBlocks(BlockType::CodePointLayout);
  char m_content[k_codePointLayoutSize];
};

/* Unlike RackLayoutDecoder that iters children of a parent rack, this points to
 * a CodePointLayout and iter its siblings until a non-codepoint layout is
 * reached. Thus the API is more similar to const char *. */
class CPLayoutDecoder : public UnicodeDecoder {
 public:
  CPLayoutDecoder(const Tree* firstCodePoint, size_t initialPosition = 0,
                  size_t layoutEnd = k_noSize)
      : UnicodeDecoder(initialPosition, layoutEnd),
        m_firstCodePoint(firstCodePoint) {
    if (m_position > m_end) {
      m_position = m_end;
    }
  }
  CPLayoutDecoder(const CPL* firstCodePoint, size_t initialPosition = 0,
                  size_t layoutEnd = k_noSize)
      : CPLayoutDecoder(reinterpret_cast<const Tree*>(firstCodePoint),
                        initialPosition, layoutEnd) {}

  CodePoint nextCodePoint() override { return codePointAt(m_position++); }
  CodePoint previousCodePoint() override { return codePointAt(--m_position); }
  CodePoint codePointAt(size_t index) const {
    if (index == m_end) {
      return UCodePointNull;
    }
    assert(0 <= index && index < m_end);
    const Tree* codePoint = m_firstCodePoint + index * k_codePointLayoutSize;
    if (!codePoint->isCodePointLayout()) {
      return UCodePointNull;
    }
    return CodePointLayout::GetCodePoint(codePoint);
  }
  const Tree* codePointLayoutAt(size_t index) {
    assert(0 <= index && index < m_end);
    return m_firstCodePoint + index * k_codePointLayoutSize;
  }

 private:
  static constexpr int k_codePointLayoutSize =
      TypeBlock::NumberOfMetaBlocks(BlockType::CodePointLayout);
  const Tree* m_firstCodePoint;
};

}  // namespace PoincareJ

#endif

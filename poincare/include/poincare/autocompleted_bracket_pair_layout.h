#ifndef POINCARE_AUTOCOMPLETED_BRACKET_PAIR_LAYOUT_H
#define POINCARE_AUTOCOMPLETED_BRACKET_PAIR_LAYOUT_H

#include <poincare/bracket_pair_layout.h>
#include <poincare/layout_cursor.h>

namespace Poincare {

class AutocompletedBracketPairLayoutNode : public BracketPairLayoutNode {
public:
  enum class Side : uint8_t {
    Left = 0,
    Right = 1,
  };

  AutocompletedBracketPairLayoutNode() : m_leftIsTemporary(false), m_rightIsTemporary(false), m_insertedAs(Side::Left) {}

  // TreeNode
  size_t size() const override { return sizeof(AutocompletedBracketPairLayoutNode); }

  // LayoutNode
  bool willAddSibling(LayoutCursor * cursor, LayoutNode * sibling, bool moveCursor) override;
  void deleteBeforeCursor(LayoutCursor * cursor) override;

  bool isTemporary(Side side) const { return side == Side::Left ? m_leftIsTemporary : m_rightIsTemporary; }
  void setTemporary(Side side, bool temporary);
  void balanceAfterInsertion(Side insertedSide, LayoutCursor * cursor);
  void makePermanent(Side side);
  void setInsertionSide(Side side) { m_insertedAs = side; }

protected:
  KDColor bracketColor(Side side, KDColor fg, KDColor bg) const { return isTemporary(side) ? KDColor::Blend(fg, bg, k_temporaryBlendAlpha) : fg; }

private:
  constexpr static uint8_t k_temporaryBlendAlpha = 0x60;

  static Side OtherSide(Side side) { return side == Side::Left ? Side::Right : Side::Left; }

  AutocompletedBracketPairLayoutNode * autocompletedParent() const;
  LayoutNode * childOnSide(Side side) const;
  bool makeTemporary(Side side, LayoutCursor * cursor);
  void absorbSiblings(Side side, LayoutCursor * cursor);
  LayoutCursor cursorAfterDeletion(Side side) const;

  bool m_leftIsTemporary : 1;
  bool m_rightIsTemporary : 1;
  Side m_insertedAs : 1;
};

}

#endif

#ifndef POINCARE_AUTOCOMPLETED_BRACKET_PAIR_LAYOUT_H
#define POINCARE_AUTOCOMPLETED_BRACKET_PAIR_LAYOUT_H

#include <poincare/bracket_pair_layout.h>
#include <poincare/layout_cursor.h>

namespace Poincare {

class AutocompletedBracketPairLayoutNode : public BracketPairLayoutNode {
public:
  enum class Side : uint8_t {
    Left = 0,
    Right,
    NumberOfSides
  };

  AutocompletedBracketPairLayoutNode() : m_status(0) {}

  // TreeNode
  size_t size() const override { return sizeof(AutocompletedBracketPairLayoutNode); }

  // LayoutNode
  bool willAddSibling(LayoutCursor * cursor, LayoutNode * sibling, bool moveCursor) override;
  void deleteBeforeCursor(LayoutCursor * cursor) override;

  bool isTemporary(Side side) const { return m_status & MaskForSide(side); }
  void balanceAfterInsertion(Side insertedSide, LayoutCursor * cursor);
  void makePermanent(Side side);
  void setInsertionSide(Side side) { m_status = (m_status & ~(1 << k_insertionSideBit)) | (static_cast<uint8_t>(side) << k_insertionSideBit); }

protected:
  KDColor bracketColor(Side side, KDColor fg, KDColor bg) const { return isTemporary(side) ? KDColor::blend(fg, bg, k_temporaryBlendAlpha) : fg; }

private:
  constexpr static uint8_t k_temporaryBlendAlpha = 0x60;
  constexpr static uint8_t k_insertionSideBit = static_cast<uint8_t>(Side::NumberOfSides);

  static uint8_t MaskForSide(Side side) { return 1 << static_cast<uint8_t>(side); }
  static Side OtherSide(Side side) { return side == Side::Left ? Side::Right : Side::Left; }

  bool makeTemporary(Side side, LayoutCursor * cursor);
  void removeIfCompletelyTemporary(LayoutCursor * cursor);
  void absorbSiblings(Side side, LayoutCursor * cursor);
  LayoutCursor cursorAfterDeletion(Side side) const;
  Side sideInsertedAs() const { return static_cast<Side>((m_status & (1 << k_insertionSideBit)) >> k_insertionSideBit); }

  /* A bit of 1 indicates the corresponding bracket is temporary i.e the
   * result of an auto-completion.
   * Third bit indicates whether the bracket was inserted as a Right or Left bracket. */
  uint8_t m_status;
};

}

#endif

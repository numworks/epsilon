#ifndef POINCARE_AUTOCOMPLETED_BRACKET_PAIR_LAYOUT_H
#define POINCARE_AUTOCOMPLETED_BRACKET_PAIR_LAYOUT_H

#include <poincare/bracket_pair_layout.h>

namespace Poincare {

class AutocompletedBracketPairLayoutNode : public BracketPairLayoutNode {
public:
  enum class Side : uint8_t {
    Left = 0,
    Right = 1,
  };

  AutocompletedBracketPairLayoutNode() : m_status(0) {}

  // TreeNode
  size_t size() const override { return sizeof(AutocompletedBracketPairLayoutNode); }

  // LayoutNode
  void deleteBeforeCursor(LayoutCursor * cursor) override;

  bool isTemporary(Side side) const { return m_status & MaskForSide(side); }
  void makeTemporary(Side side, LayoutCursor * cursor);
  // void makePermanent(Side side, int beginIndex, LayoutCursor * cursor);

protected:
  KDColor bracketColor(Side side, KDColor fg, KDColor bg) const { return isTemporary(side) ? KDColor::blend(fg, bg, k_temporaryBlendAlpha) : fg; }

private:
  constexpr static uint8_t k_temporaryBlendAlpha = 0x60;

  static uint8_t MaskForSide(Side side) { return 1 << static_cast<uint8_t>(side); }

  void removeIfCompletelyTemporary(LayoutCursor * cursor);
  void absorbSiblings(Side side, LayoutCursor * cursor);
  // void dumpChildrenInParent(Side side, int beginIndex, LayoutCursor * cursor);
  AutocompletedBracketPairLayoutNode * autocompletedParent();

  /* A bit of 1 indicates the corresponding bracket is temporary i.e the
   * result of an auto-completion. */
  uint8_t m_status;
};

}

#endif

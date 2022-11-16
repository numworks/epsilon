#ifndef POINCARE_AUTOCOMPLETED_BRACKET_PAIR_LAYOUT_H
#define POINCARE_AUTOCOMPLETED_BRACKET_PAIR_LAYOUT_H

#include <poincare/bracket_pair_layout.h>
#include <poincare/layout_cursor.h>

namespace Poincare {

class AutocompletedBracketPairLayoutNode : public BracketPairLayoutNode {
public:
  static bool IsAutoCompletedBracketPairType(Type type) { return type == LayoutNode::Type::ParenthesisLayout || type == LayoutNode::Type::CurlyBraceLayout; }

  enum class Side : uint8_t {
    Left = 0,
    Right = 1,
  };
  static Side OtherSide(Side side) { return side == Side::Left ? Side::Right : Side::Left; }

  AutocompletedBracketPairLayoutNode() : m_leftIsTemporary(false), m_rightIsTemporary(false) {}

  // TreeNode
  size_t size() const override { return sizeof(AutocompletedBracketPairLayoutNode); }

  // LayoutNode
  bool willAddSibling(LayoutCursor * cursor, Layout * sibling, bool moveCursor) override;
  void deleteBeforeCursor(LayoutCursor * cursor) override;

  bool isTemporary(Side side) const { return side == Side::Left ? m_leftIsTemporary : m_rightIsTemporary; }
  void setTemporary(Side side, bool temporary);
  // Returns Layout(this), or the bracket layout that replaced this
  Layout balanceAfterInsertion(Side insertedSide, LayoutCursor * cursor);
  void makePermanent(Side side);

protected:
  KDColor bracketColor(Side side, KDColor fg, KDColor bg) const { return isTemporary(side) ? KDColor::Blend(fg, bg, k_temporaryBlendAlpha) : fg; }

#if POINCARE_TREE_LOG
  void logAttributes(std::ostream & stream) const override {
    stream << " left=\"" << (m_leftIsTemporary ? "temporary" : "permanent") << "\"";
    stream << " right=\"" << (m_rightIsTemporary ? "temporary" : "permanent") << "\"";
  }
#endif

private:
  constexpr static uint8_t k_temporaryBlendAlpha = 0x60;


  AutocompletedBracketPairLayoutNode * autocompletedParent() const;
  LayoutNode * childOnSide(Side side) const;
  bool makeTemporary(Side side, LayoutCursor * cursor, bool force = false);
  void absorbSiblings(Side side, LayoutCursor * cursor);
  LayoutCursor cursorAfterDeletion(Side side) const;

  bool m_leftIsTemporary : 1;
  bool m_rightIsTemporary : 1;
};

}

#endif

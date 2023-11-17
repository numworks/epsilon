#ifndef POINCARE_JUNIOR_LAYOUT_AUTOCOMPLETED_PAIR_H
#define POINCARE_JUNIOR_LAYOUT_AUTOCOMPLETED_PAIR_H

#include <poincare_junior/src/memory/edition_reference.h>
#include <poincare_junior/src/memory/tree.h>

namespace PoincareJ {

enum class Side : uint8_t {
  Left = 0,
  Right = 1,
};

inline Side OtherSide(Side side) {
  return side == Side::Left ? Side::Right : Side::Left;
}

class AutocompletedPair {
 public:
  static bool IsTemporary(const Tree* node, Side side) {
    assert(node->isAutocompletedPair());
    Bits bits = node->nodeValueBlock(0)->get<Bits>();
    return side == Side::Left ? bits.m_leftIsTemporary
                              : bits.m_rightIsTemporary;
  }

  static void SetTemporary(Tree* node, Side side, bool temporary) {
    assert(node->isAutocompletedPair());
    Bits bits = node->nodeValueBlock(0)->get<Bits>();
    // TODO assignemnt ternary when clang is up to date
    if (side == Side::Left) {
      bits.m_leftIsTemporary = temporary;
    } else {
      bits.m_rightIsTemporary = temporary;
    }
    node->nodeValueBlock(0)->set<Bits>(bits);
  }

  static bool IsAutoCompletedBracketPairCodePoint(CodePoint c, TypeBlock* type,
                                                  Side* side);
  static Tree* BuildFromBracketType(TypeBlock type);

  // Deep balance the autocompleted brackets in hLayout
  static void BalanceBrackets(Tree* hLayout, EditionReference& cursorLayout,
                              int* cursorPosition);

  static void MakeChildrenPermanent(Tree* node, Side side, bool includeThis);
  static Tree* ChildOnSide(Tree* node, Side side);

 private:
  static void PrivateBalanceBrackets(TypeBlock type, Tree* hLayout,
                                     EditionReference& cursorLayout,
                                     int* cursorPosition, Tree* root);

  struct Bits {
    bool m_leftIsTemporary : 1;
    bool m_rightIsTemporary : 1;
  };
};  // namespace AutocompletedPair

}  // namespace PoincareJ

#endif

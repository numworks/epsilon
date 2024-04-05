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
    return node->nodeValueBlock(0)->getBit(side == Side::Left ? 0 : 1);
  }

  static void SetTemporary(Tree* node, Side side, bool temporary) {
    assert(node->isAutocompletedPair());
    return node->nodeValueBlock(0)->setBit(side == Side::Left ? 0 : 1,
                                           temporary);
  }

  static bool IsAutoCompletedBracketPairCodePoint(CodePoint c, TypeBlock* type,
                                                  Side* side);
  static Tree* BuildFromBracketType(TypeBlock type);

  // Deep balance the autocompleted brackets in rack
  static void BalanceBrackets(Tree* rack, TreeRef& cursorLayout,
                              int* cursorPosition);

  static void MakeChildrenPermanent(Tree* node, Side side, bool includeThis);
  static Tree* ChildOnSide(Tree* node, Side side);

 private:
  static void PrivateBalanceBrackets(TypeBlock type, Tree* hLayout,
                                     TreeRef& cursorLayout, int* cursorPosition,
                                     Tree* root);
};  // namespace AutocompletedPair

}  // namespace PoincareJ

#endif

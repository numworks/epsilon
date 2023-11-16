#ifndef POINCARE_JUNIOR_LAYOUT_AUTOCOMPLETED_PAIR_H
#define POINCARE_JUNIOR_LAYOUT_AUTOCOMPLETED_PAIR_H

#include <poincare_junior/src/memory/tree.h>

namespace PoincareJ {

enum class Side : uint8_t {
  Left = 0,
  Right = 1,
};

static Side OtherSide(Side side) {
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
    (side == Side::Left ? bits.m_leftIsTemporary : bits.m_rightIsTemporary) =
        temporary;
    node->nodeValueBlock(0)->set<Bits>(bits);
  }

 private:
  struct Bits {
    bool m_leftIsTemporary : 1;
    bool m_rightIsTemporary : 1;
  };
};  // namespace AutocompletedPair

}  // namespace PoincareJ

#endif

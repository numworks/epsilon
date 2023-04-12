#ifndef POINCARE_LAYOUT_K_CREATOR_H
#define POINCARE_LAYOUT_K_CREATOR_H

#include <poincare_junior/src/layout/code_point_layout.h>
#include <poincare_junior/src/memory/k_creator.h>

namespace PoincareJ {

// TODO : A RackLayout shouldn't have RackLayout children.
template <class... Args>
consteval auto KRackL(Args... args) {
  return KNAry<BlockType::RackLayout>(args...);
}

template <class... Args>
consteval auto KFracL(Args... args) {
  return KBinary<BlockType::FractionLayout>(args...);
}
template <class... Args>
consteval auto KVertOffL(Args... args) {
  return KUnary<BlockType::VerticalOffsetLayout>(args...);
}
template <class... Args>
consteval auto KParenthesisL(Args... args) {
  return KUnary<BlockType::ParenthesisLayout>(args...);
}

// Templating over uint32_t and not CodePoint to keep m_code private in
// CodePoint
template <uint32_t cp>
using KCodePointL = Tree<BlockType::CodePointLayout,
                         CodePointLayout::SubCodePointLayoutAtIndex(cp, 0),
                         CodePointLayout::SubCodePointLayoutAtIndex(cp, 1),
                         CodePointLayout::SubCodePointLayoutAtIndex(cp, 2),
                         CodePointLayout::SubCodePointLayoutAtIndex(cp, 3),
                         BlockType::CodePointLayout>;

template <String S,
          typename IS = decltype(std::make_index_sequence<S.size() - 1>())>
struct _RackLayoutHelper;

template <String S, std::size_t... I>
struct _RackLayoutHelper<S, std::index_sequence<I...>>
    : Concat<Tree<BlockType::RackLayout, sizeof...(I), BlockType::RackLayout>,
             KCodePointL<S[I]>...> {};

template <String S>
consteval auto operator"" _l() {
  return _RackLayoutHelper<S>();
}

}  // namespace PoincareJ

#endif

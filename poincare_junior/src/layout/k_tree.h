#ifndef POINCARE_LAYOUT_K_TREE_H
#define POINCARE_LAYOUT_K_TREE_H

#include <poincare_junior/src/layout/code_point_layout.h>
#include <poincare_junior/src/memory/k_tree.h>

namespace PoincareJ {

// TODO : A RackLayout shouldn't have RackLayout children.
constexpr auto KRackL = KNAry<BlockType::RackLayout>();
constexpr auto KFracL = KBinary<BlockType::FractionLayout>();
constexpr auto KVertOffL = KUnary<BlockType::VerticalOffsetLayout>();
constexpr auto KSqrtL = KUnary<BlockType::SquareRootLayout>();
constexpr auto KParenthesisL = KUnary<BlockType::ParenthesisLayout>();

// Templating over uint32_t and not CodePoint to keep m_code private in
// CodePoint
template <uint32_t cp>
using KCodePointL = KTree<BlockType::CodePointLayout,
                          CodePointLayout::SubCodePointLayoutAtIndex(cp, 0),
                          CodePointLayout::SubCodePointLayoutAtIndex(cp, 1),
                          CodePointLayout::SubCodePointLayoutAtIndex(cp, 2),
                          CodePointLayout::SubCodePointLayoutAtIndex(cp, 3)>;

template <String S,
          typename IS =
              decltype(std::make_index_sequence<S.codePointSize() - 1>())>
struct _RackLayoutHelper;

template <String S, std::size_t... I>
struct _RackLayoutHelper<S, std::index_sequence<I...>>
    : Concat<KTree<BlockType::RackLayout, sizeof...(I)>,
             KCodePointL<S.codePointAt(I)>...> {};

template <String S>
consteval auto operator"" _l() {
  return _RackLayoutHelper<S>();
}

}  // namespace PoincareJ

#endif

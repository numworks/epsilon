#ifndef POINCARE_LAYOUT_K_TREE_H
#define POINCARE_LAYOUT_K_TREE_H

#include <poincare_junior/src/layout/code_point_layout.h>
#include <poincare_junior/src/memory/k_tree.h>

namespace PoincareJ {

// TODO: A RackLayout shouldn't have RackLayout children.
constexpr auto KRackL = KNAry16<BlockType::RackLayout>();
constexpr auto KOperatorSeparatorL =
    KTree<BlockType::OperatorSeparatorLayout>();
constexpr auto KThousandSeparatorL =
    KTree<BlockType::ThousandSeparatorLayout>();
constexpr auto KFracL = KBinary<BlockType::FractionLayout>();
constexpr auto KSqrtL = KUnary<BlockType::SquareRootLayout>();

constexpr auto KParenthesisL = KUnary<BlockType::ParenthesisLayout, 0>();
constexpr auto KDerivativeL = KUnary<BlockType::DerivativeLayout, 0>();
constexpr auto KNthDerivativeL = KUnary<BlockType::NthDerivativeLayout, 0>();

constexpr auto KSuperscriptL = KUnary<BlockType::VerticalOffsetLayout, 0>();
constexpr auto KSubscriptL = KUnary<BlockType::VerticalOffsetLayout, 1>();
constexpr auto KPrefixSuperscriptL =
    KUnary<BlockType::VerticalOffsetLayout, 2>();
constexpr auto KPrefixSubscriptL = KUnary<BlockType::VerticalOffsetLayout, 3>();

constexpr auto KEmptyMatrixL =
    KTree<BlockType::MatrixLayout, 2, 2, BlockType::RackLayout, 0, 0,
          BlockType::RackLayout, 0, 0, BlockType::RackLayout, 0, 0,
          BlockType::RackLayout, 0, 0>();

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
    : Concat<
          KTree<BlockType::RackLayout, sizeof...(I) % 256, sizeof...(I) / 256>,
          KCodePointL<S.codePointAt(I)>...> {};

template <String S>
consteval auto operator"" _l() {
  return _RackLayoutHelper<S>();
}

// Unfortunately template operator'' does not exist, we must use strings instead
template <String S>
  requires(S.codePointSize() == 2)
consteval auto operator"" _cl() {
  return KCodePointL<S.codePointAt(0)>();
}

}  // namespace PoincareJ

#endif

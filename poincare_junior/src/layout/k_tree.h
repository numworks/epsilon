#ifndef POINCARE_LAYOUT_K_TREE_H
#define POINCARE_LAYOUT_K_TREE_H

#include <ion/unicode/code_point.h>
#include <omgpj/bit.h>
#include <poincare_junior/src/memory/k_tree.h>

namespace PoincareJ {
namespace KTrees {

// TODO: A RackLayout shouldn't have RackLayout children.
constexpr auto KRackL = KNAry16<BlockType::RackLayout>();
constexpr auto KOperatorSeparatorL =
    KTree<BlockType::OperatorSeparatorLayout>();
constexpr auto KThousandSeparatorL =
    KTree<BlockType::ThousandSeparatorLayout>();
constexpr auto KFracL = KBinary<BlockType::FractionLayout>();
constexpr auto KSqrtL = KUnary<BlockType::SquareRootLayout>();
constexpr auto KNthSqrtL = KBinary<BlockType::NthRootLayout>();

constexpr auto KParenthesisL = KUnary<BlockType::ParenthesisLayout, 0>();
constexpr auto KCurlyBracesL = KUnary<BlockType::CurlyBraceLayout, 0>();
constexpr auto KAbsL = KUnary<BlockType::AbsoluteValueLayout>();

constexpr auto KDerivativeL = KUnary<BlockType::DerivativeLayout, 0>();
constexpr auto KNthDerivativeL = KUnary<BlockType::NthDerivativeLayout, 0>();
constexpr auto KIntegralL = KFixedArity<4, BlockType::IntegralLayout>();
constexpr auto KSumL = KFixedArity<4, BlockType::SumLayout>();
constexpr auto KProductL = KFixedArity<4, BlockType::ProductLayout>();
constexpr auto KCondensedSumL = KFixedArity<3, BlockType::CondensedSumLayout>();

constexpr auto KSuperscriptL = KUnary<BlockType::VerticalOffsetLayout, 0>();
constexpr auto KSubscriptL = KUnary<BlockType::VerticalOffsetLayout, 1>();
constexpr auto KPrefixSuperscriptL =
    KUnary<BlockType::VerticalOffsetLayout, 2>();
constexpr auto KPrefixSubscriptL = KUnary<BlockType::VerticalOffsetLayout, 3>();

constexpr auto KEmptyMatrixL =
    KTree<BlockType::MatrixLayout, 2, 2, BlockType::RackLayout, 0, 0,
          BlockType::RackLayout, 0, 0, BlockType::RackLayout, 0, 0,
          BlockType::RackLayout, 0, 0>();

constexpr auto KPoint2DL = KBinary<BlockType::Point2DLayout>();

constexpr uint8_t SubCodePointLayoutAtIndex(CodePoint value, int index) {
  return Bit::getByteAtIndex(value, index);
}

// Templating over uint32_t and not CodePoint to keep m_code private in
// CodePoint
template <uint32_t cp>
using KCodePointL =
    KTree<BlockType::CodePointLayout, SubCodePointLayoutAtIndex(cp, 0),
          SubCodePointLayoutAtIndex(cp, 1), SubCodePointLayoutAtIndex(cp, 2),
          SubCodePointLayoutAtIndex(cp, 3)>;

template <uint32_t cp, uint32_t cc>
using KCombinedCodePointL =
    KTree<BlockType::CombinedCodePointsLayout, SubCodePointLayoutAtIndex(cp, 0),
          SubCodePointLayoutAtIndex(cp, 1), SubCodePointLayoutAtIndex(cp, 2),
          SubCodePointLayoutAtIndex(cp, 3), SubCodePointLayoutAtIndex(cc, 0),
          SubCodePointLayoutAtIndex(cc, 1), SubCodePointLayoutAtIndex(cc, 2),
          SubCodePointLayoutAtIndex(cc, 3)>;

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

// Rack concatenation operator ^

// rack ^ rack
template <Block N1, Block... B1, Block N2, Block... B2>
consteval auto operator^(KTree<BlockType::RackLayout, N1, 0, B1...>,
                         KTree<BlockType::RackLayout, N2, 0, B2...>) {
  static_assert(static_cast<uint8_t>(N1) + static_cast<uint8_t>(N2) < 256);
  return KTree<BlockType::RackLayout,
               Block(static_cast<uint8_t>(N1) + static_cast<uint8_t>(N2)), 0,
               B1..., B2...>();
}

// rack ^ layout
template <Block N1, Block... B1, Block T2, Block... B2>
  requires(BlockType(uint8_t(T2)) != BlockType::RackLayout)
consteval auto operator^(KTree<BlockType::RackLayout, N1, 0, B1...>,
                         KTree<T2, B2...>) {
  static_assert(static_cast<uint8_t>(N1) < 255);
  return KTree<BlockType::RackLayout, Block(static_cast<uint8_t>(N1) + 1), 0,
               B1..., T2, B2...>();
}

// layout ^ rack
template <Block T1, Block... B1, Block N2, Block... B2>
  requires(BlockType(uint8_t(T1)) != BlockType::RackLayout)
consteval auto operator^(KTree<T1, B1...>,
                         KTree<BlockType::RackLayout, N2, 0, B2...>) {
  static_assert(static_cast<uint8_t>(N2) < 255);
  return KTree<BlockType::RackLayout, Block(static_cast<uint8_t>(N2) + 1), 0,
               T1, B1..., B2...>();
}

// layout ^ layout
template <Block T1, Block... B1, Block T2, Block... B2>
  requires(BlockType(uint8_t(T1)) != BlockType::RackLayout &&
           BlockType(uint8_t(T2)) != BlockType::RackLayout)
consteval auto operator^(KTree<T1, B1...>, KTree<T2, B2...>) {
  return KTree<BlockType::RackLayout, 2, 0, T1, B1..., T2, B2...>();
}

}  // namespace KTrees
}  // namespace PoincareJ

#endif

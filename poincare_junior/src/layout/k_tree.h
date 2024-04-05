#ifndef POINCARE_LAYOUT_K_TREE_H
#define POINCARE_LAYOUT_K_TREE_H

#include <ion/unicode/code_point.h>
#include <omgpj/bit.h>
#include <poincare_junior/src/memory/k_tree.h>

namespace PoincareJ {
namespace KTrees {

// TODO: A RackLayout shouldn't have RackLayout children.
constexpr auto KRackL = KNAry16<Type::RackLayout>();
constexpr auto KOperatorSeparatorL = KTree<Type::OperatorSeparatorLayout>();
constexpr auto KThousandSeparatorL = KTree<Type::ThousandSeparatorLayout>();
constexpr auto KFracL = KBinary<Type::FractionLayout>();
constexpr auto KSqrtL = KUnary<Type::SquareRootLayout>();
constexpr auto KNthSqrtL = KBinary<Type::NthRootLayout>();

constexpr auto KParenthesisL = KUnary<Type::ParenthesisLayout, 0>();
constexpr auto KCurlyBracesL = KUnary<Type::CurlyBraceLayout, 0>();
constexpr auto KAbsL = KUnary<Type::AbsoluteValueLayout>();

constexpr auto KDerivativeL = KUnary<Type::DerivativeLayout, 0>();
constexpr auto KNthDerivativeL = KUnary<Type::NthDerivativeLayout, 0>();
constexpr auto KIntegralL = KFixedArity<4, Type::IntegralLayout>();
constexpr auto KSumL = KFixedArity<4, Type::SumLayout>();
constexpr auto KProductL = KFixedArity<4, Type::ProductLayout>();
constexpr auto KCondensedSumL = KFixedArity<3, Type::CondensedSumLayout>();

constexpr auto KSuperscriptL = KUnary<Type::VerticalOffsetLayout, 0>();
constexpr auto KSubscriptL = KUnary<Type::VerticalOffsetLayout, 1>();
constexpr auto KPrefixSuperscriptL = KUnary<Type::VerticalOffsetLayout, 2>();
constexpr auto KPrefixSubscriptL = KUnary<Type::VerticalOffsetLayout, 3>();

constexpr auto KEmptyMatrixL =
    KTree<Type::MatrixLayout, 2, 2, Type::RackLayout, 0, 0, Type::RackLayout, 0,
          0, Type::RackLayout, 0, 0, Type::RackLayout, 0, 0>();

constexpr auto KPoint2DL = KBinary<Type::Point2DLayout>();

// Templating over uint32_t and not CodePoint to keep m_code private in
// CodePoint

template <uint32_t cp>
struct KCodePointL;

template <uint32_t cp>
  requires(cp < 128)
struct KCodePointL<cp> : KTree<Type::AsciiCodePointLayout, cp> {};

template <uint32_t cp>
  requires(cp >= 128)
struct KCodePointL<cp>
    : KTree<Type::UnicodeCodePointLayout, Bit::getByteAtIndex(cp, 0),
            Bit::getByteAtIndex(cp, 1), Bit::getByteAtIndex(cp, 2),
            Bit::getByteAtIndex(cp, 3)> {};

template <uint32_t cp, uint32_t cc>
using KCombinedCodePointL =
    KTree<Type::CombinedCodePointsLayout, Bit::getByteAtIndex(cp, 0),
          Bit::getByteAtIndex(cp, 1), Bit::getByteAtIndex(cp, 2),
          Bit::getByteAtIndex(cp, 3), Bit::getByteAtIndex(cc, 0),
          Bit::getByteAtIndex(cc, 1), Bit::getByteAtIndex(cc, 2),
          Bit::getByteAtIndex(cc, 3)>;

template <String S,
          typename IS =
              decltype(std::make_index_sequence<S.codePointSize() - 1>())>
struct _RackLayoutHelper;

template <String S, std::size_t... I>
struct _RackLayoutHelper<S, std::index_sequence<I...>>
    : Concat<KTree<Type::RackLayout, sizeof...(I) % 256, sizeof...(I) / 256>,
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
consteval auto operator^(KTree<Type::RackLayout, N1, 0, B1...>,
                         KTree<Type::RackLayout, N2, 0, B2...>) {
  static_assert(static_cast<uint8_t>(N1) + static_cast<uint8_t>(N2) < 256);
  return KTree<Type::RackLayout,
               Block(static_cast<uint8_t>(N1) + static_cast<uint8_t>(N2)), 0,
               B1..., B2...>();
}

// rack ^ layout
template <Block N1, Block... B1, Block T2, Block... B2>
  requires(Type(uint8_t(T2)) != Type::RackLayout)
consteval auto operator^(KTree<Type::RackLayout, N1, 0, B1...>,
                         KTree<T2, B2...>) {
  static_assert(static_cast<uint8_t>(N1) < 255);
  return KTree<Type::RackLayout, Block(static_cast<uint8_t>(N1) + 1), 0, B1...,
               T2, B2...>();
}

// layout ^ rack
template <Block T1, Block... B1, Block N2, Block... B2>
  requires(Type(uint8_t(T1)) != Type::RackLayout)
consteval auto operator^(KTree<T1, B1...>,
                         KTree<Type::RackLayout, N2, 0, B2...>) {
  static_assert(static_cast<uint8_t>(N2) < 255);
  return KTree<Type::RackLayout, Block(static_cast<uint8_t>(N2) + 1), 0, T1,
               B1..., B2...>();
}

// layout ^ layout
template <Block T1, Block... B1, Block T2, Block... B2>
  requires(Type(uint8_t(T1)) != Type::RackLayout &&
           Type(uint8_t(T2)) != Type::RackLayout)
consteval auto operator^(KTree<T1, B1...>, KTree<T2, B2...>) {
  return KTree<Type::RackLayout, 2, 0, T1, B1..., T2, B2...>();
}

}  // namespace KTrees
}  // namespace PoincareJ

#endif

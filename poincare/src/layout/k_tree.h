#ifndef POINCARE_LAYOUT_K_TREE_H
#define POINCARE_LAYOUT_K_TREE_H

#include <omg/bit_helper.h>
#include <omg/code_point.h>
#include <poincare/src/memory/k_tree.h>

namespace Poincare::Internal {
namespace KTrees {

// TODO: A RackLayout shouldn't have RackLayout children.
constexpr auto KRackL = KNAry16<Type::RackLayout>();

constexpr auto KSuperscriptL = KUnary<Type::VerticalOffsetLayout, 0>();
constexpr auto KSubscriptL = KUnary<Type::VerticalOffsetLayout, 1>();
constexpr auto KPrefixSuperscriptL = KUnary<Type::VerticalOffsetLayout, 2>();
constexpr auto KPrefixSubscriptL = KUnary<Type::VerticalOffsetLayout, 3>();

constexpr auto KOperatorSeparatorL = KTree<Type::OperatorSeparatorLayout>();
constexpr auto KUnitSeparatorL = KTree<Type::UnitSeparatorLayout>();
constexpr auto KThousandSeparatorL = KTree<Type::ThousandSeparatorLayout>();

constexpr auto KAbsL = KUnary<Type::AbsLayout>();
constexpr auto KCeilL = KUnary<Type::CeilLayout>();
constexpr auto KFloorL = KUnary<Type::FloorLayout>();
constexpr auto KVectorNormL = KUnary<Type::VectorNormLayout>();

constexpr auto KParenthesesL = KUnary<Type::ParenthesesLayout, 0>();
constexpr auto KParenthesesLeftTempL = KUnary<Type::ParenthesesLayout, 1>();
constexpr auto KParenthesesRightTempL = KUnary<Type::ParenthesesLayout, 2>();
constexpr auto KParenthesesTempL = KUnary<Type::ParenthesesLayout, 3>();
constexpr auto KCurlyBracesL = KUnary<Type::CurlyBracesLayout, 0>();
constexpr auto KCurlyBracesLeftTempL = KUnary<Type::CurlyBracesLayout, 1>();
constexpr auto KCurlyBracesRightTempL = KUnary<Type::CurlyBracesLayout, 2>();
constexpr auto KCurlyBracesTempL = KUnary<Type::CurlyBracesLayout, 3>();

constexpr auto KCondensedSumL = KFixedArity<3, Type::CondensedSumLayout>();
constexpr auto KDiffL = KFixedArity<4, Type::DiffLayout, 0, 0>();
constexpr auto KNthDiffL = KFixedArity<4, Type::DiffLayout, 0, 0xFF>();
constexpr auto KIntegralL = KFixedArity<4, Type::IntegralLayout>();
constexpr auto KProductL = KFixedArity<4, Type::ProductLayout>();
constexpr auto KSumL = KFixedArity<4, Type::SumLayout>();
constexpr auto KListSequenceL = KFixedArity<3, Type::ListSequenceLayout>();

constexpr auto KFracL = KBinary<Type::FractionLayout>();
constexpr auto KPoint2DL = KBinary<Type::Point2DLayout>();
constexpr auto KBinomialL = KBinary<Type::BinomialLayout>();
constexpr auto KPtBinomialL = KBinary<Type::PtBinomialLayout>();
constexpr auto KPtPermuteL = KBinary<Type::PtPermuteLayout>();

constexpr auto KEmptyMatrixL =
    KTree<Type::MatrixLayout, 2, 2, Type::RackLayout, 0, 0, Type::RackLayout, 0,
          0, Type::RackLayout, 0, 0, Type::RackLayout, 0, 0>();

template <KTreeConcept A>
constexpr auto KMatrix1x1L(A) {
  return Concat<KTree<Type::MatrixLayout, 2, 2>, A,
                KTree<Type::RackLayout, 0, 0, Type::RackLayout, 0, 0,
                      Type::RackLayout, 0, 0, Type::RackLayout, 0, 0>>();
}

template <KTreeConcept A, KTreeConcept B, KTreeConcept C, KTreeConcept D>
constexpr auto KMatrix2x2L(A, B, C, D) {
  return Concat<
      KTree<Type::MatrixLayout, 3, 3>, A, B, KTree<Type::RackLayout, 0, 0>, C,
      D,
      KTree<Type::RackLayout, 0, 0, Type::RackLayout, 0, 0, Type::RackLayout, 0,
            0, Type::RackLayout, 0, 0, Type::RackLayout, 0, 0>>();
}

constexpr auto KConjL = KUnary<Type::ConjLayout>();
constexpr auto KSqrtL = KUnary<Type::SqrtLayout>();
constexpr auto KRootL = KBinary<Type::RootLayout>();

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
    : KTree<Type::UnicodeCodePointLayout, OMG::BitHelper::getByteAtIndex(cp, 0),
            OMG::BitHelper::getByteAtIndex(cp, 1),
            OMG::BitHelper::getByteAtIndex(cp, 2),
            OMG::BitHelper::getByteAtIndex(cp, 3)> {};

template <uint32_t cp, uint32_t cc>
using KCombinedCodePointL =
    KTree<Type::CombinedCodePointsLayout, OMG::BitHelper::getByteAtIndex(cp, 0),
          OMG::BitHelper::getByteAtIndex(cp, 1),
          OMG::BitHelper::getByteAtIndex(cp, 2),
          OMG::BitHelper::getByteAtIndex(cp, 3),
          OMG::BitHelper::getByteAtIndex(cc, 0),
          OMG::BitHelper::getByteAtIndex(cc, 1),
          OMG::BitHelper::getByteAtIndex(cc, 2),
          OMG::BitHelper::getByteAtIndex(cc, 3)>;

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
}  // namespace Poincare::Internal

#endif

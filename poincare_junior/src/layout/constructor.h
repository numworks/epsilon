#ifndef POINCARE_LAYOUT_CONSTRUCTOR_H
#define POINCARE_LAYOUT_CONSTRUCTOR_H

#include <poincare_junior/src/memory/tree_constructor.h>
#include <poincare_junior/src/layout/code_point_layout.h>

namespace PoincareJ {

// TODO : A RackLayout shouldn't have RackLayout children.
template <class...Args> consteval auto RackL(Args...args) { return NAry<BlockType::RackLayout>(args...); }

template <class...Args> consteval auto FracL(Args...args) { return Binary<BlockType::FractionLayout>(args...); }
template <class...Args> consteval auto VertOffL(Args...args) { return Unary<BlockType::VerticalOffsetLayout>(args...); }
template <class...Args> consteval auto ParenthesisL(Args...args) { return Unary<BlockType::ParenthesisLayout>(args...); }

// Templating over uint32_t and not CodePoint to keep m_code private in CodePoint
template <uint32_t cp> using CodePointL = Tree<BlockType::CodePointLayout, CodePointLayout::SubCodePointLayoutAtIndex(cp, 0), CodePointLayout::SubCodePointLayoutAtIndex(cp, 1), CodePointLayout::SubCodePointLayoutAtIndex(cp, 2), CodePointLayout::SubCodePointLayoutAtIndex(cp, 3), BlockType::CodePointLayout>;

template <String S, typename IS = decltype(std::make_index_sequence<S.size() - 1>())> struct _RackLayoutHelper;

template <String S, std::size_t... I>
struct _RackLayoutHelper<S, std::index_sequence<I...>> : Concat<Tree<BlockType::RackLayout, sizeof...(I), BlockType::RackLayout>, CodePointL<S[I]>...> {};

template <String S> consteval auto operator"" _l () { return _RackLayoutHelper<S>(); }

}

#endif

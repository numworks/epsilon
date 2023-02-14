#ifndef POINCARE_MEMORY_TREE_CONSTRUCTOR_H
#define POINCARE_MEMORY_TREE_CONSTRUCTOR_H

#include <array>
#include "node_constructor.h"
#include "node.h"
#include <poincare_junior/src/expression/integer.h>
#include <omgpj/assert.h>
#include <omgpj/concept.h>
#include <omg/print.h>
#include <omgpj/print.h>

namespace PoincareJ {

// https://stackoverflow.com/questions/40920149/is-it-possible-to-create-templated-user-defined-literals-literal-suffixes-for
// https://akrzemi1.wordpress.com/2012/10/29/user-defined-literals-part-iii/

/* These two abstract classes and their associated concepts are here to allow
 * templated functions using CTree to be called with any CTreeCompatible which
 * then casted to CTree and its template arguments deduced. */

class AbstractCTreeCompatible {};

template <class C> concept CTreeCompatibleConcept = Concept::is_derived_from<C, AbstractCTreeCompatible>;

class AbstractCTree : AbstractCTreeCompatible {};

template <class C> concept CTreeConcept = Concept::is_derived_from<C, AbstractCTree>;


/* The CTree template class is the compile time representation of a constexpr
 * tree. It's complete block representation is specified as template parameters
 * in order to be able to use the address of the static singleton (in flash) as
 * a Node. It also eliminated identical trees since their are all using the same
 * specialized function.
 */

template <Block... Blocks>
class CTree : public AbstractCTree {
public:
  static constexpr Block k_blocks[] = { Blocks... };
  static constexpr size_t k_size = sizeof...(Blocks);
  constexpr operator Node () const { return k_blocks; }
};


/* Helper to concatenate CTrees */

/* Usage:
 * template <Block Tag, CTreeConcept CT1, CTreeConcept CT2> consteval auto Binary(CT1, CT2) {
 *   return Concat<CTree<Tag>, CT1, CT2>();
 * }
 */

template <size_t N1, const Block B1[N1], size_t N2, const Block B2[N2], typename IS = decltype(std::make_index_sequence<N1 + N2>())> struct __BlockConcat;

template <size_t N1, const Block B1[N1], size_t N2, const Block B2[N2], std::size_t... I>
struct __BlockConcat<N1, B1, N2, B2, std::index_sequence<I...>> {
  using ctree = CTree<((I < N1) ? B1[I] : B2[I - N1])...>;
};

template <CTreeConcept CT1, CTreeConcept CT2> using __ConcatTwo = typename __BlockConcat<CT1::k_size, CT1::k_blocks, CT2::k_size, CT2::k_blocks>::ctree;

template <CTreeConcept CT1, CTreeConcept... CT> struct Concat;
template <CTreeConcept CT1> struct Concat<CT1> : CT1 {};
template <CTreeConcept CT1, CTreeConcept... CT> struct Concat : __ConcatTwo<CT1, Concat<CT...>> {};


// Helpers

template <Block Tag, Block... B1> consteval auto Unary(CTree<B1...>) {
  return CTree<Tag, B1...>();
}

template <Block Tag, CTreeCompatibleConcept A> consteval auto Unary(A a) {
  return Unary<Tag>(CTree(a));
}

template <Block Tag, Block... B1, Block... B2> consteval auto Binary(CTree<B1...>, CTree<B2...>) {
  return CTree<Tag, B1..., B2...>();
}

template <Block Tag, CTreeCompatibleConcept A, CTreeCompatibleConcept B> consteval auto Binary(A a, B b) {
  return Binary<Tag>(CTree(a), CTree(b));
}

template<Block Tag, CTreeConcept ...CTS> requires (sizeof...(CTS)>=2) static consteval auto __NAry(CTS...) {
  return Concat<CTree<Tag, sizeof...(CTS), Tag>, CTS...>();
}

template <Block Tag, CTreeCompatibleConcept ...CTS> consteval auto NAry(CTS... args) { return __NAry<Tag>(CTree(args)...); }


// Constructors

template <class...Args> consteval auto Fact(Args...args) { return Unary<BlockType::Factorial>(args...); }

template <class...Args> consteval auto Div(Args...args) { return Binary<BlockType::Division>(args...); }

template <class...Args> consteval auto Sub(Args...args) { return Binary<BlockType::Subtraction>(args...); }

template <class...Args> consteval auto Pow(Args...args) { return Binary<BlockType::Power>(args...); }

template <class...Args> consteval auto Add(Args...args) { return NAry<BlockType::Addition>(args...); }

template <class...Args> consteval auto Mult(Args...args) { return NAry<BlockType::Multiplication>(args...); }

template <class...Args> consteval auto Set(Args...args) { return NAry<BlockType::Set>(args...); }


// Alias only for readability
template <uint8_t ... Values> using Exponents = CTree<Values...>;


/* The first function is responsible of building the actual representation from
 * CTrees while the other one is just here to allow the function to take
 * CTreeCompatible arguments like integer litterals. */

template<CTreeConcept Exp, CTreeConcept ...CTS> static consteval auto __Pol(Exp exponents, CTS...) {
  constexpr uint8_t Size = sizeof...(CTS);
  return Concat<CTree<BlockType::Polynomial, Size>, Exp, CTree<Size, BlockType::Polynomial>, CTS...>();
}

template<CTreeConcept Exp, CTreeCompatibleConcept ...CTS> static consteval auto Pol(Exp exponents, CTS... args) {
  constexpr uint8_t Size = sizeof...(CTS);
  static_assert(Exp::k_size == Size - 1, "Number of children and exponents do not match in constant polynomial");
  return __Pol(exponents, CTree(args)...);
}


#if 0

template <Block Tag, CTreeCompatible A, CTreeCompatible B> consteval auto NAryOperator(A a, B b) { return NAryOperator<Tag>(CTree(a), CTree(b)); }

template <Block Tag, Block... B1, Block... B2> consteval auto NAryOperator(CTree<B1...>, CTree<B2...>) {
  return CTree<Tag, 2, Tag, B1..., B2...>();
}

template <Block Tag, Block N1, Block... B1, Block... B2> consteval auto NAryOperator(CTree<Tag, N1, Tag, B1...>, CTree<B2...>) {
  return CTree<Tag, static_cast<uint8_t>(N1) + 1, Tag, B1..., B2...>();
}

template <class...Args> consteval auto operator-(Args...args) { return Binary<BlockType::Subtraction>(args...); }

template <class...Args> consteval auto operator/(Args...args) { return Binary<BlockType::Subtraction>(args...); }

template <class...Args> consteval auto operator+(Args...args) { return NAryOperator<BlockType::Addition>(args...); }

template <class...Args> consteval auto operator*(Args...args) { return NAryOperator<BlockType::Multiplication>(args...); }

// The nice syntax above doesn't work with GCC yet and has to be expanded

template <Block... B1, Block... B2> consteval auto operator-(CTree<B1...>, CTree<B2...>) { return CTree<BlockType::Subtraction, B1..., B2...>(); }

template <CTreeCompatible A, CTreeCompatible B> consteval auto operator-(A a, B b) { return CTree(a) - CTree(b); }

template <Block... B1, Block... B2> consteval auto operator/(CTree<B1...>, CTree<B2...>) { return CTree<BlockType::Division, B1..., B2...>(); }

template <CTreeCompatible A, CTreeCompatible B> consteval auto operator/(A a, B b) { return CTree(a) / CTree(b); }

template <Block... B1, Block... B2> consteval auto operator+(CTree<B1...>, CTree<B2...>) { return CTree<BlockType::Addition, 2, BlockType::Addition, B1..., B2...>(); }

template <CTreeCompatible A, CTreeCompatible B> consteval auto operator+(A a, B b) { return CTree(a) + CTree(b); }

template <Block N1, Block... B1, Block... B2> consteval auto operator+(CTree<BlockType::Addition, N1, BlockType::Addition, B1...>, CTree<B2...>) {
  return CTree<BlockType::Addition, static_cast<uint8_t>(N1) + 1, BlockType::Addition, B1..., B2...>();
}

template <Block... B1, Block... B2> consteval auto operator*(CTree<B1...>, CTree<B2...>) { return CTree<BlockType::Multiplication, 2, BlockType::Multiplication, B1..., B2...>(); }

template <Block N1, Block... B1, Block... B2> consteval auto operator*(CTree<BlockType::Multiplication, N1, BlockType::Multiplication, B1...>, CTree<B2...>) {
  return CTree<BlockType::Multiplication, static_cast<uint8_t>(N1) + 1, BlockType::Multiplication, B1..., B2...>();
}

template <CTreeCompatible A, CTreeCompatible B> consteval auto operator*(A a, B b) { return CTree(a) * CTree(b); }

#endif


/* Immediates are used to represent numerical constants of the code (like 2_e)
 * temporarily before they are cast to CTrees, this allows writing -2_e. */

template <int V> class IntegerLitteral : public AbstractCTreeCompatible {
public:
  // once a deduction guide as required a given CTree from the immediate, build it
  template <Block...B> consteval operator CTree<B...> () { return CTree<B...>(); }

  constexpr operator const Node () { return CTree(IntegerLitteral<V>()); }

  consteval IntegerLitteral<-V> operator-() { return IntegerLitteral<-V>(); }
  // Note : we could decide to implement constant propagation operators here
};

// template <int8_t V> using Inti = CTree<BlockType::IntegerShort, V, BlockType::IntegerShort>;
// template <int8_t V> CTree(Immediate<V>)->Inti<V>; // only GCC accepts this one

// Deduction guides to create the smallest CTree that can represent the Immediate

CTree(IntegerLitteral<-1>)->CTree<BlockType::MinusOne>;
CTree(IntegerLitteral<0>)->CTree<BlockType::Zero>;
CTree(IntegerLitteral<1>)->CTree<BlockType::One>;
CTree(IntegerLitteral<2>)->CTree<BlockType::Two>;

template <int V> requires (V >= INT8_MIN && V <= INT8_MAX) CTree(IntegerLitteral<V>) -> CTree<BlockType::IntegerShort, V, BlockType::IntegerShort>;

template <int V> requires (V > 0 && Integer::NumberOfDigits(V) == 2) CTree(IntegerLitteral<V>) -> CTree<BlockType::IntegerPosBig, 2, Bit::getByteAtIndex(V, 0), Bit::getByteAtIndex(V, 1), 2, BlockType::IntegerPosBig>;

template <int V> requires (V > 0 && Integer::NumberOfDigits(V) == 3) CTree(IntegerLitteral<V>) -> CTree<BlockType::IntegerPosBig, 3, Bit::getByteAtIndex(V, 0), Bit::getByteAtIndex(V, 1), Bit::getByteAtIndex(V, 2), 3, BlockType::IntegerPosBig>;

template <int V> requires (V > 0 && Integer::NumberOfDigits(V) == 4) CTree(IntegerLitteral<V>) -> CTree<BlockType::IntegerPosBig, 4, Bit::getByteAtIndex(V, 0), Bit::getByteAtIndex(V, 1), Bit::getByteAtIndex(V, 2), Bit::getByteAtIndex(V, 3), 4, BlockType::IntegerPosBig>;


template <int V> requires (V < 0 && Integer::NumberOfDigits(-V) == 2) CTree(IntegerLitteral<V>) -> CTree<BlockType::IntegerNegBig, 2, Bit::getByteAtIndex(-V, 0), Bit::getByteAtIndex(-V, 1), 2, BlockType::IntegerNegBig>;

template <int V> requires (V < 0 && Integer::NumberOfDigits(-V) == 3) CTree(IntegerLitteral<V>) -> CTree<BlockType::IntegerNegBig, 3, Bit::getByteAtIndex(-V, 0), Bit::getByteAtIndex(-V, 1), Bit::getByteAtIndex(-V, 2), 3, BlockType::IntegerNegBig>;

template <int V> requires (V < 0 && Integer::NumberOfDigits(-V) == 4) CTree(IntegerLitteral<V>) -> CTree<BlockType::IntegerNegBig, 4, Bit::getByteAtIndex(-V, 0), Bit::getByteAtIndex(-V, 1), Bit::getByteAtIndex(-V, 2), Bit::getByteAtIndex(-V, 3), 4, BlockType::IntegerNegBig>;


// TODO: move in OMG?
constexpr static uint64_t Value(const char * str, size_t size) {
  uint64_t value = 0;
  for (int i = 0; i < size - 1; i++) {
    uint8_t digit = OMG::Print::DigitForCharacter(str[i]);
    // No overflow
    constexpr_assert(value <= (UINT64_MAX - digit)/10);
    value = 10 * value + digit;
  }
  return value;
}

template <char...C>
consteval auto operator"" _e () {
  constexpr const char value[] = { C... , '\0' };
  constexpr int V = Value(value, sizeof...(C) + 1);
  return IntegerLitteral<V>();
}




template<size_t N>
struct String {
  char m_data[N];
  constexpr size_t size() const { return N; }
  template <std::size_t... Is>
  constexpr String(const char (&arr)[N], std::integer_sequence<std::size_t, Is...>) : m_data{arr[Is]...} {}
  constexpr String(char const(&arr)[N]) : String(arr, std::make_integer_sequence<std::size_t, N>()) {}
  constexpr const char & operator[](std::size_t i) const { return m_data[i]; }
};

// specialized from https://stackoverflow.com/questions/60434033/how-do-i-expand-a-compile-time-stdarray-into-a-parameter-pack/60440611#60440611

template <String S, typename IS = decltype(std::make_index_sequence<S.size() - 1>())> struct Variable;

template <String S, std::size_t... I>
struct Variable<S, std::index_sequence<I...>> {
  using ctree = CTree<BlockType::UserSymbol, sizeof...(I), S[I]..., sizeof...(I), BlockType::UserSymbol>;
};

template <String S>
consteval auto operator"" _e () {
  return typename Variable<S>::ctree();
}

// TODO : A RackLayout shouldn't have RackLayout children.
template<unsigned ...Len> static consteval auto RackL(const Tree<Len> (&...children)) { return MakeTree<BlockType::RackLayout>(children...); }
template<unsigned L1, unsigned L2> static consteval Tree<L1+L2+1> FracL(const Tree<L1> child1, const Tree<L2> child2) { return MakeTree<BlockType::FractionLayout>(child1, child2); }
template<unsigned L1> static consteval Tree<L1+1> VertOffL(const Tree<L1> child1) { return MakeTree<BlockType::VerticalOffsetLayout>(child1); }
template<unsigned L1> static consteval Tree<L1+1> ParenthesisL(const Tree<L1> child1) { return MakeTree<BlockType::ParenthesisLayout>(child1); }

template <String S>
consteval auto operator"" _l () {
  constexpr int size = S.size()-1;
  constexpr int codePointLayoutMetaSize = TypeBlock::NumberOfMetaBlocks(BlockType::CodePointLayout);
  constexpr int rackLayoutMetaSize = TypeBlock::NumberOfMetaBlocks(BlockType::RackLayout);
  constexpr int metaSize = rackLayoutMetaSize + size * codePointLayoutMetaSize;
  Tree<metaSize> tree;
  CreateNode<BlockType::RackLayout>(&tree, size);
  for (int i = 0; i < size; i++) {
    int metaIndex = rackLayoutMetaSize + i * codePointLayoutMetaSize;
    Tree<codePointLayoutMetaSize> tree2;
    CreateNode<BlockType::CodePointLayout>(&tree2, CodePoint(S[i]));
    for (int j = 0; j < codePointLayoutMetaSize; j++) {
      tree[metaIndex + j] = tree2[j];
    }
  }
  return tree;
}

}

#endif

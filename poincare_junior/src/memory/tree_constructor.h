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
 * templated functions using Tree to be called with any TreeCompatible which
 * then casted to Tree and its template arguments deduced. */

class AbstractTreeCompatible {};

template <class C> concept TreeCompatibleConcept = Concept::is_derived_from<C, AbstractTreeCompatible>;

class AbstractTree : AbstractTreeCompatible {};

template <class C> concept TreeConcept = Concept::is_derived_from<C, AbstractTree>;


/* The Tree template class is the compile time representation of a constexpr
 * tree. It's complete block representation is specified as template parameters
 * in order to be able to use the address of the static singleton (in flash) as
 * a Node. It also eliminated identical trees since their are all using the same
 * specialized function.
 */

template <Block... Blocks>
class Tree : public AbstractTree {
public:
  static constexpr Block k_blocks[] = { Blocks... };
  static constexpr size_t k_size = sizeof...(Blocks);
  constexpr operator Node () const { return k_blocks; }
};


/* Helper to concatenate Trees */

/* Usage:
 * template <Block Tag, TreeConcept CT1, TreeConcept CT2> consteval auto Binary(CT1, CT2) {
 *   return Concat<Tree<Tag>, CT1, CT2>();
 * }
 */

template <size_t N1, const Block B1[N1], size_t N2, const Block B2[N2], typename IS = decltype(std::make_index_sequence<N1 + N2>())> struct __BlockConcat;

template <size_t N1, const Block B1[N1], size_t N2, const Block B2[N2], std::size_t... I>
struct __BlockConcat<N1, B1, N2, B2, std::index_sequence<I...>> {
  using tree = Tree<((I < N1) ? B1[I] : B2[I - N1])...>;
};

template <TreeConcept CT1, TreeConcept CT2> using __ConcatTwo = typename __BlockConcat<CT1::k_size, CT1::k_blocks, CT2::k_size, CT2::k_blocks>::tree;

template <TreeConcept CT1, TreeConcept... CT> struct Concat;
template <TreeConcept CT1> struct Concat<CT1> : CT1 {};
template <TreeConcept CT1, TreeConcept... CT> struct Concat : __ConcatTwo<CT1, Concat<CT...>> {};


// Helpers

template <Block Tag, Block... B1> consteval auto Unary(Tree<B1...>) {
  return Tree<Tag, B1...>();
}

template <Block Tag, TreeCompatibleConcept A> consteval auto Unary(A a) {
  return Unary<Tag>(Tree(a));
}

template <Block Tag, Block... B1, Block... B2> consteval auto Binary(Tree<B1...>, Tree<B2...>) {
  return Tree<Tag, B1..., B2...>();
}

template <Block Tag, TreeCompatibleConcept A, TreeCompatibleConcept B> consteval auto Binary(A a, B b) {
  return Binary<Tag>(Tree(a), Tree(b));
}

template<Block Tag, TreeConcept ...CTS> static consteval auto __NAry(CTS...) {
  return Concat<Tree<Tag, sizeof...(CTS), Tag>, CTS...>();
}

template <Block Tag, TreeCompatibleConcept ...CTS> consteval auto NAry(CTS... args) { return __NAry<Tag>(Tree(args)...); }


// Constructors

template <class...Args> consteval auto Fact(Args...args) { return Unary<BlockType::Factorial>(args...); }

template <class...Args> consteval auto Div(Args...args) { return Binary<BlockType::Division>(args...); }

template <class...Args> consteval auto Sub(Args...args) { return Binary<BlockType::Subtraction>(args...); }

template <class...Args> consteval auto Pow(Args...args) { return Binary<BlockType::Power>(args...); }

template <class...Args> consteval auto Add(Args...args) { return NAry<BlockType::Addition>(args...); }

template <class...Args> consteval auto Mult(Args...args) { return NAry<BlockType::Multiplication>(args...); }

template <class...Args> consteval auto Set(Args...args) { return NAry<BlockType::Set>(args...); }


// Alias only for readability
template <uint8_t ... Values> using Exponents = Tree<Values...>;


/* The first function is responsible of building the actual representation from
 * Trees while the other one is just here to allow the function to take
 * TreeCompatible arguments like integer litterals. */

template<TreeConcept Exp, TreeConcept ...CTS> static consteval auto __Pol(Exp exponents, CTS...) {
  constexpr uint8_t Size = sizeof...(CTS);
  return Concat<Tree<BlockType::Polynomial, Size>, Exp, Tree<Size, BlockType::Polynomial>, CTS...>();
}

template<TreeConcept Exp, TreeCompatibleConcept ...CTS> static consteval auto Pol(Exp exponents, CTS... args) {
  constexpr uint8_t Size = sizeof...(CTS);
  static_assert(Exp::k_size == Size - 1, "Number of children and exponents do not match in constant polynomial");
  return __Pol(exponents, Tree(args)...);
}


#if 0

template <Block Tag, TreeCompatible A, TreeCompatible B> consteval auto NAryOperator(A a, B b) { return NAryOperator<Tag>(Tree(a), Tree(b)); }

template <Block Tag, Block... B1, Block... B2> consteval auto NAryOperator(Tree<B1...>, Tree<B2...>) {
  return Tree<Tag, 2, Tag, B1..., B2...>();
}

template <Block Tag, Block N1, Block... B1, Block... B2> consteval auto NAryOperator(Tree<Tag, N1, Tag, B1...>, Tree<B2...>) {
  return Tree<Tag, static_cast<uint8_t>(N1) + 1, Tag, B1..., B2...>();
}

template <class...Args> consteval auto operator-(Args...args) { return Binary<BlockType::Subtraction>(args...); }

template <class...Args> consteval auto operator/(Args...args) { return Binary<BlockType::Subtraction>(args...); }

template <class...Args> consteval auto operator+(Args...args) { return NAryOperator<BlockType::Addition>(args...); }

template <class...Args> consteval auto operator*(Args...args) { return NAryOperator<BlockType::Multiplication>(args...); }

// The nice syntax above doesn't work with GCC yet and has to be expanded

template <Block... B1, Block... B2> consteval auto operator-(Tree<B1...>, Tree<B2...>) { return Tree<BlockType::Subtraction, B1..., B2...>(); }

template <TreeCompatible A, TreeCompatible B> consteval auto operator-(A a, B b) { return Tree(a) - Tree(b); }

template <Block... B1, Block... B2> consteval auto operator/(Tree<B1...>, Tree<B2...>) { return Tree<BlockType::Division, B1..., B2...>(); }

template <TreeCompatible A, TreeCompatible B> consteval auto operator/(A a, B b) { return Tree(a) / Tree(b); }

template <Block... B1, Block... B2> consteval auto operator+(Tree<B1...>, Tree<B2...>) { return Tree<BlockType::Addition, 2, BlockType::Addition, B1..., B2...>(); }

template <TreeCompatible A, TreeCompatible B> consteval auto operator+(A a, B b) { return Tree(a) + Tree(b); }

template <Block N1, Block... B1, Block... B2> consteval auto operator+(Tree<BlockType::Addition, N1, BlockType::Addition, B1...>, Tree<B2...>) {
  return Tree<BlockType::Addition, static_cast<uint8_t>(N1) + 1, BlockType::Addition, B1..., B2...>();
}

template <Block... B1, Block... B2> consteval auto operator*(Tree<B1...>, Tree<B2...>) { return Tree<BlockType::Multiplication, 2, BlockType::Multiplication, B1..., B2...>(); }

template <Block N1, Block... B1, Block... B2> consteval auto operator*(Tree<BlockType::Multiplication, N1, BlockType::Multiplication, B1...>, Tree<B2...>) {
  return Tree<BlockType::Multiplication, static_cast<uint8_t>(N1) + 1, BlockType::Multiplication, B1..., B2...>();
}

template <TreeCompatible A, TreeCompatible B> consteval auto operator*(A a, B b) { return Tree(a) * Tree(b); }

#endif


/* Immediates are used to represent numerical constants of the code (like 2_e)
 * temporarily before they are cast to Trees, this allows writing -2_e. */

template <int V> class IntegerLitteral : public AbstractTreeCompatible {
public:
  // once a deduction guide as required a given Tree from the immediate, build it
  template <Block...B> consteval operator Tree<B...> () { return Tree<B...>(); }

  constexpr operator const Node () { return Tree(IntegerLitteral<V>()); }

  consteval IntegerLitteral<-V> operator-() { return IntegerLitteral<-V>(); }
  // Note : we could decide to implement constant propagation operators here
};

// template <int8_t V> using Inti = Tree<BlockType::IntegerShort, V, BlockType::IntegerShort>;
// template <int8_t V> Tree(Immediate<V>)->Inti<V>; // only GCC accepts this one

// Deduction guides to create the smallest Tree that can represent the Immediate

Tree(IntegerLitteral<-1>)->Tree<BlockType::MinusOne>;
Tree(IntegerLitteral<0>)->Tree<BlockType::Zero>;
Tree(IntegerLitteral<1>)->Tree<BlockType::One>;
Tree(IntegerLitteral<2>)->Tree<BlockType::Two>;

template <int V> requires (V >= INT8_MIN && V <= INT8_MAX) Tree(IntegerLitteral<V>) -> Tree<BlockType::IntegerShort, V, BlockType::IntegerShort>;

template <int V> requires (V > INT8_MAX && Integer::NumberOfDigits(V) == 1) Tree(IntegerLitteral<V>) -> Tree<BlockType::IntegerPosBig, 1, Bit::getByteAtIndex(V, 0), 1, BlockType::IntegerPosBig>;

template <int V> requires (V > 0 && Integer::NumberOfDigits(V) == 2) Tree(IntegerLitteral<V>) -> Tree<BlockType::IntegerPosBig, 2, Bit::getByteAtIndex(V, 0), Bit::getByteAtIndex(V, 1), 2, BlockType::IntegerPosBig>;

template <int V> requires (V > 0 && Integer::NumberOfDigits(V) == 3) Tree(IntegerLitteral<V>) -> Tree<BlockType::IntegerPosBig, 3, Bit::getByteAtIndex(V, 0), Bit::getByteAtIndex(V, 1), Bit::getByteAtIndex(V, 2), 3, BlockType::IntegerPosBig>;

template <int V> requires (V > 0 && Integer::NumberOfDigits(V) == 4) Tree(IntegerLitteral<V>) -> Tree<BlockType::IntegerPosBig, 4, Bit::getByteAtIndex(V, 0), Bit::getByteAtIndex(V, 1), Bit::getByteAtIndex(V, 2), Bit::getByteAtIndex(V, 3), 4, BlockType::IntegerPosBig>;


template <int V> requires (V < INT8_MIN && Integer::NumberOfDigits(-V) == 1) Tree(IntegerLitteral<V>) -> Tree<BlockType::IntegerNegBig, 1, Bit::getByteAtIndex(-V, 0), 1, BlockType::IntegerNegBig>;

template <int V> requires (V < 0 && Integer::NumberOfDigits(-V) == 2) Tree(IntegerLitteral<V>) -> Tree<BlockType::IntegerNegBig, 2, Bit::getByteAtIndex(-V, 0), Bit::getByteAtIndex(-V, 1), 2, BlockType::IntegerNegBig>;

template <int V> requires (V < 0 && Integer::NumberOfDigits(-V) == 3) Tree(IntegerLitteral<V>) -> Tree<BlockType::IntegerNegBig, 3, Bit::getByteAtIndex(-V, 0), Bit::getByteAtIndex(-V, 1), Bit::getByteAtIndex(-V, 2), 3, BlockType::IntegerNegBig>;

template <int V> requires (V < 0 && Integer::NumberOfDigits(-V) == 4) Tree(IntegerLitteral<V>) -> Tree<BlockType::IntegerNegBig, 4, Bit::getByteAtIndex(-V, 0), Bit::getByteAtIndex(-V, 1), Bit::getByteAtIndex(-V, 2), Bit::getByteAtIndex(-V, 3), 4, BlockType::IntegerNegBig>;

// TODO new node_constructor
constexpr Tree π_e = Tree<BlockType::Constant, static_cast<uint8_t>(Constant::Type::Pi), BlockType::Constant>();

// TODO: move in OMG?
constexpr static uint64_t IntegerValue(const char * str, size_t size) {
  uint64_t value = 0;
  for (int i = 0; i < size - 1; i++) {
    uint8_t digit = OMG::Print::DigitForCharacter(str[i]);
    // No overflow
    constexpr_assert(value <= (UINT64_MAX - digit)/10);
    value = 10 * value + digit;
  }
  return value;
}

constexpr bool HasDecimalPoint(const char * str, size_t size) {
  for (int i = 0; i < size - 1; i++) {
    if (str[i] == '.') {
      return true;
    }
  }
  return false;
}

constexpr static float FloatValue(const char * str, size_t size) {
  float value = 0;
  bool fractionalPart = false;
  float base = 1;
  for (int i = 0; i < size - 1; i++) {
    if (str[i] == '.') {
      fractionalPart = true;
      continue;
    }
    uint8_t digit = OMG::Print::DigitForCharacter(str[i]);
    // No overflow
    constexpr_assert(value <= (UINT64_MAX - digit)/10);
    if (!fractionalPart) {
      value = 10 * value + digit;
    } else {
      // TODO use a better algo precision-wise
      base *= 10.f;
      value += digit / base;
    }
  }
  return value;
}

/* A template <float V> would be cool but support for this is poor yet so we
 * have to store the bit representation of the float. */
template <uint32_t V> class FloatLitteral : public AbstractTreeCompatible {
public:
  template <Block...B> consteval operator Tree<B...> () { return Tree<B...>(); }

  constexpr operator const Node () { return Tree(FloatLitteral<V>()); }

  // Since we are using the representation we have to manually flip the sign bit
  consteval auto operator-() { return FloatLitteral<V ^ (1 << 31)>(); }
};

template <uint32_t V> Tree(FloatLitteral<V>) -> Tree<BlockType::Float, Bit::getByteAtIndex(V, 0), Bit::getByteAtIndex(V, 1), Bit::getByteAtIndex(V, 2), Bit::getByteAtIndex(V, 3), BlockType::Float>;

template <char...C>
consteval auto operator"" _e () {
  constexpr const char value[] = { C... , '\0' };
  if constexpr (HasDecimalPoint(value, sizeof...(C) + 1)) {
    return FloatLitteral<std::bit_cast<uint32_t>(FloatValue(value, sizeof...(C) + 1))>();
  } else {
    return IntegerLitteral<IntegerValue(value, sizeof...(C) + 1)>();
  }
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
  static_assert(!OMG::Print::IsDigit(S[0]), "Integer litterals should be written without quotes");
  using tree = Tree<BlockType::UserSymbol, sizeof...(I), S[I]..., sizeof...(I), BlockType::UserSymbol>;
};

template <String S>
consteval auto operator"" _e () {
  return typename Variable<S>::tree();
}

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

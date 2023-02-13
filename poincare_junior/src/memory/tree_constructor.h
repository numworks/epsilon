#ifndef POINCARE_MEMORY_TREE_CONSTRUCTOR_H
#define POINCARE_MEMORY_TREE_CONSTRUCTOR_H

#include <array>
#include "node_constructor.h"
#include "node.h"
#include <poincare_junior/src/expression/integer.h>
#include <omgpj/assert.h>
#include <omg/print.h>
#include <omgpj/print.h>

namespace PoincareJ {

// https://stackoverflow.com/questions/40920149/is-it-possible-to-create-templated-user-defined-literals-literal-suffixes-for
// https://akrzemi1.wordpress.com/2012/10/29/user-defined-literals-part-iii/

class AbstractCTreeCompatible {};

// less strict than derived_from from <concepts> that imposes is_convertible too
template<typename Derived, typename Base>
concept is_derived_from = __is_base_of(Base, Derived);

template <class C> concept CTreeCompatible = is_derived_from<C, AbstractCTreeCompatible>;

class AbstractCTree : AbstractCTreeCompatible {};

template <class C> concept CTreeish = is_derived_from<C, AbstractCTree>;

template <Block... Blocks>
class CTree : public AbstractCTree {
public:
  static constexpr Block blocks[] = { Blocks... };
  static constexpr size_t size = sizeof...(Blocks);
  constexpr operator Node () const { return blocks; }
};

template <size_t N, const uint8_t B[N], typename IS = decltype(std::make_index_sequence<N>())> struct CTreeFromArray;

template <size_t N, const uint8_t B[N], std::size_t... I>
struct CTreeFromArray<N, B, std::index_sequence<I...>> : CTree<B[I]...> {};

template <size_t N1, const Block B1[N1], size_t N2, const Block B2[N2], typename IS = decltype(std::make_index_sequence<N1 + N2>())> struct BlockConcat;

template <size_t N1, const Block B1[N1], size_t N2, const Block B2[N2], std::size_t... I>
struct BlockConcat<N1, B1, N2, B2, std::index_sequence<I...>> {
  using ctree = CTree<((I < N1) ? B1[I] : B2[I - N1])...>;
};

template <CTreeish CT1, CTreeish CT2> using ConcatTwo = typename BlockConcat<CT1::size, CT1::blocks, CT2::size, CT2::blocks>::ctree;

template <CTreeish CT1, CTreeish... CT> struct Concat;
template <CTreeish CT1> struct Concat<CT1> : CT1 {};
template <CTreeish CT1, CTreeish... CT> struct Concat : ConcatTwo<CT1, Concat<CT...>> {};

// Usage:
// template <Block Tag, CTreeish CT1, CTreeish CT2> consteval auto Binary(CT1, CT2) {
//   return ConcatTwo<ConcatTwo<CTree<Tag>, CT1>, CT2>();
//      or
//   return Concat<CTree<Tag>, CT1, CT2>();
// }

// Helpers

template <Block Tag, Block... B1> consteval auto Unary(CTree<B1...>) {
  return CTree<Tag, B1...>();
}

template <Block Tag, CTreeCompatible A> consteval auto Unary(A a) {
  return Unary<Tag>(CTree(a));
}

template <Block Tag, Block... B1, Block... B2> consteval auto Binary(CTree<B1...>, CTree<B2...>) {
  return CTree<Tag, B1..., B2...>();
}

template <Block Tag, CTreeCompatible A, CTreeCompatible B> consteval auto Binary(A a, B b) {
  return Binary<Tag>(CTree(a), CTree(b));
}

template <Block Tag, Block... B1, Block... B2> consteval auto __NAry(CTree<B1...>, CTree<B2...>) {
  return CTree<Tag, 2, Tag, B1..., B2...>();
}

template <Block Tag, Block... B1, Block... B2, Block... B3> consteval auto __NAry(CTree<B1...>, CTree<B2...>, CTree<B3...>) {
  return CTree<Tag, 3, Tag, B1..., B2..., B3...>();
}

template <Block Tag, Block... B1, Block... B2, Block... B3, Block... B4> consteval auto __NAry(CTree<B1...>, CTree<B2...>, CTree<B3...>, CTree<B4...>) {
  return CTree<Tag, 4, Tag, B1..., B2..., B3..., B4...>();
}

template <Block Tag, Block... B1, Block... B2> consteval auto NAryOperator(CTree<B1...>, CTree<B2...>) {
  return CTree<Tag, 2, Tag, B1..., B2...>();
}

template <Block Tag, Block N1, Block... B1, Block... B2> consteval auto NAryOperator(CTree<Tag, N1, Tag, B1...>, CTree<B2...>) {
  return CTree<Tag, static_cast<uint8_t>(N1) + 1, Tag, B1..., B2...>();
}

// Enable operators for structs that have deduction guides into CTrees
template <Block Tag, CTreeCompatible ...CTS> consteval auto NAry(CTS... args) { return __NAry<Tag>(CTree(args)...); }

template <Block Tag, CTreeCompatible A, CTreeCompatible B> consteval auto NAryOperator(A a, B b) { return NAryOperator<Tag>(CTree(a), CTree(b)); }


// Constructors

template <class...Args> consteval auto Fact(Args...args) { return Unary<BlockType::Factorial>(args...); }

template <class...Args> consteval auto Div(Args...args) { return Binary<BlockType::Division>(args...); }

template <class...Args> consteval auto Sub(Args...args) { return Binary<BlockType::Subtraction>(args...); }

template <class...Args> consteval auto Pow(Args...args) { return Binary<BlockType::Power>(args...); }

template <class...Args> consteval auto Addi(Args...args) { return NAry<BlockType::Addition>(args...); }

template <class...Args> consteval auto Multi(Args...args) { return NAry<BlockType::Multiplication>(args...); }

template <class...Args> consteval auto Seti(Args...args) { return NAry<BlockType::Set>(args...); }


template <uint8_t ... Values> using Exponents = CTree<Values...>;

template<CTreeish Exp, CTreeish ...CTS> static consteval auto __Poly(Exp exponents, CTS...) {
  constexpr uint8_t Size = sizeof...(CTS);
  return Concat<CTree<BlockType::Polynomial, Size>, Exp, CTree<Size, BlockType::Polynomial>, CTS...>();
}

template<CTreeish Exp, CTreeCompatible ...CTS> static consteval auto Poly(Exp exponents, CTS... args) {
  constexpr uint8_t Size = sizeof...(CTS);
  static_assert(Exp::size == Size - 1, "Number of children and exponents do not match in constant polynomial");
  return __Poly(exponents, CTree(args)...);
}


#if 0

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


// Integers

template <int V> requires (V >= INT8_MIN && V <= INT8_MAX) consteval auto Int() {
  return CTree<BlockType::IntegerShort, V, BlockType::IntegerShort>();
}

template <Block S, Block T, int U> struct IntHelper;
template <Block T, int V> struct IntHelper<2,T,V> : CTree<T, 2, Bit::getByteAtIndex(V, 0), Bit::getByteAtIndex(V, 1), 2, T> {};
template <Block T, int V> struct IntHelper<3,T,V> : CTree<T, 3, Bit::getByteAtIndex(V, 0), Bit::getByteAtIndex(V, 1), Bit::getByteAtIndex(V, 2), 3, T> {};
template <Block T, int V> struct IntHelper<4,T,V> : CTree<T, 4, Bit::getByteAtIndex(V, 0), Bit::getByteAtIndex(V, 1), Bit::getByteAtIndex(V, 2), Bit::getByteAtIndex(V, 3), 4, T> {};

template <int V> requires (V < INT8_MIN || V > INT8_MAX) consteval auto Int() {
  constexpr Block tag = V < 0 ? BlockType::IntegerNegBig : BlockType::IntegerPosBig;
  constexpr int value = V < 0 ? -V : V;
  constexpr Block size = Integer::NumberOfDigits(value);
  return IntHelper<size, tag, value>();
}

template<> consteval auto Int<-1>() { return CTree<BlockType::MinusOne>(); }
template<> consteval auto Int<0>() { return CTree<BlockType::Zero>(); }
template<> consteval auto Int<1>() { return CTree<BlockType::One>(); }
template<> consteval auto Int<2>() { return CTree<BlockType::Two>(); }


// Unary operator-

template <int V> class Immediate : public AbstractCTreeCompatible {
public:
  // once a deduction guide as required a given CTree from the immediate, build it
  template <Block...B> consteval operator CTree<B...> () { return CTree<B...>(); }

  constexpr operator const Node () { return CTree(Immediate<V>()); }

  consteval Immediate<-V> operator-() { return Immediate<-V>(); }
  // Note : we could decide to implement constant propagation operators here
};

// template <int8_t V> using Inti = CTree<BlockType::IntegerShort, V, BlockType::IntegerShort>;
// template <int8_t V> CTree(Immediate<V>)->Inti<V>; // only GCC accepts this one

// Deduction guides to create the smallest CTree that can represent the Immediate

CTree(Immediate<-1>)->CTree<BlockType::MinusOne>;
CTree(Immediate<0>)->CTree<BlockType::Zero>;
CTree(Immediate<1>)->CTree<BlockType::One>;
CTree(Immediate<2>)->CTree<BlockType::Two>;

template <int V> requires (V >= INT8_MIN && V <= INT8_MAX) CTree(Immediate<V>) -> CTree<BlockType::IntegerShort, V, BlockType::IntegerShort>;

template <int V> requires (V > 0 && Integer::NumberOfDigits(V) == 2) CTree(Immediate<V>) -> CTree<BlockType::IntegerPosBig, 2, Bit::getByteAtIndex(V, 0), Bit::getByteAtIndex(V, 1), 2, BlockType::IntegerPosBig>;

template <int V> requires (V > 0 && Integer::NumberOfDigits(V) == 3) CTree(Immediate<V>) -> CTree<BlockType::IntegerPosBig, 3, Bit::getByteAtIndex(V, 0), Bit::getByteAtIndex(V, 1), Bit::getByteAtIndex(V, 2), 3, BlockType::IntegerPosBig>;

template <int V> requires (V > 0 && Integer::NumberOfDigits(V) == 4) CTree(Immediate<V>) -> CTree<BlockType::IntegerPosBig, 4, Bit::getByteAtIndex(V, 0), Bit::getByteAtIndex(V, 1), Bit::getByteAtIndex(V, 2), Bit::getByteAtIndex(V, 3), 4, BlockType::IntegerPosBig>;


template <int V> requires (V < 0 && Integer::NumberOfDigits(-V) == 2) CTree(Immediate<V>) -> CTree<BlockType::IntegerNegBig, 2, Bit::getByteAtIndex(-V, 0), Bit::getByteAtIndex(-V, 1), 2, BlockType::IntegerNegBig>;

template <int V> requires (V < 0 && Integer::NumberOfDigits(-V) == 3) CTree(Immediate<V>) -> CTree<BlockType::IntegerNegBig, 3, Bit::getByteAtIndex(-V, 0), Bit::getByteAtIndex(-V, 1), Bit::getByteAtIndex(-V, 2), 3, BlockType::IntegerNegBig>;

template <int V> requires (V < 0 && Integer::NumberOfDigits(-V) == 4) CTree(Immediate<V>) -> CTree<BlockType::IntegerNegBig, 4, Bit::getByteAtIndex(-V, 0), Bit::getByteAtIndex(-V, 1), Bit::getByteAtIndex(-V, 2), Bit::getByteAtIndex(-V, 3), 4, BlockType::IntegerNegBig>;




constexpr static uint64_t Value(const char * str, size_t size);
template <char...C>
consteval auto operator"" _n () {
  constexpr const char value[] = { C... , '\0' };
  constexpr int V = Value(value, sizeof...(C) + 1);
  return Immediate<V>();
}



template <unsigned N>
class Tree {
public:
  // TODO: make all constructor consteval
  constexpr Tree() {}
  constexpr Block & operator[] (size_t n) { return m_blocks[n]; }
  constexpr operator Node() const { return Node(const_cast<TypeBlock *>(m_blocks)); }
private:
  // Using this instead of a Block[N] simplifies up casting in constexprs
  TypeBlock m_blocks[N];
};

template <BlockType blockType, unsigned N, typename... Types>
constexpr static void CreateNode(Tree<N> * tree, Types... args) {
  size_t i = 0;
  while (!NodeConstructor::CreateBlockAtIndexForType<blockType>(&(*tree)[i], i, args...)) {
    i++;
  }
}

template<unsigned N, unsigned ...Len>
constexpr static auto MakeChildren(Tree<N> * tree, size_t blockIndex, const Tree<Len> (&...nodes)) {
  size_t childIndex = 0;
  size_t childrenSizes[] = {Len...};
  std::initializer_list<Node> childrenNodes{static_cast<Node>(nodes)...};
  for (Node node : childrenNodes) {
    // We can't use node.copyTreeTo(tree.blockAtIndex(blockIndex++)) because memcpy isn't constexpr
    // TODO: use constexpr version of memcpy in copyTreeTo?
    for (size_t i = 0; i < childrenSizes[childIndex]; i++) {
      (*tree)[blockIndex++] = *(node.block() + i);
    }
    childIndex++;
  }
  return tree;
}

template<BlockType type, unsigned ...Len>
consteval static auto MakeTree(const Tree<Len> (&...nodes)) {
  // Compute the total length of the children
  constexpr unsigned k_numberOfChildren = sizeof...(Len);
  constexpr unsigned k_numberOfChildrenBlocks = (0 + ... + Len);
  constexpr size_t numberOfBlocksInNode = TypeBlock::NumberOfMetaBlocks(type);

  Tree<k_numberOfChildrenBlocks + numberOfBlocksInNode> tree;
  CreateNode<type>(&tree, k_numberOfChildren);

  MakeChildren(&tree, numberOfBlocksInNode, nodes...);
  return tree;
}

template<unsigned ...Len> static consteval auto Add(const Tree<Len> (&...children)) { return MakeTree<BlockType::Addition>(children...); }

template<unsigned L1, unsigned L2> static consteval Tree<L1+L2+1> Div(const Tree<L1> child1, const Tree<L2> child2) { return MakeTree<BlockType::Division>(child1, child2); }

template<unsigned ...Len> static consteval auto Mult(const Tree<Len> (&...children)) { return MakeTree<BlockType::Multiplication>(children...); }

template<unsigned ...Len> static consteval auto Set(const Tree<Len> (&...children)) { return MakeTree<BlockType::Set>(children...); }

template<unsigned L1, unsigned L2> static consteval Tree<L1+L2+1> Pow(const Tree<L1> child1, const Tree<L2> child2) { return MakeTree<BlockType::Power>(child1, child2); }

template<unsigned L1, unsigned L2> static consteval Tree<L1+L2+1> Sub(const Tree<L1> child1, const Tree<L2> child2) { return MakeTree<BlockType::Subtraction>(child1, child2); }

template<unsigned ...Len> static consteval auto Pol(std::array<uint8_t, sizeof...(Len) - 1> exponents, const Tree<Len> (&...coefficients)) {
  // Compute the total length of the children
  constexpr unsigned k_numberOfChildren = sizeof...(Len);
  constexpr unsigned k_numberOfChildrenBlocks = (0 + ... + Len);
  constexpr size_t numberOfBlocksInNode = TypeBlock::NumberOfMetaBlocks(BlockType::Polynomial) + k_numberOfChildren - 1;

  Tree<k_numberOfChildrenBlocks + numberOfBlocksInNode> tree;
  size_t currentTreeIndex = 0;
  tree[currentTreeIndex++] = TypeBlock(BlockType::Polynomial);
  tree[currentTreeIndex++] = k_numberOfChildren;
  for (size_t i = 0; i < k_numberOfChildren - 1; i++) {
    tree[currentTreeIndex++] = exponents[i];
  }
  tree[currentTreeIndex++] = k_numberOfChildren;
  tree[currentTreeIndex++] = TypeBlock(BlockType::Polynomial);

  MakeChildren(&tree, currentTreeIndex, coefficients...);
  return tree;
}

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
consteval auto operator"" _v () {
  return typename Variable<S>::ctree();
}

template <String S>
constexpr unsigned IntegerTreeSize(std::initializer_list<char> specialChars, uint64_t maxValueInShortInteger, BlockType genericBlockType) {
  const char * chars = S.m_data;
  size_t size = S.size();
  constexpr_assert(size > 1);
  if (chars[0] == '-') {
    size--;
    chars = chars + 1;
  }
  if (size == 2) {
    for (char c : specialChars) {
      if (c == chars[0]) {
        return 1;
      }
    }
  }
  uint64_t value = Value(chars, size);
  if (value <= maxValueInShortInteger) {
    return TypeBlock::NumberOfMetaBlocks(BlockType::IntegerShort);
  }
  return TypeBlock::NumberOfMetaBlocks(genericBlockType) + Integer::NumberOfDigits(value);
}

template<String S>
constexpr unsigned TreeSize() {
  const char * chars = S.m_data;
  size_t size = S.size();
  constexpr_assert(size > 1);
  if (OMG::Print::IsLowercaseLetter(*chars)) {
    // Symbol
    return size - 1 + TypeBlock::NumberOfMetaBlocks(BlockType::UserSymbol);
  }
  if (chars[0] == '-') {
    // Negative integer
    return IntegerTreeSize<S>({'1'}, -INT8_MIN, BlockType::IntegerNegBig);
  } else {
    // Positive integer
    constexpr_assert(OMG::Print::IsDigit(chars[0]));
    return IntegerTreeSize<S>({'0', '1', '2'}, INT8_MAX, BlockType::IntegerPosBig);
  }
}

template <String S>
constexpr Tree<TreeSize<S>()> operator"" _n() {
  constexpr unsigned treeSize = TreeSize<S>();
  Tree<treeSize> tree;
  const char * chars = S.m_data;
  size_t size = S.size();
  constexpr_assert(S.size() > 1);
  if (OMG::Print::IsLowercaseLetter(*chars)) {
    // Symbol
    CreateNode<BlockType::UserSymbol>(&tree, S.m_data, size - 1);
    return tree;
  }
  // Integer
  bool negativeInt = chars[0] == '-';
  if (negativeInt) {
    chars = chars + 1;
    size--;
  }
  constexpr_assert(OMG::Print::IsDigit(*chars));
  if (treeSize == 1) {
    constexpr_assert(size == 2);
    switch (*chars) {
      case '0':
        CreateNode<BlockType::Zero>(&tree);
        return tree;
      case '1':
      {
        if (negativeInt) {
          CreateNode<BlockType::MinusOne>(&tree);
        } else {
          CreateNode<BlockType::One>(&tree);
        }
        return tree;
      }
      case '2':
        CreateNode<BlockType::Two>(&tree);
        return tree;
      default:
          constexpr_assert(false);
    }
  } else {
    uint64_t value = Value(chars, size);
    if (treeSize == 3) {
      constexpr_assert(value != 0 && value != 1 && value != 2);
      constexpr_assert(value <= INT8_MAX);
      int8_t truncatedValue = static_cast<int8_t>(negativeInt ? -value : value);
      CreateNode<BlockType::IntegerShort>(&tree, truncatedValue);
    } else {
      if (negativeInt) {
        CreateNode<BlockType::IntegerNegBig>(&tree, value);
      } else {
        CreateNode<BlockType::IntegerPosBig>(&tree, value);
      }
    }
  }
  return tree;
}

constexpr Tree<TypeBlock::NumberOfMetaBlocks(BlockType::Constant)> operator "" _n(char16_t name) {
  Tree<TypeBlock::NumberOfMetaBlocks(BlockType::Constant)> tree;
  CreateNode<BlockType::Constant>(&tree, name);
  return tree;
}

constexpr Tree<TypeBlock::NumberOfMetaBlocks(BlockType::Float)> operator "" _n(long double value) {
  // TODO: integrate to template <String S> operator"" _n() to be able to parse "-1.2"_n
  Tree<TypeBlock::NumberOfMetaBlocks(BlockType::Float)> tree;
  CreateNode<BlockType::Float>(&tree, static_cast<float>(value));
  return tree;
}

}

#endif

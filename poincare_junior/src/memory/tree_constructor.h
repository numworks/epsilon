#ifndef POINCARE_MEMORY_TREE_CONSTRUCTOR_H
#define POINCARE_MEMORY_TREE_CONSTRUCTOR_H

#include <array>
#include "node_constructor.h"
#include "node.h"
#include "pattern_matching.h"
#include <poincare_junior/src/expression/integer.h>
#include <omgpj/assert.h>
#include <omg/print.h>
#include <omgpj/print.h>

namespace PoincareJ {

// https://stackoverflow.com/questions/40920149/is-it-possible-to-create-templated-user-defined-literals-literal-suffixes-for
// https://akrzemi1.wordpress.com/2012/10/29/user-defined-literals-part-iii/

template <BlockType... Blocks>
class CTree {
public:
  static constexpr TypeBlock blocks[] = { Blocks... };
  constexpr operator const Node () { return blocks; }
};

// Helpers

template <BlockType Tag, BlockType... B1> consteval auto Unary(CTree<B1...>) {
  return CTree<Tag, B1...>();
}

template <BlockType Tag, BlockType... B1, BlockType... B2> consteval auto Binary(CTree<B1...>, CTree<B2...>) {
  return CTree<Tag, B1..., B2...>();
}

template <BlockType Tag, BlockType... B1, BlockType... B2> consteval auto NAry(CTree<B1...>, CTree<B2...>) {
  return CTree<Tag, static_cast<BlockType>(2), Tag, B1..., B2...>();
}

template <BlockType Tag, BlockType... B1, BlockType... B2, BlockType... B3> consteval auto NAry(CTree<B1...>, CTree<B2...>, CTree<B3...>) {
  return CTree<Tag, static_cast<BlockType>(3), Tag, B1..., B2..., B3...>();
}

template <BlockType Tag, BlockType... B1, BlockType... B2, BlockType... B3, BlockType... B4> consteval auto NAry(CTree<B1...>, CTree<B2...>, CTree<B3...>, CTree<B4...>) {
  return CTree<Tag, static_cast<BlockType>(4), Tag, B1..., B2..., B3..., B4...>();
}


// Constructors

template <class...Args> consteval auto Fact(Args...args) { return Unary<BlockType::Factorial>(args...); }

template <class...Args> consteval auto Div(Args...args) { return Binary<BlockType::Division>(args...); }

template <class...Args> consteval auto Sub(Args...args) { return Binary<BlockType::Subtraction>(args...); }

template <class...Args> consteval auto Pow(Args...args) { return Binary<BlockType::Power>(args...); }

template <class...Args> consteval auto Add(Args...args) { return NAry<BlockType::Addition>(args...); }

template <class...Args> consteval auto Mult(Args...args) { return NAry<BlockType::Multiplication>(args...); }

template <class...Args> consteval auto Set(Args...args) { return NAry<BlockType::Set>(args...); }

// Integers

static constexpr int k_minIntegerShort = -(1 << 7);
static constexpr int k_maxIntegerShort = (1 << 7) - 1; // INT8MAX
static constexpr int k_maxIntegerPosBigTwoBytes = (1 << 16) - 1;
static constexpr int k_maxIntegerPosBigThreeBytes = (1 << 24) - 1;
static constexpr int k_minIntegerNegBigTwoBytes = - (1 << 16) - 1;

template <int V> requires (V >= k_minIntegerShort && V <= k_maxIntegerShort) consteval auto Int() {
  return CTree<BlockType::IntegerShort, static_cast<BlockType>(V), BlockType::IntegerShort>();
}

// TODO produce the following with a template

template <int V> requires (V > k_maxIntegerShort && V <= k_maxIntegerPosBigTwoBytes) consteval auto Int() {
  return CTree<BlockType::IntegerPosBig, static_cast<BlockType>(2), static_cast<BlockType>(V%256), static_cast<BlockType>(V/256), static_cast<BlockType>(2), BlockType::IntegerPosBig>();
}

template <int V> requires (V < k_minIntegerShort && V >= k_minIntegerNegBigTwoBytes) consteval auto Int() {
  return CTree<BlockType::IntegerNegBig, static_cast<BlockType>(2), static_cast<BlockType>((-V)%256), static_cast<BlockType>((-V)/256), static_cast<BlockType>(2), BlockType::IntegerNegBig>();
}

template<> consteval auto Int<-1>() { return CTree<BlockType::MinusOne>(); }
template<> consteval auto Int<0>() { return CTree<BlockType::Zero>(); }
template<> consteval auto Int<1>() { return CTree<BlockType::One>(); }
template<> consteval auto Int<2>() { return CTree<BlockType::Two>(); }


template <unsigned N>
class Tree {
public:
  // TODO: make all constructor consteval
  constexpr Tree() {}
  constexpr Tree(PatternMatching::Placeholder &&placeholder);
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
};

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

// Deduction guide
Tree(PatternMatching::Placeholder placeholder) -> Tree<TypeBlock::NumberOfMetaBlocks(BlockType::Placeholder)>;

template <>
constexpr Tree<TypeBlock::NumberOfMetaBlocks(BlockType::Placeholder)>::Tree(PatternMatching::Placeholder &&placeholder) {
  CreateNode<BlockType::Placeholder>(this, placeholder);
}

}

#endif

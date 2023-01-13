#ifndef POINCARE_MEMORY_TREE_CONSTRUCTOR_H
#define POINCARE_MEMORY_TREE_CONSTRUCTOR_H

#include <array>
#include <omg/print.h>
#include <omg/print.h>
#include "node_constructor.h"
#include "node.h"
#include <poincare_junior/src/expression/integer.h>

namespace Poincare {

#warning Ensure that we can't use it in a non-constexpr mode
// TODO Use consteval? We should ensure one way or another that the number of specialized class is limited and used for constexpr methods only. Meaning that no code should be emited at runtime...

// https://stackoverflow.com/questions/40920149/is-it-possible-to-create-templated-user-defined-literals-literal-suffixes-for
// https://akrzemi1.wordpress.com/2012/10/29/user-defined-literals-part-iii/


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
  int childrenSizes[] = {Len...};
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
constexpr static auto MakeTree(const Tree<Len> (&...nodes)) {
  // Compute the total length of the children
  constexpr unsigned k_numberOfChildren = sizeof...(Len);
  constexpr unsigned k_numberOfChildrenBlocks = (0 + ... + Len);
  constexpr size_t numberOfBlocksInNode = TypeBlock::NumberOfMetaBlocks(type);

  Tree<k_numberOfChildrenBlocks + numberOfBlocksInNode> tree;
  CreateNode<type>(&tree, k_numberOfChildren);

  MakeChildren(&tree, numberOfBlocksInNode, nodes...);
  return tree;
}

template<unsigned ...Len> static constexpr auto Add(const Tree<Len> (&...children)) { return MakeTree<BlockType::Addition>(children...); }

template<unsigned L1, unsigned L2> static constexpr Tree<L1+L2+1> Div(const Tree<L1> child1, const Tree<L2> child2) { return MakeTree<BlockType::Division>(child1, child2); }

template<unsigned ...Len> static constexpr auto Mult(const Tree<Len> (&...children)) { return MakeTree<BlockType::Multiplication>(children...); }

template<unsigned ...Len> static constexpr auto Set(const Tree<Len> (&...children)) { return MakeTree<BlockType::Set>(children...); }

template<unsigned L1, unsigned L2> static constexpr Tree<L1+L2+1> Pow(const Tree<L1> child1, const Tree<L2> child2) { return MakeTree<BlockType::Power>(child1, child2); }

template<unsigned L1, unsigned L2> static constexpr Tree<L1+L2+1> Sub(const Tree<L1> child1, const Tree<L2> child2) { return MakeTree<BlockType::Subtraction>(child1, child2); }

template<unsigned ...Len> static constexpr auto Pol(std::array<uint8_t, sizeof...(Len) - 1> exponents, const Tree<Len> (&...coefficients)) {
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
constexpr static uint64_t Value(const char * str, size_t len) {
  uint64_t value = 0;
  for (int i = 0; i < len; i++) {
    uint8_t digit = OMG::Print::DigitForCharacter(str[i]);
    // No overflow
    assert(value <= (UINT64_MAX - digit)/10);
    value = 10 * value + digit;
  }
  return value;
}

template <char... str>
constexpr unsigned IntegerTreeSize(std::initializer_list<char> specialChars, uint64_t maxValueInShortInteger, BlockType genericBlockType) {
  constexpr unsigned size = sizeof...(str);
  char chars[size] = {static_cast<char>(str)...};
  assert(size > 0);
  if (size == 1) {
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


template <char... str>
constexpr unsigned SignedIntegerTreeSize() {
  return IntegerTreeSize<str...>({'1'}, -INT8_MIN, BlockType::IntegerNegBig);
}

template <char... str>
constexpr unsigned UnsignedIntegerTreeSize() {
  return IntegerTreeSize<str...>({'0', '1', '2'}, INT8_MAX, BlockType::IntegerPosBig);
}

template <char... str>
constexpr Tree<UnsignedIntegerTreeSize<str...>()> operator"" _ui_n()
{
  constexpr unsigned size = sizeof...(str);
  char chars[size] = {static_cast<char>(str)...};
  constexpr unsigned treeSize = UnsignedIntegerTreeSize<str...>();
  Tree<treeSize> tree;
  if (treeSize == 1) {
    assert(size == 1);
    switch (chars[0]) {
      case '0':
        CreateNode<BlockType::Zero>(&tree);
        break;
      case '1':
        CreateNode<BlockType::One>(&tree);
        break;
      case '2':
        CreateNode<BlockType::Two>(&tree);
        break;
      default:
        assert(false);
    }
  } else {
    uint64_t value = Value(chars, size);
    if (treeSize == 3) {
      assert(value != 0 && value != 1 && value != 2);
      assert(value <= INT8_MAX);
      CreateNode<BlockType::IntegerShort>(&tree, static_cast<int8_t>(value));
    } else {
      CreateNode<BlockType::IntegerPosBig>(&tree, value);
    }
  }
  return tree;
}


template <char... str>
constexpr Tree<SignedIntegerTreeSize<str...>()> operator"" _si_n()
{
  constexpr unsigned size = sizeof...(str);
  char chars[size] = {static_cast<char>(str)...};
  constexpr unsigned treeSize = SignedIntegerTreeSize<str...>();
  Tree<treeSize> tree;
  if (treeSize == 1) {
    assert(size == 1);
    switch (chars[0]) {
      case '1':
        CreateNode<BlockType::MinusOne>(&tree);
        break;
      default:
        assert(false);
    }
  } else {
    uint64_t value = Value(chars, size);
    if (treeSize == 3) {
      assert(value != 1);
      assert(value <= -INT8_MIN);
      CreateNode<BlockType::IntegerShort>(&tree, static_cast<int8_t>(-value));
    } else {
      CreateNode<BlockType::IntegerNegBig>(&tree, value);
    }
  }
  return tree;
}

template<size_t N>
struct String {
  char m_data[N];
  constexpr size_t size() const { return N; }
  template <std::size_t... Is>
  constexpr String(const char (&arr)[N], std::integer_sequence<std::size_t, Is...>) : m_data{arr[Is]...} {}
  constexpr String(char const(&arr)[N]) : String(arr, std::make_integer_sequence<std::size_t, N>()) {}

};

template<String S>
constexpr Tree<S.size() + TypeBlock::NumberOfMetaBlocks(BlockType::UserSymbol)> operator"" _uds_n() {
  constexpr size_t size = S.size();
  constexpr size_t treeSize = size + TypeBlock::NumberOfMetaBlocks(BlockType::UserSymbol);
  Tree<treeSize> tree;
  CreateNode<BlockType::UserSymbol>(&tree, S.m_data, size);
  return tree;

}

constexpr Tree<TypeBlock::NumberOfMetaBlocks(BlockType::Constant)> operator "" _n(char16_t name) {
  Tree<TypeBlock::NumberOfMetaBlocks(BlockType::Constant)> tree;
  CreateNode<BlockType::Constant>(&tree, name);
  return tree;
}

// TODO: improve suffix format

constexpr Tree<1> operator "" _nsn(unsigned long long value) { // single-block node
  Tree<1> tree;
  switch (value) {
    case 1:
      CreateNode<BlockType::MinusOne>(&tree);
      break;
    default:
      assert(false);
  }
  return tree;
}

constexpr Tree<1> operator "" _sn(unsigned long long value) { // single-block node
  Tree<1> tree;
  switch (value) {
    case 0:
      CreateNode<BlockType::Zero>(&tree);
      break;
    case 1:
      CreateNode<BlockType::One>(&tree);
      break;
    case 2:
      CreateNode<BlockType::Two>(&tree);
      break;
    default:
      assert(false);
  }
  return tree;
}

constexpr Tree<3> operator "" _n(unsigned long long value) {
  assert(value != 0 && value != -1 && value != 1 && value != 2); // TODO: make this robust
  assert(value < 128); // TODO: handle negative small numbers?
  Tree<3> tree;
  CreateNode<BlockType::IntegerShort>(&tree, static_cast<int8_t>(value));
  return tree;
}

constexpr Tree<TypeBlock::NumberOfMetaBlocks(BlockType::Float)> operator "" _fn(long double value) {
  Tree<TypeBlock::NumberOfMetaBlocks(BlockType::Float)> tree;
  CreateNode<BlockType::Float>(&tree, static_cast<float>(value));
  return tree;
}

}

#endif

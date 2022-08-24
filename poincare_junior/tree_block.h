#ifndef POINCARE_TREE_BLOCK_H
#define POINCARE_TREE_BLOCK_H

#define POINCARE_TREE_LOG 1
#if POINCARE_TREE_LOG
#include <ostream>
#endif

#include "properties.h"

namespace Poincare {

/* A block is a byte-long object containing either a type or some value.
 * Several blocks can form a node, like:
 * [INT][LENGTH][DIGIT0][DIGIT1]...[DIGITN][LENGTH][INT]
 * [ADDITION][LENGTH][ADDITION]
 * A node can also be composed of a single block:
 * [COSINE]
 */

enum class BlockType : uint8_t {
#if GHOST_REQUIRED
  Ghost,
#endif
  Integer,
  IntegerShort,
  Float,
  Addition,
  Multiplication,
  Subtraction,
  Division,
  Power,
  Constant
};

class Handle;
class TreeSandbox;
class TypeTreeBlock;

struct IndexedTypeTreeBlock {
  TypeTreeBlock * m_block;
  int m_index;
};

typedef void (TypeTreeBlock::*InPlaceTreeFunction)();

// This helper wraps a motion on const blocks into a motion on non-const blocks.
// It should be completely eliminated by the compiler after the type checking.
typedef const TypeTreeBlock * (TypeTreeBlock::*ConstMotion)() const;
inline TypeTreeBlock * motion(TypeTreeBlock *object, ConstMotion constMotion) {
  return const_cast<TypeTreeBlock*>((const_cast<const TypeTreeBlock*>(object)->*constMotion)());
}

// TODO: merge with previous one ?
template<typename T>
inline TypeTreeBlock * motion(TypeTreeBlock *object,  const TypeTreeBlock * (TypeTreeBlock::*constMotion)(T) const, T arg) {
  return const_cast<TypeTreeBlock*>((const_cast<const TypeTreeBlock*>(object)->*constMotion)(arg));
}

class TypeTreeBlock : public TreeBlock {
public:
  constexpr TypeTreeBlock(BlockType content = BlockType::Integer) : TreeBlock(static_cast<uint8_t>(content)) {}

#if POINCARE_TREE_LOG
  void log(std::ostream & stream, bool recursive = true, int indentation = 0, bool verbose = true) const;
#endif

  void copyTo(TreeBlock * address) const;

  // Block Navigation
  const TypeTreeBlock * nextNode() const;
  TypeTreeBlock * nextNode() { return motion(this, &TypeTreeBlock::nextNode); };
  const TypeTreeBlock * previousNode(const TreeBlock * firstBlock) const;
  TypeTreeBlock * previousNode(const TreeBlock * firstBlock) { return motion(this, &TypeTreeBlock::previousNode, firstBlock);}
  const TypeTreeBlock * nextSibling() const;
  TypeTreeBlock * nextSibling() { return motion(this, &TypeTreeBlock::nextSibling); };
  const TypeTreeBlock * previousSibling(const TreeBlock * firstBlock) const;
  TypeTreeBlock * previousSibling(const TreeBlock * firstBlock) { return motion(this, &TypeTreeBlock::previousSibling, firstBlock);}

  // Sizes
  size_t treeSize() const { return nextSibling() - this; }

  // Node Hierarchy
  const TypeTreeBlock * parent(const TreeBlock * firstBlock) const;
  TypeTreeBlock * parent(const TreeBlock * firstBlock) { return motion(this, &TypeTreeBlock::parent, firstBlock);}
  const TypeTreeBlock * root(const TreeBlock * firstBlock) const;
  TypeTreeBlock * root(const TreeBlock * firstBlock) { return motion(this, &TypeTreeBlock::root, firstBlock);}
  int numberOfDescendants(bool includeSelf) const;
  const TypeTreeBlock * childAtIndex(int index) const;
  TypeTreeBlock * childAtIndex(int index) { return motion(this, &TypeTreeBlock::childAtIndex, index);}
  int indexOfChild(const TypeTreeBlock * child) const;
  int indexInParent(const TreeBlock * firstBlock) const;
  bool hasChild(const TypeTreeBlock * child) const;
  bool hasAncestor(const TreeBlock * firstBlock, const TypeTreeBlock * node, bool includeSelf) const;
  bool hasSibling(const TreeBlock * firstBlock, const TypeTreeBlock * e) const;

  // Properties
  #if GHOST_REQUIRED
  static constexpr GhostProperties k_ghostProperties;
#endif
  static constexpr IntegerProperties k_integerProperties;
  static constexpr AdditionProperties k_additionProperties;
  static constexpr MultiplicationProperties k_multiplicationProperties;
  static constexpr SubtractionProperties k_subtractionProperties;
  static constexpr DivisionProperties k_divisionProperties;
  static constexpr PowerProperties k_powerProperties;
  static constexpr ConstantProperties k_constantProperties;
  static constexpr const Properties * k_properties[] = {
  // Order has to be the same as TypeTreeBlock
#if GHOST_REQUIRED
    &k_ghostProperties,
#endif
    &k_integerProperties,
    &k_integerProperties,
    &k_integerProperties,
    &k_additionProperties,
    &k_multiplicationProperties,
    &k_subtractionProperties,
    &k_divisionProperties,
    &k_powerProperties,
    &k_constantProperties
  };

  constexpr const Properties * properties() const { return k_properties[m_content]; }
  constexpr size_t nodeSize(bool head = true) const { return properties()->nodeSize(this, head); }
  constexpr int numberOfChildren() const { return properties()->numberOfChildren(this); }

  // Virtuality
  constexpr const Handle * handle() const;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const;
  void logAttributes(std::ostream & stream) const;
#endif
  void basicReduction();

  // TODO: dynamic_cast-like that can check its is a subclass with m_content
  void beautify();

  class ForwardDirect final {
  public:
    ForwardDirect(TypeTreeBlock * block) : m_block(block) {}
    class Iterator {
    public:
      Iterator(const TypeTreeBlock * block, int index) : m_indexedBlock({.m_block = const_cast<TypeTreeBlock *>(block), .m_index = index}) {}
      IndexedTypeTreeBlock operator*() { return m_indexedBlock; }
      bool operator!=(const Iterator& it) const { return (m_indexedBlock.m_index != it.m_indexedBlock.m_index); }
      Iterator & operator++() {
        m_indexedBlock.m_block = m_indexedBlock.m_block->nextSibling();
        m_indexedBlock.m_index++;
        return *this;
      }
    private:
      IndexedTypeTreeBlock m_indexedBlock;
    };
    Iterator begin() const { return Iterator(m_block->nextNode(), 0); }
    Iterator end() const { return Iterator(nullptr, m_block->numberOfChildren()); }
  private:
    TypeTreeBlock * m_block;
  };

  class BackwardsDirect final {
  public:
    BackwardsDirect(TypeTreeBlock * block) : m_memoizer(block) {}

    // TODO: explain why memoizing
    class Iterator {
    public:

      class Memoizer {
      public:
        Memoizer(TypeTreeBlock * treeBlock);
        TypeTreeBlock * childAtIndex(int i);
        size_t numberOfChildren() { return m_numberOfChildren; }
      private:
        void memoizeUntilIndex(int i);
        TypeTreeBlock * m_block;
        // Memoization of children addresses in a RingBuffer
        constexpr static size_t k_maxNumberOfMemoizedSubtrees = 16;
        TypeTreeBlock * m_children[k_maxNumberOfMemoizedSubtrees];
        size_t m_firstMemoizedSubtreeIndex; // Index used for ring buffer
        size_t m_firstSubtreeIndex;
        size_t m_numberOfChildren;
      };

      Iterator(int childIndex, Memoizer * memoizer) :
        m_childIndex(childIndex),
        m_memoizer(memoizer) {}
      TypeTreeBlock * operator*() { return m_memoizer->childAtIndex(m_childIndex); }
      bool operator!=(const Iterator& it) const { return (m_childIndex != it.m_childIndex); }

      Iterator & operator++() {
        m_childIndex--;
        return *this;
      }
    private:
      int m_childIndex;
      mutable Memoizer * m_memoizer;
    };
    Iterator begin() const { return Iterator(m_memoizer.numberOfChildren() - 1, &m_memoizer); }
    Iterator end() const { return Iterator(-1, &m_memoizer); }
  private:
    mutable Iterator::Memoizer m_memoizer;
  };

  ForwardDirect directChildren() const { return ForwardDirect(const_cast<TypeTreeBlock *>(this)); }
  BackwardsDirect backwardsDirectChildren() const { return BackwardsDirect(const_cast<TypeTreeBlock *>(this)); }

  constexpr BlockType type() const { return static_cast<BlockType>(m_content); }

  // Recursive helper
  void recursivelyApply(InPlaceTreeFunction treeFunction);

private:
  const TypeTreeBlock * previousRelative(const TreeBlock * firstBlock, bool parent) const;
#if GHOST_REQUIRED
  bool isGhost() const { return type() == BlockType::Ghost; }
#endif
};

class ValueTreeBlock : public TreeBlock {
public:
  constexpr ValueTreeBlock(uint8_t value) : TreeBlock(value) {}
  uint8_t value() const { return m_content; }
  // This dirty cast is a workaround to stricter static_cast in constexprs
  constexpr operator TypeTreeBlock() { return TypeTreeBlock(static_cast<BlockType>(m_content)); }
};

static_assert(sizeof(TreeBlock) == 1);
static_assert(sizeof(ValueTreeBlock) == sizeof(TreeBlock));
static_assert(sizeof(TypeTreeBlock) == sizeof(TreeBlock));

// static ?
constexpr static TypeTreeBlock AdditionBlock = TypeTreeBlock(BlockType::Addition);
constexpr static TypeTreeBlock MultiplicationBlock = TypeTreeBlock(BlockType::Multiplication);
constexpr static TypeTreeBlock IntegerBlock = TypeTreeBlock(BlockType::Integer);
constexpr static TypeTreeBlock SubtractionBlock = TypeTreeBlock(BlockType::Subtraction);
constexpr static TypeTreeBlock DivisionBlock = TypeTreeBlock(BlockType::Division);
constexpr static TypeTreeBlock PowerBlock = TypeTreeBlock(BlockType::Power);
constexpr static TypeTreeBlock ConstantBlock = TypeTreeBlock(BlockType::Constant);

template <unsigned N>
struct TreeNode {
  // Using this instead of a TreeBlock[N] simplifies up casting in constexprs
  TypeTreeBlock blocks[N];
  constexpr operator const TypeTreeBlock*() const { return blocks; }
};

template<bool ChildrenCount, unsigned ...Len>
constexpr auto makeNary(const TypeTreeBlock type, const TreeNode<Len> (&...nodes)) {
  // Compute the total length of the children
  constexpr unsigned N = (... + Len);
  TreeNode<N + 1 + 2*ChildrenCount> result = {};
  result.blocks[0] = type;
  if (ChildrenCount) {
    result.blocks[1] = ValueTreeBlock(sizeof...(Len));
    result.blocks[2] = type;
  }

  TypeTreeBlock* dst = &result.blocks[1 + 2*ChildrenCount];
  for (const TypeTreeBlock * node : {static_cast<const TypeTreeBlock*>(nodes)...}) {
    int toCopy = 1;
    while (toCopy) {
      toCopy += node->numberOfChildren() - 1;
      const TypeTreeBlock * src = node;
      for (int size = node->nodeSize(); size>0; size--) {
	*dst++ = *src++;
      }
    }
  }
  return result;
}

}

#endif

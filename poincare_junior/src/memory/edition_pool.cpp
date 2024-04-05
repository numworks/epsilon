#include "edition_pool.h"

#include <assert.h>
#include <omgpj.h>
#include <poincare/junior_layout.h>
#include <poincare_junior/include/poincare.h>

#include <algorithm>

#include "exception_checkpoint.h"
#include "node_constructor.h"

namespace PoincareJ {

// Edition Pool

OMG::GlobalBox<TreeStack> TreeStack::SharedTreeStack;

size_t TreeStack::numberOfTrees() const {
  const Block *currentBlock = firstBlock();
  size_t result = 0;
  while (currentBlock < lastBlock()) {
    currentBlock = Tree::FromBlocks(currentBlock)->nextTree()->block();
    result++;
  }
  assert(currentBlock == lastBlock());
  return result;
}

bool TreeStack::isRootBlock(const Block *block, bool allowLast) const {
  const Block *currentBlock = firstBlock();
  if (block >= lastBlock()) {
    return allowLast && block == lastBlock();
  }
  while (currentBlock < block) {
    currentBlock = Tree::FromBlocks(currentBlock)->nextTree()->block();
  }
  return currentBlock == block;
}

Tree *TreeStack::initFromAddress(const void *address, bool isTree) {
  const Tree *node = Tree::FromBlocks(reinterpret_cast<const Block *>(address));
  size_t size = isTree ? node->treeSize() : node->nodeSize();
  Block *copiedTree = lastBlock();
  if (!insertBlocks(copiedTree, static_cast<const Block *>(address),
                    size * sizeof(Block), true)) {
    return nullptr;
  }
#if POINCARE_POOL_VISUALIZATION
  Log("Copy", copiedTree,
      isTree ? Tree::FromBlocks(copiedTree)->treeSize()
             : Tree::FromBlocks(copiedTree)->nodeSize());
#endif
  return Tree::FromBlocks(copiedTree);
}

template <>
Tree *TreeStack::push<Type::UserFunction>(const char *name) {
  return push<Type::UserFunction>(name, strlen(name) + 1);
}
template <>
Tree *TreeStack::push<Type::UserSequence>(const char *name) {
  return push<Type::UserSequence>(name, strlen(name) + 1);
}
template <>
Tree *TreeStack::push<Type::UserSymbol>(const char *name) {
  return push<Type::UserSymbol>(name, strlen(name) + 1);
}

template <Type blockType, typename... Types>
Tree *TreeStack::push(Types... args) {
  Block *newNode = lastBlock();

  size_t i = 0;
  bool endOfNode = false;
  do {
    Block block;
    endOfNode = NodeConstructor::CreateBlockAtIndexForType<blockType>(
        &block, i++, args...);
    push(block);
  } while (!endOfNode);
#if POINCARE_POOL_VISUALIZATION
  Log("Push", newNode, i);
#endif
  return Tree::FromBlocks(newNode);
}

void TreeStack::replaceBlock(Block *previousBlock, Block newBlock) {
  replaceBlocks(previousBlock, &newBlock, 1);
}

void TreeStack::replaceBlocks(Block *destination, const Block *source,
                              size_t numberOfBlocks) {
  memcpy(destination, source, numberOfBlocks * sizeof(Block));
  m_referenceTable.updateNodes(
      [](uint16_t *offset, Block *block, const Block *destination,
         const Block *source, int size) {
        if (block >= destination && block < destination + size) {
          *offset = ReferenceTable::InvalidatedOffset;
        }
      },
      destination, nullptr, numberOfBlocks);
}

bool TreeStack::insertBlocks(Block *destination, const Block *source,
                             size_t numberOfBlocks, bool at) {
  if (numberOfBlocks == 0) {
    return true;
  }
  if (m_size + numberOfBlocks > k_maxNumberOfBlocks) {
    ExceptionCheckpoint::Raise(ExceptionType::PoolIsFull);
  }
  size_t insertionSize = numberOfBlocks * sizeof(Block);
  if (at && destination == lastBlock()) {
    m_size += numberOfBlocks;
    memcpy(destination, source, insertionSize);
    return true;
  }
  size_t editionPoolRightSize = static_cast<Block *>(lastBlock()) - destination;
  memmove(destination + insertionSize, destination, editionPoolRightSize);
  if (source >= destination && source < destination + editionPoolRightSize) {
    // Source has been memmoved.
    source += insertionSize;
  }

  m_size += numberOfBlocks;
  memcpy(destination, source, insertionSize);
  m_referenceTable.updateNodes(
      [](uint16_t *offset, Block *block, const Block *destination,
         const Block *source, int size) {
        if (destination <= block) {
          *offset += size;
        }
      },
      destination + at, nullptr, numberOfBlocks);
  return true;
}

void TreeStack::removeBlocks(Block *address, size_t numberOfBlocks) {
  // If this assert triggers, add an escape case
  assert(numberOfBlocks != 0);
  int deletionSize = numberOfBlocks * sizeof(Block);
  assert(m_size >= numberOfBlocks);
  m_size -= numberOfBlocks;
  assert(static_cast<Block *>(lastBlock()) >= address);
  memmove(address, address + deletionSize,
          static_cast<Block *>(lastBlock()) - address);
  m_referenceTable.updateNodes(
      [](uint16_t *offset, Block *block, const Block *address,
         const Block *source, int size) {
        if (block >= address + size) {
          *offset -= size;
        } else if (block >= address) {
          *offset = ReferenceTable::InvalidatedOffset;
        }
      },
      address, nullptr, numberOfBlocks);
}

void TreeStack::moveBlocks(Block *destination, Block *source,
                           size_t numberOfBlocks, bool at) {
  if (destination == source || numberOfBlocks == 0) {
    return;
  }
  uint8_t *src = reinterpret_cast<uint8_t *>(source);
  uint8_t *dst = reinterpret_cast<uint8_t *>(destination);
  size_t len = numberOfBlocks * sizeof(Block);
  Memory::Rotate(dst, src, len);
  if (at) {
    m_referenceTable.updateNodes(
        [](uint16_t *offset, Block *block, const Block *dst, const Block *src,
           int size) {
          if (src <= block && block < src + size) {
            *offset += dst - src - (dst > src ? size : 0);
          } else if (src + size <= block && block <= dst) {
            *offset -= size;
          } else if (dst < block && block < src) {
            *offset += size;
          }
        },
        destination, source, numberOfBlocks);
  } else {
    m_referenceTable.updateNodes(
        [](uint16_t *offset, Block *block, const Block *dst, const Block *src,
           int size) {
          if (src <= block && block < src + size) {
            *offset += dst - src - (dst > src ? size : 0);
          } else if (src + size <= block && block < dst) {
            *offset -= size;
          } else if (dst <= block && block < src) {
            *offset += size;
          }
        },
        destination, source, numberOfBlocks);
  }
}

void TreeStack::flush() {
  m_size = 0;
  m_referenceTable.reset();
#if POINCARE_POOL_VISUALIZATION
  Log("Flush");
#endif
}

void TreeStack::flushFromBlock(const Block *block) {
  assert(isRootBlock(block, true));
  m_size = block - m_blocks;
  m_referenceTable.deleteIdentifiersAfterBlock(block);
#if POINCARE_POOL_VISUALIZATION
  Log("flushFromBlock", block);
#endif
}

uint16_t TreeStack::referenceNode(Tree *node) {
  return m_referenceTable.storeNode(node);
}

void TreeStack::executeAndStoreLayout(ActionWithContext action, void *context,
                                      const void *data,
                                      Poincare::JuniorLayout *layout,
                                      Relax relax) {
  assert(numberOfTrees() == 0);
  execute(action, context, data, k_maxNumberOfBlocks, relax);
  assert(Tree::FromBlocks(firstBlock())->isLayout());
  *layout = Poincare::JuniorLayout::Builder(Tree::FromBlocks(firstBlock()));
  flush();
}

void TreeStack::executeAndReplaceTree(ActionWithContext action, void *context,
                                      Tree *data, Relax relax) {
  Block *previousLastBlock = lastBlock();
  execute(action, context, data, k_maxNumberOfBlocks, relax);
  assert(previousLastBlock != lastBlock());
  data->moveTreeOverTree(Tree::FromBlocks(previousLastBlock));
}

#if POINCARE_TREE_LOG

void TreeStack::logNode(std::ostream &stream, const Tree *node, bool recursive,
                        bool verbose, int indentation) {
  Indent(stream, indentation);
  stream << "<Reference id=\"";
  m_referenceTable.logIdsForNode(stream, node);
  stream << "\">\n";
  node->log(stream, recursive, verbose, indentation + 1);
  Indent(stream, indentation);
  stream << "</Reference>" << std::endl;
}

void TreeStack::log(std::ostream &stream, LogFormat format, bool verbose,
                    int indentation) {
  const char *formatName = format == LogFormat::Tree ? "tree" : "flat";
  Indent(stream, indentation);
  stream << "<TreeStack format=\"" << formatName << "\" size=\"" << size()
         << "\">\n";
  if (format == LogFormat::Tree) {
    for (const Tree *tree : trees()) {
      logNode(stream, tree, true, verbose, indentation + 1);
    }
  } else {
    for (const Tree *tree : allNodes()) {
      logNode(stream, tree, false, verbose, indentation + 1);
    }
  }
  Indent(stream, indentation);
  stream << "</TreeStack>" << std::endl;
}

#endif

void TreeStack::execute(ActionWithContext action, void *context,
                        const void *data, int maxSize, Relax relax) {
#if ASSERTIONS
  size_t treesNumber = numberOfTrees();
#endif
  size_t previousSize = size();
  while (true) {
    ExceptionTry {
      assert(numberOfTrees() == treesNumber);
      action(context, data);
      // Prevent edition action from leaking: an action create at most one tree.
      assert(numberOfTrees() <= treesNumber + 1);
      // Ensure the result tree doesn't exceeds the expected size.
      if (size() - previousSize > maxSize) {
        ExceptionCheckpoint::Raise(ExceptionType::RelaxContext);
      }
      return;
    }
    ExceptionCatch(type) {
      assert(numberOfTrees() == treesNumber);
      switch (type) {
        case ExceptionType::PoolIsFull:
        case ExceptionType::IntegerOverflow:
        case ExceptionType::RelaxContext:
          if (relax(context)) {
            continue;
          }
        default:
          ExceptionCheckpoint::Raise(type);
      }
    }
  }
}

// ReferenceTable

Tree *TreeStack::ReferenceTable::nodeForIdentifier(uint16_t id) const {
  if (id == NoNodeIdentifier) {
    return nullptr;
  }
  assert(id < m_length);
  uint16_t offset = m_nodeOffsetForIdentifier[id];
  if (offset == InvalidatedOffset) {
    return nullptr;
  }
  Tree *n = Tree::FromBlocks(m_pool->referenceBlock() + offset);
  if (!m_pool->contains(n->block()) && n->block() != m_pool->lastBlock()) {
    /* The node has been corrupted, this is not referenced anymore. Referencing
     * the last block is tolerated though. */
    return nullptr;
  }
  return n;
}

uint16_t TreeStack::ReferenceTable::storeNode(Tree *node) {
  if (isFull()) {
    Tree *n;
    size_t index = 0;
    do {
      n = nodeForIdentifier(index++);
    } while (n && index < k_maxNumberOfReferences);
    assert(!n);  // Otherwise, the pool is full with non-corrupted
                 // references; increment k_maxNumberOfReferences?
    return storeNodeAtIndex(node, index - 1);
  } else {
    return storeNodeAtIndex(node, m_length);
  }
}

void TreeStack::ReferenceTable::updateIdentifier(uint16_t id, Tree *newNode) {
  assert(id < m_length);
  storeNodeAtIndex(newNode, id);
}

void TreeStack::ReferenceTable::deleteIdentifier(uint16_t id) {
  if (id == TreeStack::ReferenceTable::NoNodeIdentifier) {
    return;
  }
  assert(id < m_length);
  if (id == m_length - 1) {
    do {
      m_length--;
    } while (m_length > 0 &&
             m_nodeOffsetForIdentifier[m_length - 1] == DeletedOffset);
  } else {
    // Mark the offset with a special tag until we can reduce length
    m_nodeOffsetForIdentifier[id] = DeletedOffset;
  }
}

void TreeStack::ReferenceTable::updateNodes(AlterSelectedBlock function,
                                            const Block *contextSelection1,
                                            const Block *contextSelection2,
                                            int contextAlteration) {
  Block *first = static_cast<Block *>(m_pool->firstBlock());
  for (int i = 0; i < m_length; i++) {
    if (m_nodeOffsetForIdentifier[i] == InvalidatedOffset ||
        m_nodeOffsetForIdentifier[i] == DeletedOffset) {
      continue;
    }
    function(&m_nodeOffsetForIdentifier[i],
             m_nodeOffsetForIdentifier[i] + first, contextSelection1,
             contextSelection2, contextAlteration);
  }
}

void TreeStack::ReferenceTable::deleteIdentifiersAfterBlock(
    const Block *block) {
  Block *first = static_cast<Block *>(m_pool->firstBlock());
  assert(block >= first && block <= first + UINT16_MAX);
  uint16_t maxOffset = block - first;
  for (int i = 0; i < m_length; i++) {
    if (m_nodeOffsetForIdentifier[i] != DeletedOffset &&
        m_nodeOffsetForIdentifier[i] >= maxOffset) {
      deleteIdentifier(i);
    }
  }
}

bool TreeStack::ReferenceTable::reset() {
  if (m_length == 0) {
    // We can't reset an empty table
    return false;
  }
  m_length = 0;
  return true;
}

#if POINCARE_TREE_LOG

void TreeStack::ReferenceTable::logIdsForNode(std::ostream &stream,
                                              const Tree *node) const {
  bool found = false;
  for (size_t i = 0; i < m_length; i++) {
    Tree *n = TreeStack::ReferenceTable::nodeForIdentifier(i);
    if (node == n) {
      stream << static_cast<int>(i) << ", ";
      found = true;
    }
  }
  if (found == false) {
    stream << "ø";
  }
}

#endif

uint16_t TreeStack::ReferenceTable::storeNodeAtIndex(Tree *node, size_t index) {
  if (index >= m_length) {
    assert(index == m_length);
    assert(!isFull());
    // Increment first to make firstBlock != nullptr
    m_length++;
  }
  m_nodeOffsetForIdentifier[index] =
      static_cast<uint16_t>(node->block() - m_pool->referenceBlock());
  // Assertion requires valid firstBlock/lastBlock (so the order matters)
  assert(m_pool->contains(node->block()) ||
         node->block() == m_pool->lastBlock());
  return index;
}

// Edition Pool

template Tree *TreeStack::push<Type::Addition, int>(int);
template Tree *TreeStack::push<Type::AsciiCodePointLayout, CodePoint>(
    CodePoint);
template Tree *TreeStack::push<Type::CombinedCodePointsLayout, CodePoint,
                               CodePoint>(CodePoint, CodePoint);
template Tree *TreeStack::push<Type::Decimal, int8_t>(int8_t);
template Tree *TreeStack::push<Type::DoubleFloat, double>(double);
template Tree *TreeStack::push<Type::IntegerNegBig>(uint64_t);
template Tree *TreeStack::push<Type::IntegerPosBig>(uint64_t);
template Tree *TreeStack::push<Type::IntegerShort>(int8_t);
template Tree *TreeStack::push<Type::List, int>(int);
template Tree *TreeStack::push<Type::Matrix, int, int>(int, int);
template Tree *TreeStack::push<Type::Matrix, uint8_t, uint8_t>(uint8_t,
                                                               uint8_t);
template Tree *TreeStack::push<Type::MatrixLayout, uint8_t, uint8_t>(uint8_t,
                                                                     uint8_t);
template Tree *TreeStack::push<Type::Multiplication, int>(int);
template Tree *TreeStack::push<Type::ParenthesisLayout, bool, bool>(
    bool leftIsTemporary, bool rightIsTemporary);
template Tree *TreeStack::push<Type::PhysicalConstant, uint8_t>(uint8_t);
template Tree *TreeStack::push<Type::Piecewise, int>(int);
template Tree *TreeStack::push<Type::PointOfInterest, double, double, uint32_t,
                               uint8_t, bool, uint8_t>(double, double, uint32_t,
                                                       uint8_t, bool, uint8_t);
template Tree *TreeStack::push<Type::Polynomial, int>(int);
template Tree *TreeStack::push<Type::RackLayout, int>(int);
template Tree *TreeStack::push<Type::Set>(int);
template Tree *TreeStack::push<Type::Set>(uint8_t);
template Tree *TreeStack::push<Type::SingleFloat, float>(float);
template Tree *TreeStack::push<Type::UnicodeCodePointLayout, CodePoint>(
    CodePoint);
template Tree *TreeStack::push<Type::Unit, uint8_t, uint8_t>(uint8_t, uint8_t);
template Tree *TreeStack::push<Type::UserFunction, const char *, size_t>(
    const char *, size_t);
template Tree *TreeStack::push<Type::UserSequence, const char *, size_t>(
    const char *, size_t);
template Tree *TreeStack::push<Type::UserSymbol, const char *, size_t>(
    const char *, size_t);
template Tree *TreeStack::push<Type::Variable, uint8_t, ComplexSign>(
    uint8_t, ComplexSign);
template Tree *TreeStack::push<Type::VerticalOffsetLayout, bool, bool>(
    bool isSubscript, bool isPrefix);

}  // namespace PoincareJ

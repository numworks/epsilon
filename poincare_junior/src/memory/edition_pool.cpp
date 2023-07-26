#include "edition_pool.h"

#include <assert.h>
#include <omgpj.h>
#include <poincare_junior/include/poincare.h>

#include <algorithm>

#include "cache_pool.h"
#include "exception_checkpoint.h"
#include "node_constructor.h"

namespace PoincareJ {

// ReferenceTable

EditionPool *const SharedEditionPool =
    CachePool::sharedCachePool()->editionPool();

Tree *EditionPool::ReferenceTable::nodeForIdentifier(uint16_t id) const {
  Tree *n = Pool::ReferenceTable::nodeForIdentifier(id);
  if (!m_pool->contains(n->block()) && n->block() != m_pool->lastBlock()) {
    /* The node has been corrupted, this is not referenced anymore. Referencing
     * the last block is tolerated though. */
    return nullptr;
  }
  return n;
}

uint16_t EditionPool::ReferenceTable::storeNode(Tree *node) {
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
};

void EditionPool::ReferenceTable::updateNodes(AlterSelectedBlock function,
                                              const Block *contextSelection1,
                                              const Block *contextSelection2,
                                              int contextAlteration) {
  Block *first = static_cast<Block *>(m_pool->firstBlock());
  for (int i = 0; i < m_length; i++) {
    if (m_nodeOffsetForIdentifier[i] == UninitializedOffset) {
      continue;
    }
    function(&m_nodeOffsetForIdentifier[i],
             m_nodeOffsetForIdentifier[i] + first, contextSelection1,
             contextSelection2, contextAlteration);
  }
}

// EditionTable

void EditionPool::reinit(TypeBlock *firstBlock, size_t size) {
  m_firstBlock = firstBlock;
  m_size = size;
}

uint16_t EditionPool::referenceNode(Tree *node) {
  return m_referenceTable.storeNode(node);
}

void EditionPool::flush() {
  m_numberOfBlocks = 0;
  m_referenceTable.reset();
#if POINCARE_POOL_VISUALIZATION
  Log(LoggerType::Edition, "Flush");
#endif
}

bool EditionPool::executeAndDump(ActionWithContext action, void *context,
                                 const void *data, void *address, int maxSize,
                                 Relax relax) {
  if (!execute(action, context, data, maxSize, relax)) {
    return false;
  }
  assert(Tree::FromBlocks(firstBlock())->treeSize() <= maxSize);
  Tree::FromBlocks(firstBlock())->copyTreeTo(address);
  flush();
  return true;
}

uint16_t EditionPool::executeAndCache(ActionWithContext action, void *context,
                                      const void *data, Relax relax) {
  execute(action, context, data, CachePool::k_maxNumberOfBlocks, relax);
  /* If execute failed, storeEditedTree will handle an empty EditionPool and
   * return a ReferenceTable::NoNodeIdentifier. */
  return CachePool::sharedCachePool()->storeEditedTree();
}

void EditionPool::replaceBlock(Block *previousBlock, Block newBlock) {
  replaceBlocks(previousBlock, &newBlock, 1);
}

void EditionPool::replaceBlocks(Block *destination, const Block *source,
                                size_t numberOfBlocks) {
  memcpy(destination, source, numberOfBlocks * sizeof(Block));
  m_referenceTable.updateNodes(
      [](uint16_t *offset, Block *block, const Block *destination,
         const Block *source, int size) {
        if (block >= destination && block < destination + size) {
          *offset = ReferenceTable::UninitializedOffset;
        }
      },
      destination, nullptr, numberOfBlocks);
}

bool EditionPool::insertBlocks(Block *destination, const Block *source,
                               size_t numberOfBlocks, bool at) {
  if (destination == source || numberOfBlocks == 0) {
    return true;
  }
  if (!checkForEnoughSpace(numberOfBlocks)) {
    return false;
  }
  size_t insertionSize = numberOfBlocks * sizeof(Block);
  if (at && destination == lastBlock()) {
    m_numberOfBlocks += numberOfBlocks;
    memcpy(destination, source, insertionSize);
    return true;
  }
  memmove(destination + insertionSize, destination,
          static_cast<Block *>(lastBlock()) - destination);
  m_numberOfBlocks += numberOfBlocks;
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

void EditionPool::removeBlocks(Block *address, size_t numberOfBlocks) {
  // If this assert triggers, add an escape case
  assert(numberOfBlocks != 0);
  int deletionSize = numberOfBlocks * sizeof(Block);
  assert(m_numberOfBlocks >= numberOfBlocks);
  m_numberOfBlocks -= numberOfBlocks;
  memmove(address, address + deletionSize,
          static_cast<Block *>(lastBlock()) - address);
  m_referenceTable.updateNodes(
      [](uint16_t *offset, Block *block, const Block *address,
         const Block *source, int size) {
        if (block >= address + size) {
          *offset -= size;
        } else if (block >= address) {
          *offset = ReferenceTable::UninitializedOffset;
        }
      },
      address, nullptr, numberOfBlocks);
}

void EditionPool::moveBlocks(Block *destination, Block *source,
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

Tree *EditionPool::initFromAddress(const void *address, bool isTree) {
  const Tree *node =
      Tree::FromBlocks(reinterpret_cast<const TypeBlock *>(address));
  size_t size = isTree ? node->treeSize() : node->nodeSize();
  TypeBlock *copiedTree = lastBlock();
  if (!insertBlocks(copiedTree, static_cast<const Block *>(address),
                    size * sizeof(Block))) {
    return nullptr;
  }
#if POINCARE_POOL_VISUALIZATION
  Log(LoggerType::Edition, "Copy", copiedTree,
      isTree ? Tree::FromBlocks(copiedTree)->treeSize()
             : Tree::FromBlocks(copiedTree)->nodeSize());
#endif
  return Tree::FromBlocks(copiedTree);
}

bool EditionPool::execute(ActionWithContext action, void *context,
                          const void *data, int maxSize, Relax relax) {
  ExceptionCheckpoint checkpoint;
start_execute:
  if (ExceptionRun(checkpoint)) {
    assert(numberOfTrees() == 0);
    action(context, data);
    // Prevent edition action from leaking: an action create at most one tree
    assert(numberOfTrees() <= 1);
  } else {
    /* TODO: assert that we don't delete last called treeForIdentifier otherwise
     * can't copyTreeFromAddress if in cache... */
    int size = fullSize();
    /* Free blocks and try again. If no more blocks can be freed, try relaxing
     * the context and try again. Otherwise, return false. */
    if ((size >= maxSize || !CachePool::sharedCachePool()->freeBlocks(
                                std::min(size, maxSize - size))) &&
        !relax(context)) {
      return false;
    }
    goto start_execute;
  }
  // Action has been successfully executed
  return true;
}

template <BlockType blockType, typename... Types>
Tree *EditionPool::push(Types... args) {
  TypeBlock *newNode = lastBlock();

  size_t i = 0;
  bool endOfNode = false;
  do {
    Block block;
    endOfNode = NodeConstructor::CreateBlockAtIndexForType<blockType>(
        &block, i++, args...);
    pushBlock(block);
  } while (!endOfNode);
#if POINCARE_POOL_VISUALIZATION
  Log(LoggerType::Edition, "Push", newNode, i);
#endif
  return Tree::FromBlocks(newNode);
}

bool EditionPool::checkForEnoughSpace(size_t numberOfRequiredBlock) {
  if (m_numberOfBlocks + numberOfRequiredBlock > m_size) {
    ExceptionCheckpoint::Raise();
    return false;
  }
  return true;
}

template Tree *EditionPool::push<BlockType::Addition, int>(int);
template Tree *EditionPool::push<BlockType::Multiplication, int>(int);
template Tree *EditionPool::push<BlockType::Constant, char16_t>(char16_t);
template Tree *EditionPool::push<BlockType::Power>();
template Tree *EditionPool::push<BlockType::Factorial>();
template Tree *EditionPool::push<BlockType::SquareRoot>();
template Tree *EditionPool::push<BlockType::Subtraction>();
template Tree *EditionPool::push<BlockType::Division>();
template Tree *EditionPool::push<BlockType::IntegerShort>(int8_t);
template Tree *EditionPool::push<BlockType::IntegerPosBig>(uint64_t);
template Tree *EditionPool::push<BlockType::IntegerNegBig>(uint64_t);
template Tree *EditionPool::push<BlockType::Float, float>(float);
template Tree *EditionPool::push<BlockType::MinusOne>();
template Tree *EditionPool::push<BlockType::Set>(int);
template Tree *EditionPool::push<BlockType::Half>();
template Tree *EditionPool::push<BlockType::Zero>();
template Tree *EditionPool::push<BlockType::One>();
template Tree *EditionPool::push<BlockType::Two>();
template Tree *EditionPool::push<BlockType::RationalShort>(int8_t, uint8_t);
template Tree *EditionPool::push<BlockType::Polynomial, int, int>(int, int);
template Tree *EditionPool::push<BlockType::UserSymbol, const char *, size_t>(
    const char *, size_t);
template Tree *EditionPool::push<BlockType::Derivative>();
template Tree *EditionPool::push<BlockType::RackLayout, int>(int);
template Tree *EditionPool::push<BlockType::SystemList, int>(int);
template Tree *EditionPool::push<BlockType::FractionLayout>();
template Tree *EditionPool::push<BlockType::ParenthesisLayout>();
template Tree *EditionPool::push<BlockType::VerticalOffsetLayout>();
template Tree *EditionPool::push<BlockType::CodePointLayout, CodePoint>(
    CodePoint);

}  // namespace PoincareJ

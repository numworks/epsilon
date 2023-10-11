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

#if PLATFORM_DEVICE
EditionPool *const volatile SharedEditionPool = nullptr;
#else
EditionPool *const volatile SharedEditionPool =
    CachePool::sharedCachePool()->editionPool();
#endif

void EditionPool::InitSharedEditionPool() {
  // TODO is this a defined behavior ?
  EditionPool **pool = const_cast<EditionPool **>(&SharedEditionPool);
  *pool = CachePool::sharedCachePool()->editionPool();
}

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

void EditionPool::reinit(Block *firstBlock, size_t size) {
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

void EditionPool::executeAndDump(ActionWithContext action, void *context,
                                 const void *data, void *address, int maxSize,
                                 Relax relax) {
  execute(action, context, data, maxSize, relax);
  assert(Tree::FromBlocks(firstBlock())->treeSize() <= maxSize);
  Tree::FromBlocks(firstBlock())->copyTreeTo(address);
  flush();
}

uint16_t EditionPool::executeAndCache(ActionWithContext action, void *context,
                                      const void *data, Relax relax) {
  execute(action, context, data, CachePool::k_maxNumberOfBlocks, relax);
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
  size_t editionPoolRightSize = static_cast<Block *>(lastBlock()) - destination;
  memmove(destination + insertionSize, destination, editionPoolRightSize);
  if (source >= destination && source < destination + editionPoolRightSize) {
    // Source has been memmoved.
    source += insertionSize;
  }

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
  const Tree *node = Tree::FromBlocks(reinterpret_cast<const Block *>(address));
  size_t size = isTree ? node->treeSize() : node->nodeSize();
  Block *copiedTree = lastBlock();
  if (!insertBlocks(copiedTree, static_cast<const Block *>(address),
                    size * sizeof(Block), true)) {
    return nullptr;
  }
#if POINCARE_POOL_VISUALIZATION
  Log(LoggerType::Edition, "Copy", copiedTree,
      isTree ? Tree::FromBlocks(copiedTree)->treeSize()
             : Tree::FromBlocks(copiedTree)->nodeSize());
#endif
  return Tree::FromBlocks(copiedTree);
}

void EditionPool::execute(ActionWithContext action, void *context,
                          const void *data, int maxSize, Relax relax) {
  while (true) {
    ExceptionTry {
      assert(numberOfTrees() == 0);
      action(context, data);
      // Prevent edition action from leaking: an action create at most one tree.
      assert(numberOfTrees() <= 1);
      return;
    }
    ExceptionCatch(type) {
      if (type != ExceptionType::PoolIsFull) {
        ExceptionCheckpoint::Raise(type);
      }
      /* TODO: assert that we don't delete last called treeForIdentifier
       * otherwise can't copyTreeFromAddress if in cache... */
      int size = fullSize();
      // Free blocks and try again.
      if ((size >= maxSize || !CachePool::sharedCachePool()->freeBlocks(
                                  std::min(size, maxSize - size))) &&
          !relax(context)) {
        /* TODO: If no more blocks can be freed, try relaxing the context and
         * try again. Otherwise, raise again. */
        ExceptionCheckpoint::Raise(type);
      }
    }
  }
}

template <BlockType blockType, typename... Types>
Tree *EditionPool::push(Types... args) {
  Block *newNode = lastBlock();

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
    ExceptionCheckpoint::Raise(ExceptionType::PoolIsFull);
    return false;
  }
  return true;
}

template Tree *EditionPool::push<BlockType::Addition, int>(int);
template Tree *EditionPool::push<BlockType::Multiplication, int>(int);
template Tree *EditionPool::push<BlockType::Constant, char16_t>(char16_t);
template Tree *EditionPool::push<BlockType::Power>();
template Tree *EditionPool::push<BlockType::Abs>();
template Tree *EditionPool::push<BlockType::Factorial>();
template Tree *EditionPool::push<BlockType::Complex>();
template Tree *EditionPool::push<BlockType::SquareRoot>();
template Tree *EditionPool::push<BlockType::Subtraction>();
template Tree *EditionPool::push<BlockType::Division>();
template Tree *EditionPool::push<BlockType::Exponential>();
template Tree *EditionPool::push<BlockType::Ln>();
template Tree *EditionPool::push<BlockType::RealPart>();
template Tree *EditionPool::push<BlockType::ImaginaryPart>();
template Tree *EditionPool::push<BlockType::ComplexArgument>();
template Tree *EditionPool::push<BlockType::Conjugate>();
template Tree *EditionPool::push<BlockType::IntegerShort>(int8_t);
template Tree *EditionPool::push<BlockType::IntegerPosBig>(uint64_t);
template Tree *EditionPool::push<BlockType::IntegerNegBig>(uint64_t);
template Tree *EditionPool::push<BlockType::Float, float>(float);
template Tree *EditionPool::push<BlockType::Decimal, uint8_t>(uint8_t);
template Tree *EditionPool::push<BlockType::MinusOne>();
template Tree *EditionPool::push<BlockType::Half>();
template Tree *EditionPool::push<BlockType::Zero>();
template Tree *EditionPool::push<BlockType::One>();
template Tree *EditionPool::push<BlockType::Two>();
template Tree *EditionPool::push<BlockType::RationalShort>(int8_t, uint8_t);
template Tree *EditionPool::push<BlockType::Unit, uint8_t, uint8_t>(uint8_t,
                                                                    uint8_t);
template Tree *EditionPool::push<BlockType::Matrix, uint8_t, uint8_t>(uint8_t,
                                                                      uint8_t);
template Tree *EditionPool::push<BlockType::Matrix, int, int>(int, int);
template Tree *EditionPool::push<BlockType::Set>(int);
template Tree *EditionPool::push<BlockType::Set>(uint8_t);
template Tree *EditionPool::push<BlockType::Polynomial, int>(int);
template Tree *EditionPool::push<BlockType::UserSymbol, const char *, size_t>(
    const char *, size_t);
template Tree *EditionPool::push<BlockType::Variable>(uint8_t);
template Tree *EditionPool::push<BlockType::Derivative>();
template Tree *EditionPool::push<BlockType::RackLayout, int>(int);
template Tree *EditionPool::push<BlockType::SystemList, int>(int);
template Tree *EditionPool::push<BlockType::FractionLayout>();
template Tree *EditionPool::push<BlockType::ParenthesisLayout>();
template Tree *EditionPool::push<BlockType::VerticalOffsetLayout>();
template Tree *EditionPool::push<BlockType::CodePointLayout, CodePoint>(
    CodePoint);

}  // namespace PoincareJ

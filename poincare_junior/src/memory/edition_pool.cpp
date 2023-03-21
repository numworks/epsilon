#include "edition_pool.h"

#include <assert.h>
#include <omgpj.h>
#include <poincare_junior/include/poincare.h>

#include <algorithm>

#include "cache_pool.h"
#include "exception_checkpoint.h"

namespace PoincareJ {

// ReferenceTable

Node EditionPool::ReferenceTable::nodeForIdentifier(uint16_t id) const {
  Node n = Pool::ReferenceTable::nodeForIdentifier(id);
  if (!m_pool->contains(n.block()) && n.block() != m_pool->lastBlock()) {
    /* The node has been corrupted, this is not referenced anymore. Referencing
     * the last block is tolerated though. */
    return Node();
  }
  return n;
}

uint16_t EditionPool::ReferenceTable::storeNode(Node node) {
  if (isFull()) {
    Node n;
    size_t index = 0;
    do {
      n = nodeForIdentifier(index++);
    } while (!n.isUninitialized() && index < k_maxNumberOfReferences);
    assert(
        n.isUninitialized());  // Otherwise, the pool is full with non-corrupted
                               // references; increment k_maxNumberOfReferences?
    return storeNodeAtIndex(node, index - 1);
  } else {
    return storeNodeAtIndex(node, m_length);
  }
};

void EditionPool::ReferenceTable::updateNodes(AlterSelectedBlock function,
                                              Block *contextSelection1,
                                              Block *contextSelection2,
                                              int contextAlteration) {
  Block *first = static_cast<Block *>(m_pool->firstBlock());
  for (int i = 0; i < m_length; i++) {
    if (m_nodeOffsetForIdentifier[i] == NoNodeIdentifier) {
      continue;
    }
    function(&m_nodeOffsetForIdentifier[i],
             m_nodeOffsetForIdentifier[i] + first, contextSelection1,
             contextSelection2, contextAlteration);
  }
}

// EditionTable

EditionPool *EditionPool::sharedEditionPool() {
  return CachePool::sharedCachePool()->editionPool();
}

void EditionPool::reinit(TypeBlock *firstBlock, size_t size) {
  m_firstBlock = firstBlock;
  m_size = size;
}

uint16_t EditionPool::referenceNode(Node node) {
  return m_referenceTable.storeNode(node);
}

void EditionPool::flush() {
  m_numberOfBlocks = 0;
  m_referenceTable.reset();
#if POINCARE_POOL_VISUALIZATION
  Logger(LoggerType::Edition) << "  <Flush>\n";
  CachePool::sharedCachePool()->log(Logger(LoggerType::Edition),
                                    Pool::LogFormat::Tree, true, 2);
  Logger(LoggerType::Edition) << "\n  </Flush>" << std::endl;
#endif
}

bool EditionPool::executeAndDump(ActionWithContext action, void *context,
                                 const void *data, void *address, int maxSize,
                                 Relax relax) {
  if (!execute(action, context, data, maxSize, relax)) {
    return false;
  }
  assert(Node(firstBlock()).treeSize() <= maxSize);
  Node(firstBlock()).copyTreeTo(address);
  flush();
  return true;
}

int EditionPool::executeAndCache(ActionWithContext action, void *context,
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
#if POINCARE_POOL_VISUALIZATION
  Logger(LoggerType::Edition) << "  <Replace address=\"" << destination
                              << "\" size=\"" << numberOfBlocks << "\">\n";
  CachePool::sharedCachePool()->log(Logger(LoggerType::Edition),
                                    Pool::LogFormat::Tree, true, 2);
  Logger(LoggerType::Edition) << "\n  </Replace>" << std::endl;
#endif
}

bool EditionPool::insertBlocks(Block *destination, Block *source,
                               size_t numberOfBlocks) {
  if (!checkForEnoughSpace(numberOfBlocks)) {
    return false;
  }
  size_t insertionSize = numberOfBlocks * sizeof(Block);
  memmove(destination + insertionSize, destination,
          static_cast<Block *>(lastBlock()) - destination);
  m_numberOfBlocks += numberOfBlocks;
  memcpy(destination, source, insertionSize);
  m_referenceTable.updateNodes(
      [](uint16_t *offset, Block *testedBlock, Block *destination, Block *block,
         int numberOfBlocks) {
        if (destination <= testedBlock) {
          *offset += numberOfBlocks;
        }
      },
      destination, nullptr, numberOfBlocks);
#if POINCARE_POOL_VISUALIZATION
  Logger(LoggerType::Edition) << "  <Insert address=\"" << destination
                              << "\" size=\"" << numberOfBlocks << "\">\n";
  CachePool::sharedCachePool()->log(Logger(LoggerType::Edition),
                                    Pool::LogFormat::Tree, true, 2);
  Logger(LoggerType::Edition) << "\n  </Insert>" << std::endl;
#endif
  return true;
}

void EditionPool::removeBlocks(Block *address, size_t numberOfBlocks) {
  assert(m_numberOfBlocks >= numberOfBlocks);
  int deletionSize = numberOfBlocks * sizeof(Block);
  m_numberOfBlocks -= numberOfBlocks;
  memmove(address, address + deletionSize,
          static_cast<Block *>(lastBlock()) - address);
  m_referenceTable.updateNodes(
      [](uint16_t *offset, Block *testedBlock, Block *address, Block *block,
         int numberOfBlocks) {
        if (testedBlock > address) {
          *offset -= numberOfBlocks;
        } else if (testedBlock == address) {
          *offset = ReferenceTable::NoNodeIdentifier;
        }
      },
      address, nullptr, numberOfBlocks);
#if POINCARE_POOL_VISUALIZATION
  Logger(LoggerType::Edition) << "  <Remove address=\"" << address
                              << "\" size=\"" << numberOfBlocks << "\">\n";
  CachePool::sharedCachePool()->log(Logger(LoggerType::Edition),
                                    Pool::LogFormat::Tree, true, 2);
  Logger(LoggerType::Edition) << "\n  </Remove>" << std::endl;
#endif
}

void EditionPool::moveBlocks(Block *destination, Block *source,
                             size_t numberOfBlocks) {
  uint8_t *src = reinterpret_cast<uint8_t *>(source);
  uint8_t *dst = reinterpret_cast<uint8_t *>(destination);
  size_t len = numberOfBlocks * sizeof(Block);
  Memory::Rotate(dst, src, len);
  m_referenceTable.updateNodes(
      [](uint16_t *offset, Block *testedBlock, Block *dst, Block *src,
         int nbOfBlocks) {
        if (testedBlock >= src && testedBlock < src + nbOfBlocks) {
          *offset += dst - src - (dst > src ? nbOfBlocks : 0);
        } else if ((testedBlock >= src + nbOfBlocks && testedBlock < dst) ||
                   (testedBlock >= dst && testedBlock < src)) {
          *offset += dst > src ? -nbOfBlocks : nbOfBlocks;
        }
      },
      destination, source, numberOfBlocks);
#if POINCARE_POOL_VISUALIZATION
  Logger(LoggerType::Edition) << "  <Move address=\"" << destination
                              << "\" size=\"" << numberOfBlocks << "\">\n";
  CachePool::sharedCachePool()->log(Logger(LoggerType::Edition),
                                    Pool::LogFormat::Tree, true, 2);
  Logger(LoggerType::Edition) << "\n  </Move>" << std::endl;
#endif
}

Node EditionPool::initFromAddress(const void *address, bool isTree) {
  Node node = Node(reinterpret_cast<const TypeBlock *>(address));
  size_t size = isTree ? node.treeSize() : node.nodeSize();
  if (!checkForEnoughSpace(size)) {
    return Node();
  }
  TypeBlock *copiedTree = lastBlock();
  m_numberOfBlocks += size;
  replaceBlocks(copiedTree, static_cast<const Block *>(address),
                size * sizeof(Block));
  return Node(copiedTree);
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

bool EditionPool::checkForEnoughSpace(size_t numberOfRequiredBlock) {
  if (m_numberOfBlocks + numberOfRequiredBlock > m_size) {
    ExceptionCheckpoint::Raise();
    return false;
  }
  return true;
}

}  // namespace PoincareJ

#include <assert.h>
#include "cache_pool.h"
#include "edition_pool.h"
#include "exception_checkpoint.h"
#include <omgpj.h>

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
    assert(n.isUninitialized()); // Otherwise, the pool is full with non-corrupted references; increment k_maxNumberOfReferences?
    return storeNodeAtIndex(node, index - 1);
  } else {
    return storeNodeAtIndex(node, m_length);
  }
};

void EditionPool::reinit(TypeBlock * firstBlock, size_t size) {
  m_firstBlock = firstBlock;
  m_size = size;
}

void EditionPool::ReferenceTable::updateNodes(AlterSelectedBlock function, Block * contextSelection1, Block * contextSelection2, int contextAlteration) {
  Block * first = static_cast<Block *>(m_pool->firstBlock());
  for (int i = 0; i < m_length; i++) {
    if (m_nodeOffsetForIdentifier[i] == NoNodeIdentifier) {
      continue;
    }
    function(&m_nodeOffsetForIdentifier[i], m_nodeOffsetForIdentifier[i] + first, contextSelection1, contextSelection2, contextAlteration);
  }
}

// EditionTable

EditionPool * EditionPool::sharedEditionPool() {
  return CachePool::sharedCachePool()->editionPool();
}

uint16_t EditionPool::referenceNode(Node node) {
  return m_referenceTable.storeNode(node);
}

void EditionPool::flush() {
  m_numberOfBlocks = 0;
  m_referenceTable.reset();
}

bool EditionPool::execute(ActionWithContext action, void * subAction, const void * data, void * address) {
  ExceptionCheckpoint checkpoint;
start_execute:
  if (ExceptionRun(checkpoint)) {
    assert(numberOfTrees() == 0);
    action(subAction, data);
    // Prevent edition action from leaking: an action create at most one tree
    assert(numberOfTrees() <= 1);
    Node(firstBlock()).copyTreeTo(address);
    flush();
    return true;
  } else {
    if (!CachePool::sharedCachePool()->freeBlocks(fullSize())) {
      return false;
    }
    goto start_execute;
  }
}

Block * EditionPool::pushBlock(Block block) {
  if (!checkForEnoughSpace(1)) {
    return nullptr;
  }
  assert(m_numberOfBlocks < m_size);
  *static_cast<Block *>(lastBlock()) = block;
  m_numberOfBlocks++;
  return lastBlock() - 1;
}

void EditionPool::popBlock() {
  assert(m_numberOfBlocks > 0);
  m_numberOfBlocks--;
}

void EditionPool::replaceBlock(Block * previousBlock, Block newBlock) {
  *previousBlock = newBlock;
}

bool EditionPool::insertBlocks(Block * destination, Block * source, size_t numberOfBlocks) {
  if (!checkForEnoughSpace(numberOfBlocks)) {
    return false;
  }
  size_t insertionSize = numberOfBlocks * sizeof(Block);
  memmove(destination + insertionSize, destination, static_cast<Block *>(lastBlock()) - destination);
  m_numberOfBlocks += numberOfBlocks;
  memcpy(destination, source, insertionSize);
  m_referenceTable.updateNodes(
      [](uint16_t * offset, Block * testedBlock, Block * destination, Block * block, int numberOfBlocks) {
        if (destination <= testedBlock) {
          *offset += numberOfBlocks;
          }
      }, destination, nullptr, numberOfBlocks);
  return true;
}

void EditionPool::removeBlocks(Block * address, size_t numberOfBlocks) {
  int deletionSize = numberOfBlocks * sizeof(Block);
  m_numberOfBlocks -= numberOfBlocks;
  memmove(address, address + deletionSize, static_cast<Block *>(lastBlock()) - address);
  m_referenceTable.updateNodes(
      [](uint16_t * offset, Block * testedBlock, Block * address, Block * block, int numberOfBlocks) {
        if (testedBlock > address) {
          *offset -= numberOfBlocks;
        } else if (testedBlock == address) {
          *offset = ReferenceTable::NoNodeIdentifier;
        }
      }, address, nullptr, numberOfBlocks);
}

void EditionPool::moveBlocks(Block * destination, Block * source, size_t numberOfBlocks) {
  uint8_t * src = reinterpret_cast<uint8_t *>(source);
  uint8_t * dst = reinterpret_cast<uint8_t *>(destination);
  size_t len = numberOfBlocks * sizeof(Block);
  Memory::Rotate(dst, src, len);
  m_referenceTable.updateNodes(
      [](uint16_t * offset, Block * testedBlock, Block * dst, Block * src, int nbOfBlocks) {
        if (testedBlock >= src && testedBlock < src + nbOfBlocks) {
          *offset += dst - src - (dst > src ? nbOfBlocks : 0);
        } else if ((testedBlock >= src + nbOfBlocks && testedBlock < dst) ||
                   (testedBlock >= dst && testedBlock < src)) {
          *offset += dst > src ? -nbOfBlocks : nbOfBlocks;
        }
      }, destination, source, numberOfBlocks);
}

Node EditionPool::initFromAddress(const void * address) {
  size_t size = Node(reinterpret_cast<const TypeBlock *>(address)).treeSize();
  if (!checkForEnoughSpace(size)) {
    return Node();
  }
  TypeBlock * copiedTree = lastBlock();
  memcpy(copiedTree, address, size * sizeof(Block));
  m_numberOfBlocks += size;
  return Node(copiedTree);
}

bool EditionPool::checkForEnoughSpace(size_t numberOfRequiredBlock) {
  if (m_numberOfBlocks + numberOfRequiredBlock > m_size) {
    ExceptionCheckpoint::Raise();
    return false;
  }
  return true;
}

}

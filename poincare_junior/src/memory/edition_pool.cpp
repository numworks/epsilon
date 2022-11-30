#include <assert.h>
#include "cache_pool.h"
#include "edition_pool.h"
#include "exception_checkpoint.h"
#include <utils.h>

namespace Poincare {

// ReferenceTable

Node EditionPool::ReferenceTable::nodeForIdentifier(uint16_t id) const {
  Node n = Pool::ReferenceTable::nodeForIdentifier(id);
  if (n.block() > m_pool->lastBlock()) {
    return Node();
  }
  return n;
}

void EditionPool::reinit(TypeBlock * firstBlock, size_t size) {
  m_firstBlock = firstBlock;
  m_size = size;
  flush();
}

void EditionPool::ReferenceTable::updateNodes(AlterSelectedBlock function, Block * contextSelection1, Block * contextSelection2, int contextAlteration) {
  Block * first = static_cast<Block *>(m_pool->firstBlock());
  for (int i = 0; i < m_length; i++) {
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

Block * EditionPool::pushBlock(Block block) {
  if (!checkForEnoughSpace(1)) {
    return nullptr;
  }
  assert(m_numberOfBlocks < m_size);
  *lastBlock() = block;
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
  memmove(destination + insertionSize, destination, lastBlock() - destination);
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
  memmove(address, address + deletionSize, lastBlock() - address);
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
          *offset += dst - src;
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
  TypeBlock * copiedTree = static_cast<TypeBlock *>(lastBlock());
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

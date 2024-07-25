#include "block_stack.h"

#include <assert.h>
#include <omg/memory.h>

#include <algorithm>

#include "tree.h"
#include "tree_stack_checkpoint.h"

#if POINCARE_POOL_VISUALIZATION
#include <poincare/src/memory/visualization.h>
#endif

namespace Poincare::Internal {

bool BlockStack::isRootBlock(const Block* block, bool allowLast) const {
  const Block* currentBlock = firstBlock();
  if (block >= lastBlock()) {
    return allowLast && block == lastBlock();
  }
  while (currentBlock < block) {
    currentBlock = Tree::FromBlocks(currentBlock)->nextTree()->block();
  }
  return currentBlock == block;
}

Block* BlockStack::initFromAddress(const void* address, bool isTree) {
  const Tree* node = Tree::FromBlocks(reinterpret_cast<const Block*>(address));
  size_t size = isTree ? node->treeSize() : node->nodeSize();
  Block* copiedTree = lastBlock();
  if (!insertBlocks(copiedTree, static_cast<const Block*>(address),
                    size * sizeof(Block), true)) {
    return nullptr;
  }
#if POINCARE_POOL_VISUALIZATION
  Log("Copy", copiedTree,
      isTree ? Tree::FromBlocks(copiedTree)->treeSize()
             : Tree::FromBlocks(copiedTree)->nodeSize());
#endif
  return copiedTree;
}

void BlockStack::replaceBlock(Block* previousBlock, Block newBlock) {
  replaceBlocks(previousBlock, &newBlock, 1);
}

void BlockStack::replaceBlocks(Block* destination, const Block* source,
                               size_t numberOfBlocks) {
  memcpy(destination, source, numberOfBlocks * sizeof(Block));
  m_referenceTable.updateNodes(
      [](uint16_t* offset, Block* block, const Block* destination,
         const Block* source, int size) {
        if (block >= destination && block < destination + size) {
          *offset = ReferenceTable::InvalidatedOffset;
        }
      },
      destination, nullptr, numberOfBlocks);
}

bool BlockStack::insertBlocks(Block* destination, const Block* source,
                              size_t numberOfBlocks, bool at) {
  if (numberOfBlocks == 0) {
    return true;
  }
  if (m_size + numberOfBlocks > k_maxNumberOfBlocks) {
    TreeStackCheckpoint::Raise(ExceptionType::TreeStackOverflow);
  }
  size_t insertionSize = numberOfBlocks * sizeof(Block);
  if (at && destination == lastBlock()) {
    m_size += numberOfBlocks;
    memcpy(destination, source, insertionSize);
    return true;
  }
  size_t editionPoolRightSize = static_cast<Block*>(lastBlock()) - destination;
  memmove(destination + insertionSize, destination, editionPoolRightSize);
  if (source >= destination && source < destination + editionPoolRightSize) {
    // Source has been memmoved.
    source += insertionSize;
  }

  m_size += numberOfBlocks;
  memcpy(destination, source, insertionSize);
  m_referenceTable.updateNodes(
      [](uint16_t* offset, Block* block, const Block* destination,
         const Block* source, int size) {
        if (destination <= block) {
          *offset += size;
        }
      },
      destination + at, nullptr, numberOfBlocks);
  return true;
}

void BlockStack::removeBlocks(Block* address, size_t numberOfBlocks) {
  // If this assert triggers, add an escape case
  assert(numberOfBlocks != 0);
  int deletionSize = numberOfBlocks * sizeof(Block);
  assert(m_size >= numberOfBlocks);
  m_size -= numberOfBlocks;
  assert(static_cast<Block*>(lastBlock()) >= address);
  memmove(address, address + deletionSize,
          static_cast<Block*>(lastBlock()) - address);
  m_referenceTable.updateNodes(
      [](uint16_t* offset, Block* block, const Block* address,
         const Block* source, int size) {
        if (block >= address + size) {
          *offset -= size;
        } else if (block >= address) {
          *offset = ReferenceTable::InvalidatedOffset;
        }
      },
      address, nullptr, numberOfBlocks);
}

void BlockStack::moveBlocks(Block* destination, Block* source,
                            size_t numberOfBlocks, bool at) {
  if (destination == source || numberOfBlocks == 0) {
    return;
  }
  uint8_t* src = reinterpret_cast<uint8_t*>(source);
  uint8_t* dst = reinterpret_cast<uint8_t*>(destination);
  size_t len = numberOfBlocks * sizeof(Block);
  OMG::Memory::Rotate(dst, src, len);
  if (at) {
    m_referenceTable.updateNodes(
        [](uint16_t* offset, Block* block, const Block* dst, const Block* src,
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
        [](uint16_t* offset, Block* block, const Block* dst, const Block* src,
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

void BlockStack::flush() {
  m_size = 0;
  m_referenceTable.reset();
#if POINCARE_POOL_VISUALIZATION
  Log("Flush");
#endif
}

void BlockStack::flushFromBlock(const Block* block) {
  /* This function is used to flush the right side of the pool when we revert to
   * a checkpoint. We initially expected the block to revert to to be a root
   * block, in order to have only well-formed trees on the stack during the
   * catch. This condition was removed and it is the responsability of the catch
   * to leave the stack in a state similar to what would have happened with no
   * raise. */
  m_size = block - m_blocks;
  m_referenceTable.invalidateIdentifiersAfterBlock(block);
#if POINCARE_POOL_VISUALIZATION
  Log("flushFromBlock", block);
#endif
}

uint16_t BlockStack::referenceBlock(Block* node) {
  return m_referenceTable.storeNode(node);
}

// ReferenceTable

Block* BlockStack::ReferenceTable::nodeForIdentifier(uint16_t id) const {
  if (id == NoNodeIdentifier) {
    return nullptr;
  }
  assert(id < m_length);
  uint16_t offset = m_nodeOffsetForIdentifier[id];
  if (offset == InvalidatedOffset) {
    return nullptr;
  }
  Block* n = m_pool->firstBlock() + offset;
  if (!m_pool->contains(n) && n != m_pool->lastBlock()) {
    /* The node has been corrupted, this is not referenced anymore. Referencing
     * the last block is tolerated though. */
    return nullptr;
  }
  return n;
}

uint16_t BlockStack::ReferenceTable::storeNode(Block* node) {
  if (isFull()) {
    Block* n;
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

void BlockStack::ReferenceTable::updateIdentifier(uint16_t id, Block* newNode) {
  assert(id < m_length);
  storeNodeAtIndex(newNode, id);
}

void BlockStack::ReferenceTable::deleteIdentifier(uint16_t id) {
  if (id == BlockStack::ReferenceTable::NoNodeIdentifier) {
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

void BlockStack::ReferenceTable::updateNodes(AlterSelectedBlock function,
                                             const Block* contextSelection1,
                                             const Block* contextSelection2,
                                             int contextAlteration) {
  Block* first = static_cast<Block*>(m_pool->firstBlock());
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

void BlockStack::ReferenceTable::invalidateIdentifiersAfterBlock(
    const Block* block) {
  Block* first = static_cast<Block*>(m_pool->firstBlock());
  assert(block >= first && block <= first + UINT16_MAX);
  uint16_t maxOffset = block - first;
  for (int i = 0; i < m_length; i++) {
    if (m_nodeOffsetForIdentifier[i] != DeletedOffset &&
        m_nodeOffsetForIdentifier[i] >= maxOffset) {
      m_nodeOffsetForIdentifier[i] = InvalidatedOffset;
    }
  }
}

bool BlockStack::ReferenceTable::reset() {
  if (m_length == 0) {
    // We can't reset an empty table
    return false;
  }
  m_length = 0;
  return true;
}

#if POINCARE_TREE_LOG

void BlockStack::ReferenceTable::logIdsForNode(std::ostream& stream,
                                               const Block* node) const {
  bool found = false;
  for (size_t i = 0; i < m_length; i++) {
    Block* n = BlockStack::ReferenceTable::nodeForIdentifier(i);
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

uint16_t BlockStack::ReferenceTable::storeNodeAtIndex(Block* node,
                                                      size_t index) {
  if (index >= m_length) {
    assert(index == m_length);
    assert(!isFull());
    // Increment first to make firstBlock != nullptr
    m_length++;
  }
  m_nodeOffsetForIdentifier[index] =
      static_cast<uint16_t>(node - m_pool->firstBlock());
  // Assertion requires valid firstBlock/lastBlock (so the order matters)
  assert(m_pool->contains(node) || node == m_pool->lastBlock());
  return index;
}

}  // namespace Poincare::Internal

#ifndef POINCARE_MEMORY_EDITION_POOL_H
#define POINCARE_MEMORY_EDITION_POOL_H

#include "pool.h"

namespace Poincare {

class EditionPool final : public Pool {
public:
  EditionPool(TypeBlock * firstBlock, size_t size, int numberOfBlocks = 0) :
    m_referenceTable(this),
    m_firstBlock(firstBlock),
    m_numberOfBlocks(numberOfBlocks),
    m_size(size)
  {}

  static EditionPool * sharedEditionPool();
  void reinit(TypeBlock * firstBlock, size_t size);

  uint16_t referenceNode(Node node);
  Node nodeForIdentifier(uint16_t id) { return m_referenceTable.nodeForIdentifier(id); }
  void flush();

  Block * pushBlock(Block block);
  void popBlock();
  void replaceBlock(Block * previousBlock, Block newBlock);
  bool insertBlock(Block * destination, Block block) { return insertBlocks(destination, &block, 1); }
  bool insertBlocks(Block * destination, Block * source, size_t numberOfBlocks);
  void removeBlocks(Block * address, size_t numberOfBlocks);
  void moveBlocks(Block * destination, Block * source, size_t numberOfBlocks);

  Node initFromTree(const Node node) { return initFromAddress(static_cast<const void *>(node.block())); }
  Node initFromAddress(const void * address);

  bool contains(Block * block) { return block >= firstBlock() && block < lastBlock(); }
  TypeBlock * firstBlock() override { return m_firstBlock; }
  Block * lastBlock() override { return m_firstBlock + m_numberOfBlocks; }
  Block * blockAtIndex(int i) { return m_firstBlock + i * sizeof(Block); }
  size_t fullSize() const { return m_size; }
  void setNumberOfBlocks(int numberOfBlocks) { m_numberOfBlocks = numberOfBlocks; }
  int numberOfBlocks() const { return m_numberOfBlocks; }

private:
  // Pool memory
  bool checkForEnoughSpace(size_t numberOfRequiredBlock);

  class ReferenceTable : public Pool::ReferenceTable {
    /* The edition pool reference table stores the offset of the tree in the
     * edition pool.
     * - We assume (and assert) that we never referenced more then
     *   k_maxNumberOfEditionReferences at the same time. We make sure of if by
     *  regularly flushing the reference table.
     * - The order of identifiers gives no guarantee on the order of the trees
     *   in the pool.
     */
  public:
    ReferenceTable(Pool * pool) : Pool::ReferenceTable(pool) {}
    Node nodeForIdentifier(uint16_t id) const override;
    typedef void (*AlterSelectedBlock)(uint16_t *, Block *, Block *, Block *, int);
    void updateNodes(AlterSelectedBlock function, Block * contextSelection1, Block * contextSelection2, int contextAlteration);
  };

  ReferenceTable m_referenceTable;
  TypeBlock * m_firstBlock;
  int m_numberOfBlocks;
  size_t m_size;
};

}

#endif

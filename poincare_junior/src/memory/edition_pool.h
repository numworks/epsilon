#ifndef POINCARE_MEMORY_EDITION_POOL_H
#define POINCARE_MEMORY_EDITION_POOL_H

#include "pool.h"
#include "reference.h"

namespace PoincareJ {

class EditionPool final : public Pool {
  friend class EditionReference;
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
  void flush();

  // TODO : Make execute private and force the use of a Relax method.
  bool execute(ActionWithContext action, void * subAction, const void * data, int maxSize);
  typedef bool (*Relax) (const void * data);
  bool executeWithRelax(ActionWithContext action, void * subAction, void * data, int maxSize, Relax relax);
  bool executeAndDump(ActionWithContext action, void * subAction, const void * data, void * address, int maxSize);

  Block * pushBlock(Block block);
  void popBlock();
  void replaceBlock(Block * previousBlock, Block newBlock);
  bool insertBlock(Block * destination, Block block) { return insertBlocks(destination, &block, 1); }
  bool insertBlocks(Block * destination, Block * source, size_t numberOfBlocks);
  void removeBlocks(Block * address, size_t numberOfBlocks);
  void moveBlocks(Block * destination, Block * source, size_t numberOfBlocks);

  Node initFromTree(const Node node) { return initFromAddress(static_cast<const void *>(node.block())); }
  Node initFromAddress(const void * address);

  using Pool::firstBlock;
  const TypeBlock * firstBlock() const override { return m_firstBlock; }
  using Pool::lastBlock;
  // If EditionPool is empty, first and last blocks are the same one
  const TypeBlock * lastBlock() const override { return m_firstBlock + m_numberOfBlocks; }
  size_t fullSize() const { return m_size; }
  void setNumberOfBlocks(int numberOfBlocks) { m_numberOfBlocks = numberOfBlocks; }

  constexpr static int k_maxNumberOfReferences = 1024;
private:
  // Pool memory
  bool checkForEnoughSpace(size_t numberOfRequiredBlock);
#if POINCARE_MEMORY_TREE_LOG
  const char * name() override { return "Edition"; }
#endif

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
    uint16_t storeNode(Node node) override;
    typedef void (*AlterSelectedBlock)(uint16_t *, Block *, Block *, Block *, int);
    void updateNodes(AlterSelectedBlock function, Block * contextSelection1, Block * contextSelection2, int contextAlteration);
  private:
    size_t maxNumberOfReferences() override { return EditionPool::k_maxNumberOfReferences; }
    uint16_t * nodeOffsetArray() override { return m_nodeOffsetForIdentifier; }
    uint16_t m_nodeOffsetForIdentifier[EditionPool::k_maxNumberOfReferences];
  };
  const ReferenceTable * referenceTable() const override { return &m_referenceTable; }

  ReferenceTable m_referenceTable;
  TypeBlock * m_firstBlock;
  int m_numberOfBlocks;
  size_t m_size;
};

}

#endif

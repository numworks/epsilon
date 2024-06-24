#ifndef POINCARE_MEMORY_TREE_STACK_H
#define POINCARE_MEMORY_TREE_STACK_H

#include <omg/global_box.h>
#include <string.h>

#include "block_stack.h"
#include "tree.h"
#include "type_block.h"
#if POINCARE_TREE_LOG
#include <iostream>
#endif

namespace Poincare {
class JuniorLayout;
}

namespace Poincare::Internal {

/* TreeStack is built on top of BlockStack to interpret contiguous chunk of
 * blocks as Trees and provide creation, iteration and logging methods. */

class TreeStack : public BlockStack {
  friend class TreeRef;

 public:
  using BlockStack::BlockStack;

  constexpr static int k_maxNumberOfBlocks = 1024 * 16;
  constexpr static int k_maxNumberOfReferences = k_maxNumberOfBlocks / 8;

  static OMG::GlobalBox<TreeStack> SharedTreeStack;

  size_t numberOfTrees() const;

  // Will changing the modified tree alter the other tree ?
  bool isAfter(const Tree* other, Tree* modified) {
    return !contains(other->block()) || other < modified;
  }

  // Initialize trees
  Tree* initFromAddress(const void* address, bool isTree = true) {
    return Tree::FromBlocks(BlockStack::initFromAddress(address, isTree));
  }
  Tree* clone(const Tree* node, bool isTree = true) {
    return initFromAddress(static_cast<const void*>(node->block()), isTree);
  }
  template <Type blockType, typename... Types>
  Tree* push(Types... args);
  Tree* pushBlock(Block block) {
    insertBlock(lastBlock(), block, true);
    return Tree::FromBlocks(lastBlock() - 1);
  }

  // TODO: factorize with TypeBlock
  constexpr static int NARY = -1;
  constexpr static int NARY2D = -2;
  constexpr static int NARY16 = -3;

  /* Define pushCos(), pushAdd(2) for simple nodes without additional value
   * blocks.
   *
   * We cannot define the function conditionally on the value of the template
   * parameter so we define all the versions for all nodes and filter them with
   * requires. It gives nice completions and ok-ish errors.
   */

#define PUSHER(F, N, S)                       \
  template <int I = N>                        \
    requires(I >= 0 && I == N && S == 0)      \
  Tree* push##F() {                           \
    return pushBlock(Type::F);                \
  }                                           \
                                              \
  template <int I = N>                        \
    requires(I == NARY && I == N && S == 0)   \
  Tree* push##F(int nbChildren) {             \
    Tree* result = pushBlock(Type::F);        \
    pushBlock(nbChildren);                    \
    return result;                            \
  }                                           \
                                              \
  template <int I = N>                        \
    requires(I == NARY2D && I == N && S == 0) \
  Tree* push##F(int nbRows, int nbCols) {     \
    Tree* result = pushBlock(Type::F);        \
    pushBlock(nbRows);                        \
    pushBlock(nbCols);                        \
    return result;                            \
  }

#define PUSHER_(F, N, S) PUSHER(F, N, S)
#define NODE_USE(F, N, S) PUSHER_(SCOPED_NODE(F), N, S)
#include "types.h"
#undef PUSHER
#undef PUSHER_

  // Reset TreeStack end to tree, ignoring what comes after
  void dropBlocksFrom(const Tree* tree) { flushFromBlock(tree->block()); }
  uint16_t referenceNode(Tree* node) {
    return BlockStack::referenceBlock(node->block());
  }

  Tree* nodeForIdentifier(uint16_t id) {
    return Tree::FromBlocks(blockForIdentifier(id));
  }

  /* TODO: move execute and relax out of this class ? */

  typedef void (*ActionWithContext)(void* context, const void* data);
  typedef bool (*Relax)(void* context);
  constexpr static Relax k_defaultRelax = [](void* context) { return false; };
  void executeAndStoreLayout(ActionWithContext action, void* context,
                             const void* data, Poincare::JuniorLayout* layout,
                             Relax relax = k_defaultRelax);
  void executeAndReplaceTree(ActionWithContext action, void* context,
                             Tree* data, Relax relax = k_defaultRelax);

  /* We delete the assignment operator because copying without care the
   * ReferenceTable would corrupt the m_referenceTable.m_pool pointer. */
  TreeStack& operator=(TreeStack&&) = delete;
  TreeStack& operator=(const TreeStack&) = delete;

#if POINCARE_TREE_LOG
  enum class LogFormat { Flat, Tree };
  void logNode(std::ostream& stream, const Tree* node, bool recursive,
               bool verbose, int indentation);
  void log(std::ostream& stream, LogFormat format, bool verbose,
           int indentation = 0);
  __attribute__((__used__)) void log() {
    log(std::cout, LogFormat::Tree, false);
  }
#endif
 private:
  Tree::ConstNodes allNodes() {
    return Tree::ConstNodes(Tree::FromBlocks(firstBlock()), numberOfTrees());
  }
  Tree::ConstTrees trees() {
    return Tree::ConstTrees(Tree::FromBlocks(firstBlock()), numberOfTrees());
  }

  void execute(ActionWithContext action, void* context, const void* data,
               int maxSize, Relax relax = k_defaultRelax);
};

#define SharedTreeStack TreeStack::SharedTreeStack

}  // namespace Poincare::Internal

#endif

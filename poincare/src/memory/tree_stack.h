#ifndef POINCARE_MEMORY_TREE_STACK_H
#define POINCARE_MEMORY_TREE_STACK_H

#include <omg/code_point.h>
#include <omg/global_box.h>
#include <string.h>

#include "block_stack.h"
#include "tree.h"
#include "type_block.h"
#if POINCARE_TREE_LOG
#include <iostream>
#endif

// TODO: used only for pushVar
#include <poincare/src/expression/sign.h>

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

  Tree* pushBlock(Block block) {
    insertBlock(lastBlock(), block, true);
    return Tree::FromBlocks(lastBlock() - 1);
  }

  // TODO: factorize with TypeBlock
  constexpr static int NARY = -1;
  constexpr static int NARY2D = -2;
  constexpr static int NARY16 = -3;

  // Generic pushers for simple nodes and n-aries e.g. pushCos(), pushAdd(nb)

  /* We cannot define the function conditionally on the value of the template
   * parameter so we define all the versions for all nodes and filter them with
   * requires. It gives nice completions and ok-ish errors.
   *
   * For instance, pushCos() is indeed pushCos<1>() and only the first template
   * will match resolving to :
   *   Tree* pushCos() {
   *     return pushBlock(Type::Cos());
   *   }
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

  // Specialized pushers for nodes with additional value blocks

  Tree* pushDecimal(uint8_t digitsAfterZero) {
    Tree* result = pushBlock(Type::Decimal);
    pushBlock(digitsAfterZero);
    return result;
  }

  Tree* pushSingleFloat(float value);
  Tree* pushDoubleFloat(double value);

  // overloaded pushFloat to be used with pushFloat(T value)
  Tree* pushFloat(float value) { return pushSingleFloat(value); }
  Tree* pushFloat(double value) { return pushDoubleFloat(value); }

  Tree* pushUserSymbol(const char* name) {
    return pushUserSymbol(name, strlen(name) + 1);
  }

  Tree* pushUserSymbol(const char* name, size_t size) {
    // TODO: change size to length
    return pushUserNamed(Type::UserSymbol, name, size);
  }

  Tree* pushUserFunction(const char* name) {
    return pushUserFunction(name, strlen(name) + 1);
  }

  Tree* pushUserFunction(const char* name, size_t size) {
    return pushUserNamed(Type::UserFunction, name, size);
  }

  Tree* pushUserSequence(const char* name) {
    return pushUserSequence(name, strlen(name) + 1);
  }

  Tree* pushUserSequence(const char* name, size_t size) {
    return pushUserNamed(Type::UserSequence, name, size);
  }

  Tree* pushVar(uint8_t id, ComplexSign sign);

  Tree* pushRandom(uint8_t seed);
  Tree* pushRandInt(uint8_t seed);
  Tree* pushRandIntNoRep(uint8_t seed);

  Tree* pushPhysicalConstant(uint8_t constantId);
  Tree* pushUnit(uint8_t representativeId, uint8_t prefixId);

  Tree* pushAsciiCodePointLayout(CodePoint codePoint);
  Tree* pushUnicodeCodePointLayout(CodePoint codePoint);
  Tree* pushCombinedCodePointsLayout(CodePoint codePoint,
                                     CodePoint combinedCodePoint);

  Tree* pushParenthesisLayout(bool leftIsTemporary, bool rightIsTemporary);
  Tree* pushVerticalOffsetLayout(bool isSubscript, bool isPrefix);

  Tree* pushRackLayout(int nbChildren);

  Tree* pushPointOfInterest(CustomTypeStructs::PointOfInterestNode data);

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
               size_t maxSize, Relax relax = k_defaultRelax);

  // type should be UserSequence, UserFunction or UserSymbol
  Tree* pushUserNamed(TypeBlock type, const char* name, size_t size);
};

#define SharedTreeStack TreeStack::SharedTreeStack

}  // namespace Poincare::Internal

#endif

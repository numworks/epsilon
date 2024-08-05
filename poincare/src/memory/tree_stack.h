#ifndef POINCARE_MEMORY_TREE_STACK_H
#define POINCARE_MEMORY_TREE_STACK_H

#include <omg/code_point.h>
#include <omg/global_box.h>
#include <string.h>

#include "block_stack.h"
#include "tree.h"
#include "tree_stack_checkpoint.h"
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

  static OMG::GlobalBox<TreeStack> SharedTreeStack
#if PLATFORM_DEVICE
      __attribute__((section(".bss.$tree_stack")))
#endif
      ;

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

  Tree* pushPointOfInterest(double abscissa, double ordinate, uint32_t data,
                            uint8_t interest, bool inverted,
                            uint8_t subCurveIndex);

  // Reset TreeStack end to tree, ignoring what comes after
  void dropBlocksFrom(const Tree* tree) { flushFromBlock(tree->block()); }
  uint16_t referenceNode(Tree* node) {
    return BlockStack::referenceBlock(node->block());
  }

  Tree* nodeForIdentifier(uint16_t id) {
    return Tree::FromBlocks(blockForIdentifier(id));
  }

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

  // type should be UserSequence, UserFunction or UserSymbol
  Tree* pushUserNamed(TypeBlock type, const char* name, size_t size);

  // Execute an action with input parameters types (Tree*, ContextT*,
  // ParametersTs...)
  template <typename ActionT, typename RelaxT, typename ContextT,
            typename... ParametersTs>
  void execute(ActionT action, Tree* tree, ContextT* context,
               std::size_t maxSize, RelaxT relax,
               ParametersTs... extraParameters) {
#if ASSERTIONS
    size_t treesNumber = numberOfTrees();
#endif
    size_t previousSize = size();
    while (true) {
      ExceptionTry {
        assert(numberOfTrees() == treesNumber);
        action(tree, context, extraParameters...);
        // Prevent edition action from leaking: an action create at most one
        // tree.
        assert(numberOfTrees() <= treesNumber + 1);
        // Ensure the result tree doesn't exceeds the expected size.
        if (size() - previousSize > maxSize) {
          TreeStackCheckpoint::Raise(ExceptionType::RelaxContext);
        }
        return;
      }
      ExceptionCatch(type) {
        assert(numberOfTrees() == treesNumber);
        switch (type) {
          case ExceptionType::TreeStackOverflow:
          case ExceptionType::IntegerOverflow:
          case ExceptionType::RelaxContext:
            if (relax(context)) {
              continue;
            }
          default:
            TreeStackCheckpoint::Raise(type);
        }
      }
    }
  }

 public:
  template <typename ActionT, typename ContextT, typename RelaxT,
            typename... ParametersTs>
  void executeAndReplaceTree(ActionT action, Tree* tree,
                             const ContextT* context, RelaxT relax,
                             ParametersTs... extraParameters) {
    assert(context);
    // Copy context to avoid altering the original
    ContextT contextCopy = *context;
    Block* previousLastBlock = lastBlock();
    execute(action, tree, &contextCopy, k_maxNumberOfBlocks, relax,
            extraParameters...);
    assert(previousLastBlock != lastBlock());
    tree->moveTreeOverTree(Tree::FromBlocks(previousLastBlock));
  }
};

#define SharedTreeStack TreeStack::SharedTreeStack

}  // namespace Poincare::Internal

#endif

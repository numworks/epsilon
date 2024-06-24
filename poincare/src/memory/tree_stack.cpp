#include "tree_stack.h"

#include <assert.h>
#include <omg/memory.h>
#include <poincare/old/junior_layout.h>

#include <algorithm>

#include "node_constructor.h"
#include "tree_stack_checkpoint.h"

#if POINCARE_POOL_VISUALIZATION
#include <poincare/src/memory/visualization.h>
#endif

namespace Poincare::Internal {

OMG::GlobalBox<TreeStack> TreeStack::SharedTreeStack;

size_t TreeStack::numberOfTrees() const {
  const Block* currentBlock = firstBlock();
  size_t result = 0;
  while (currentBlock < lastBlock()) {
    currentBlock = Tree::FromBlocks(currentBlock)->nextTree()->block();
    result++;
  }
  assert(currentBlock == lastBlock());
  return result;
}

template <>
Tree* TreeStack::push<Type::UserFunction>(const char* name) {
  return push<Type::UserFunction>(name, strlen(name) + 1);
}
template <>
Tree* TreeStack::push<Type::UserSequence>(const char* name) {
  return push<Type::UserSequence>(name, strlen(name) + 1);
}
template <>
Tree* TreeStack::push<Type::UserSymbol>(const char* name) {
  return push<Type::UserSymbol>(name, strlen(name) + 1);
}

template <Type blockType, typename... Types>
Tree* TreeStack::push(Types... args) {
  Block* newNode = lastBlock();

  size_t i = 0;
  bool endOfNode = false;
  do {
    Block block;
    endOfNode = NodeConstructor::CreateBlockAtIndexForType<blockType>(
        &block, i++, args...);
    pushBlock(block);
  } while (!endOfNode);
#if POINCARE_POOL_VISUALIZATION
  Log("Push", newNode, i);
#endif
  return Tree::FromBlocks(newNode);
}

void TreeStack::executeAndStoreLayout(ActionWithContext action, void* context,
                                      const void* data,
                                      Poincare::JuniorLayout* layout,
                                      Relax relax) {
  assert(numberOfTrees() == 0);
  execute(action, context, data, k_maxNumberOfBlocks, relax);
  assert(Tree::FromBlocks(firstBlock())->isLayout());
  *layout = Poincare::JuniorLayout::Builder(Tree::FromBlocks(firstBlock()));
  flush();
}

void TreeStack::executeAndReplaceTree(ActionWithContext action, void* context,
                                      Tree* data, Relax relax) {
  Block* previousLastBlock = lastBlock();
  execute(action, context, data, k_maxNumberOfBlocks, relax);
  assert(previousLastBlock != lastBlock());
  data->moveTreeOverTree(Tree::FromBlocks(previousLastBlock));
}

#if POINCARE_TREE_LOG

void TreeStack::logNode(std::ostream& stream, const Tree* node, bool recursive,
                        bool verbose, int indentation) {
  Indent(stream, indentation);
  stream << "<Reference id=\"";
  m_referenceTable.logIdsForNode(stream, node);
  stream << "\">\n";
  node->log(stream, recursive, verbose, indentation + 1);
  Indent(stream, indentation);
  stream << "</Reference>" << std::endl;
}

void TreeStack::log(std::ostream& stream, LogFormat format, bool verbose,
                    int indentation) {
  const char* formatName = format == LogFormat::Tree ? "tree" : "flat";
  Indent(stream, indentation);
  stream << "<TreeStack format=\"" << formatName << "\" size=\"" << size()
         << "\">\n";
  if (format == LogFormat::Tree) {
    for (const Tree* tree : trees()) {
      logNode(stream, tree, true, verbose, indentation + 1);
    }
  } else {
    for (const Tree* tree : allNodes()) {
      logNode(stream, tree, false, verbose, indentation + 1);
    }
  }
  Indent(stream, indentation);
  stream << "</TreeStack>" << std::endl;
}

#endif

void TreeStack::execute(ActionWithContext action, void* context,
                        const void* data, int maxSize, Relax relax) {
#if ASSERTIONS
  size_t treesNumber = numberOfTrees();
#endif
  size_t previousSize = size();
  while (true) {
    ExceptionTry {
      assert(numberOfTrees() == treesNumber);
      action(context, data);
      // Prevent edition action from leaking: an action create at most one tree.
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

template Tree* TreeStack::push<Type::Add, int>(int);
template Tree* TreeStack::push<Type::AsciiCodePointLayout, CodePoint>(
    CodePoint);
template Tree* TreeStack::push<Type::CombinedCodePointsLayout, CodePoint,
                               CodePoint>(CodePoint, CodePoint);
template Tree* TreeStack::push<Type::Decimal, int8_t>(int8_t);
template Tree* TreeStack::push<Type::DoubleFloat, double>(double);
template Tree* TreeStack::push<Type::IntegerNegBig>(uint64_t);
template Tree* TreeStack::push<Type::IntegerPosBig>(uint64_t);
template Tree* TreeStack::push<Type::IntegerNegShort>(uint8_t);
template Tree* TreeStack::push<Type::IntegerPosShort>(uint8_t);
template Tree* TreeStack::push<Type::List, int>(int);
template Tree* TreeStack::push<Type::Matrix, int, int>(int, int);
template Tree* TreeStack::push<Type::Matrix, uint8_t, uint8_t>(uint8_t,
                                                               uint8_t);
template Tree* TreeStack::push<Type::MatrixLayout, uint8_t, uint8_t>(uint8_t,
                                                                     uint8_t);
template Tree* TreeStack::push<Type::Mult, int>(int);
template Tree* TreeStack::push<Type::ParenthesisLayout, bool, bool>(
    bool leftIsTemporary, bool rightIsTemporary);
template Tree* TreeStack::push<Type::PhysicalConstant, uint8_t>(uint8_t);
template Tree* TreeStack::push<Type::Piecewise, int>(int);
template Tree* TreeStack::push<Type::PointOfInterest, double, double, uint32_t,
                               uint8_t, bool, uint8_t>(double, double, uint32_t,
                                                       uint8_t, bool, uint8_t);
template Tree* TreeStack::push<Type::Polynomial, int>(int);
template Tree* TreeStack::push<Type::RackLayout, int>(int);
template Tree* TreeStack::push<Type::Set>(int);
template Tree* TreeStack::push<Type::Set>(uint8_t);
template Tree* TreeStack::push<Type::SingleFloat, float>(float);
template Tree* TreeStack::push<Type::UnicodeCodePointLayout, CodePoint>(
    CodePoint);
template Tree* TreeStack::push<Type::Unit, uint8_t, uint8_t>(uint8_t, uint8_t);
template Tree* TreeStack::push<Type::UserFunction, const char*, size_t>(
    const char*, size_t);
template Tree* TreeStack::push<Type::UserSequence, const char*, size_t>(
    const char*, size_t);
template Tree* TreeStack::push<Type::UserSymbol, const char*, size_t>(
    const char*, size_t);
template Tree* TreeStack::push<Type::Var, uint8_t, ComplexSign>(uint8_t,
                                                                ComplexSign);
template Tree* TreeStack::push<Type::VerticalOffsetLayout, bool, bool>(
    bool isSubscript, bool isPrefix);

}  // namespace Poincare::Internal

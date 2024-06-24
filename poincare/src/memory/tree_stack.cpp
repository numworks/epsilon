#include "tree_stack.h"

#include <assert.h>
#include <omg/memory.h>
#include <poincare/old/junior_layout.h>
#include <poincare/src/expression/physical_constant.h>

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

Tree* TreeStack::pushSingleFloat(float value) {
  Tree* result = pushBlock(Type::SingleFloat);
  pushBlock(FloatNode::SubFloatAtIndex(value, 0));
  pushBlock(FloatNode::SubFloatAtIndex(value, 1));
  pushBlock(FloatNode::SubFloatAtIndex(value, 2));
  pushBlock(FloatNode::SubFloatAtIndex(value, 3));
  return result;
}

Tree* TreeStack::pushDoubleFloat(double value) {
  Tree* result = pushBlock(Type::DoubleFloat);
  pushBlock(FloatNode::SubFloatAtIndex(value, 0));
  pushBlock(FloatNode::SubFloatAtIndex(value, 1));
  pushBlock(FloatNode::SubFloatAtIndex(value, 2));
  pushBlock(FloatNode::SubFloatAtIndex(value, 3));
  pushBlock(FloatNode::SubFloatAtIndex(value, 4));
  pushBlock(FloatNode::SubFloatAtIndex(value, 5));
  pushBlock(FloatNode::SubFloatAtIndex(value, 6));
  pushBlock(FloatNode::SubFloatAtIndex(value, 7));
  return result;
}

Tree* TreeStack::pushUserNamed(TypeBlock type, const char* name, size_t size) {
  assert(type.isUserNamed());
  Tree* result = pushBlock(type);
  pushBlock(size);
  for (int i = 0; i < size - 1; i++) {
    pushBlock(name[i]);
  }
  pushBlock(0);
  return result;
}

Tree* TreeStack::pushVar(uint8_t id, ComplexSign sign) {
  Tree* result = pushBlock(Type::Var);
  pushBlock(id);
  pushBlock(sign.getValue());
  return result;
}

Tree* TreeStack::pushPhysicalConstant(uint8_t constantId) {
  assert(constantId < PhysicalConstant::k_numberOfConstants);
  Tree* result = pushBlock(Type::PhysicalConstant);
  pushBlock(constantId);
  return result;
}

Tree* TreeStack::pushUnit(uint8_t representativeId, uint8_t prefixId) {
  Tree* result = pushBlock(Type::Unit);
  pushBlock(representativeId);
  pushBlock(prefixId);
  return result;
}

Tree* TreeStack::pushAsciiCodePointLayout(CodePoint codePoint) {
  Tree* result = pushBlock(Type::AsciiCodePointLayout);
  assert(codePoint < 128);
  pushBlock(int(codePoint));
  return result;
}

Tree* TreeStack::pushUnicodeCodePointLayout(CodePoint codePoint) {
  static_assert(sizeof(CodePoint) / sizeof(uint8_t) == 4);
  Tree* result = pushBlock(Type::UnicodeCodePointLayout);
  int first = codePoint;
  pushBlock(OMG::BitHelper::getByteAtIndex(first, 0));
  pushBlock(OMG::BitHelper::getByteAtIndex(first, 1));
  pushBlock(OMG::BitHelper::getByteAtIndex(first, 2));
  pushBlock(OMG::BitHelper::getByteAtIndex(first, 3));
  return result;
}

Tree* TreeStack::pushCombinedCodePointsLayout(CodePoint codePoint,
                                              CodePoint combinedCodePoint) {
  static_assert(sizeof(CodePoint) / sizeof(uint8_t) == 4);
  Tree* result = pushBlock(Type::CombinedCodePointsLayout);
  int first = codePoint;
  int second = combinedCodePoint;
  pushBlock(OMG::BitHelper::getByteAtIndex(first, 0));
  pushBlock(OMG::BitHelper::getByteAtIndex(first, 1));
  pushBlock(OMG::BitHelper::getByteAtIndex(first, 2));
  pushBlock(OMG::BitHelper::getByteAtIndex(first, 3));
  pushBlock(OMG::BitHelper::getByteAtIndex(second, 0));
  pushBlock(OMG::BitHelper::getByteAtIndex(second, 1));
  pushBlock(OMG::BitHelper::getByteAtIndex(second, 2));
  pushBlock(OMG::BitHelper::getByteAtIndex(second, 3));
  return result;
}

Tree* TreeStack::pushParenthesisLayout(bool leftIsTemporary,
                                       bool rightIsTemporary) {
  Tree* result = pushBlock(Type::ParenthesisLayout);
  // TODO: factor with autocompleted_pair.h
  pushBlock(leftIsTemporary | (0b10 && rightIsTemporary));
  return result;
}

Tree* TreeStack::pushVerticalOffsetLayout(bool isSubscript, bool isPrefix) {
  Tree* result = pushBlock(Type::VerticalOffsetLayout);
  // TODO: factor with vertical_offset.h
  pushBlock(isSubscript | (0b10 && isPrefix));
  return result;
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

template Tree* TreeStack::push<Type::PointOfInterest, double, double, uint32_t,
                               uint8_t, bool, uint8_t>(double, double, uint32_t,
                                                       uint8_t, bool, uint8_t);
template Tree* TreeStack::push<Type::Polynomial, int>(int);
template Tree* TreeStack::push<Type::RackLayout, int>(int);

}  // namespace Poincare::Internal

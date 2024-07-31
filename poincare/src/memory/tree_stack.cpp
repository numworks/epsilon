#include "tree_stack.h"

#include <assert.h>
#include <omg/memory.h>
#include <poincare/numeric/point_of_interest.h>
#include <poincare/src/expression/float_helper.h>
#include <poincare/src/expression/physical_constant.h>

#include <algorithm>

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
  pushBlock(FloatHelper::SubFloatAtIndex(value, 0));
  pushBlock(FloatHelper::SubFloatAtIndex(value, 1));
  pushBlock(FloatHelper::SubFloatAtIndex(value, 2));
  pushBlock(FloatHelper::SubFloatAtIndex(value, 3));
  return result;
}

Tree* TreeStack::pushDoubleFloat(double value) {
  Tree* result = pushBlock(Type::DoubleFloat);
  pushBlock(FloatHelper::SubFloatAtIndex(value, 0));
  pushBlock(FloatHelper::SubFloatAtIndex(value, 1));
  pushBlock(FloatHelper::SubFloatAtIndex(value, 2));
  pushBlock(FloatHelper::SubFloatAtIndex(value, 3));
  pushBlock(FloatHelper::SubFloatAtIndex(value, 4));
  pushBlock(FloatHelper::SubFloatAtIndex(value, 5));
  pushBlock(FloatHelper::SubFloatAtIndex(value, 6));
  pushBlock(FloatHelper::SubFloatAtIndex(value, 7));
  return result;
}

Tree* TreeStack::pushUserNamed(TypeBlock type, const char* name, size_t size) {
  assert(type.isUserNamed());
  Tree* result = pushBlock(type);
  pushBlock(size);
  for (size_t i = 0; i < size - 1; i++) {
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

Tree* TreeStack::pushRandom(uint8_t seed) {
  Tree* result = pushBlock(Type::Random);
  pushBlock(seed);
  return result;
}

Tree* TreeStack::pushRandInt(uint8_t seed) {
  Tree* result = pushBlock(Type::RandInt);
  pushBlock(seed);
  return result;
}
Tree* TreeStack::pushRandIntNoRep(uint8_t seed) {
  Tree* result = pushBlock(Type::RandIntNoRep);
  pushBlock(seed);
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
  Tree* result = pushBlock(Type::ParenthesesLayout);
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

Tree* TreeStack::pushRackLayout(int nbChildren) {
  // Move this inside PUSHER if more NARY16 node are added
  Tree* result = pushBlock(Type::RackLayout);
  pushBlock(nbChildren % 256);
  pushBlock(nbChildren / 256);
  return result;
}

Tree* TreeStack::pushPointOfInterest(double abscissa, double ordinate,
                                     uint32_t data, uint8_t interest,
                                     bool inverted, uint8_t subCurveIndex) {
  Tree* result = pushBlock(Type::PointOfInterest);
  PointOfInterest p = {
      abscissa, ordinate,
      data,     static_cast<Internal::Solver<double>::Interest>(interest),
      inverted, subCurveIndex};
  // Copy content of data as if it was blocks
  insertBlocks(lastBlock(), reinterpret_cast<Block*>(&p),
               sizeof(CustomTypeStructs::PointOfInterestNode));
  return result;
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

}  // namespace Poincare::Internal

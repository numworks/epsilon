#include "tree_stack.h"

#include <assert.h>
#include <omg/memory.h>
#include <poincare/solver/point_of_interest.h>
#include <poincare/src/expression/float_helper.h>
#include <poincare/src/expression/integer.h>
#include <poincare/src/expression/physical_constant.h>

#include <algorithm>

#if POINCARE_TREE_STACK_VISUALIZATION
#include <poincare/src/memory/visualization.h>
#endif

namespace Poincare::Internal {

OMG::GlobalBox<TreeStack> TreeStack::SharedTreeStack;

size_t AbstractTreeStack::numberOfTrees() const {
  const Block* currentBlock = firstBlock();
  size_t result = 0;
  while (currentBlock < lastBlock()) {
    currentBlock = Tree::FromBlocks(currentBlock)->nextTree()->block();
    result++;
  }
  assert(currentBlock == lastBlock());
  return result;
}

Tree* AbstractTreeStack::pushSingleFloat(float value) {
  Tree* result = pushBlock(Type::SingleFloat);
  pushValueBlock(FloatHelper::SubFloatAtIndex(value, 0));
  pushValueBlock(FloatHelper::SubFloatAtIndex(value, 1));
  pushValueBlock(FloatHelper::SubFloatAtIndex(value, 2));
  pushValueBlock(FloatHelper::SubFloatAtIndex(value, 3));
  return result;
}

Tree* AbstractTreeStack::pushDoubleFloat(double value) {
  Tree* result = pushBlock(Type::DoubleFloat);
  pushValueBlock(FloatHelper::SubFloatAtIndex(value, 0));
  pushValueBlock(FloatHelper::SubFloatAtIndex(value, 1));
  pushValueBlock(FloatHelper::SubFloatAtIndex(value, 2));
  pushValueBlock(FloatHelper::SubFloatAtIndex(value, 3));
  pushValueBlock(FloatHelper::SubFloatAtIndex(value, 4));
  pushValueBlock(FloatHelper::SubFloatAtIndex(value, 5));
  pushValueBlock(FloatHelper::SubFloatAtIndex(value, 6));
  pushValueBlock(FloatHelper::SubFloatAtIndex(value, 7));
  return result;
}

Tree* AbstractTreeStack::pushInteger(int32_t value) {
  return Integer::Push(value);
}

Tree* AbstractTreeStack::pushInteger(uint32_t value) {
  return Integer::Push(value);
}

Tree* AbstractTreeStack::pushUserNamed(TypeBlock type, const char* name,
                                       size_t size) {
  assert(type.isUserNamed());
  Tree* result = pushBlock(type);
  pushValueBlock(size);
  for (size_t i = 0; i < size - 1; i++) {
    pushValueBlock(name[i]);
  }
  pushValueBlock(0);
  return result;
}

Tree* AbstractTreeStack::pushVar(uint8_t id, ComplexSign sign) {
  Tree* result = pushBlock(Type::Var);
  pushValueBlock(id);
  pushValueBlock(sign.getRealValue());
  pushValueBlock(sign.getImagValue());
  return result;
}

Tree* AbstractTreeStack::pushRandom(uint8_t seed) {
  Tree* result = pushBlock(Type::Random);
  pushValueBlock(seed);
  return result;
}

Tree* AbstractTreeStack::pushRandInt(uint8_t seed) {
  Tree* result = pushBlock(Type::RandInt);
  pushValueBlock(seed);
  return result;
}
Tree* AbstractTreeStack::pushRandIntNoRep(uint8_t seed) {
  Tree* result = pushBlock(Type::RandIntNoRep);
  pushValueBlock(seed);
  return result;
}

#if POINCARE_UNIT
Tree* AbstractTreeStack::pushPhysicalConstant(uint8_t constantId) {
  assert(constantId < PhysicalConstant::k_numberOfConstants);
  Tree* result = pushBlock(Type::PhysicalConstant);
  pushValueBlock(constantId);
  return result;
}

Tree* AbstractTreeStack::pushUnit(uint8_t representativeId, uint8_t prefixId) {
  Tree* result = pushBlock(Type::Unit);
  pushValueBlock(representativeId);
  pushValueBlock(prefixId);
  return result;
}
#endif

Tree* AbstractTreeStack::pushAngleUnitContext(AngleUnit angleUnit) {
  Tree* result = pushBlock(Type::AngleUnitContext);
  static_assert(sizeof(AngleUnit) == sizeof(Block));
  pushValueBlock(static_cast<uint8_t>(angleUnit));
  return result;
}

Tree* AbstractTreeStack::pushAsciiCodePointLayout(CodePoint codePoint) {
  Tree* result = pushBlock(Type::AsciiCodePointLayout);
  assert(codePoint < 128);
  pushValueBlock(int(codePoint));
  return result;
}

Tree* AbstractTreeStack::pushUnicodeCodePointLayout(CodePoint codePoint) {
  static_assert(sizeof(CodePoint) / sizeof(uint8_t) == 4);
  Tree* result = pushBlock(Type::UnicodeCodePointLayout);
  int first = codePoint;
  pushValueBlock(OMG::BitHelper::getByteAtIndex(first, 0));
  pushValueBlock(OMG::BitHelper::getByteAtIndex(first, 1));
  pushValueBlock(OMG::BitHelper::getByteAtIndex(first, 2));
  pushValueBlock(OMG::BitHelper::getByteAtIndex(first, 3));
  return result;
}

Tree* AbstractTreeStack::pushCombinedCodePointsLayout(
    CodePoint codePoint, CodePoint combiningCodePoint) {
  static_assert(sizeof(CodePoint) / sizeof(uint8_t) == 4);
  Tree* result = pushBlock(Type::CombinedCodePointsLayout);
  int first = codePoint;
  int second = combiningCodePoint;
  pushValueBlock(OMG::BitHelper::getByteAtIndex(first, 0));
  pushValueBlock(OMG::BitHelper::getByteAtIndex(first, 1));
  pushValueBlock(OMG::BitHelper::getByteAtIndex(first, 2));
  pushValueBlock(OMG::BitHelper::getByteAtIndex(first, 3));
  pushValueBlock(OMG::BitHelper::getByteAtIndex(second, 0));
  pushValueBlock(OMG::BitHelper::getByteAtIndex(second, 1));
  pushValueBlock(OMG::BitHelper::getByteAtIndex(second, 2));
  pushValueBlock(OMG::BitHelper::getByteAtIndex(second, 3));
  return result;
}

Tree* AbstractTreeStack::pushAutocompletedPairLayout(TypeBlock type,
                                                     bool leftIsTemporary,
                                                     bool rightIsTemporary) {
  Tree* result = pushBlock(type);
  pushValueBlock(leftIsTemporary | (rightIsTemporary << 1));
  return result;
}

Tree* AbstractTreeStack::pushVerticalOffsetLayout(bool isSubscript,
                                                  bool isPrefix) {
  Tree* result = pushBlock(Type::VerticalOffsetLayout);
  pushValueBlock(isSubscript | (isPrefix << 1));
  return result;
}

Tree* AbstractTreeStack::pushRackLayout(int nbChildren) {
  // Move this inside PUSHER if more NARY16 node are added
  Tree* result = pushBlock(Type::RackSimpleLayout);
  pushValueBlock(nbChildren % 256);
  pushValueBlock(nbChildren / 256);
  return result;
}

Tree* AbstractTreeStack::pushRackMemoizedLayout(int nbChildren) {
  // Move this inside PUSHER if more NARY16 node are added
  Tree* result = pushBlock(Type::RackMemoizedLayout);
  pushValueBlock(nbChildren % 256);
  pushValueBlock(nbChildren / 256);
  for (size_t i = 0; i < sizeof(CustomTypeStructs::RackMemoizedLayoutNode);
       i++) {
    pushValueBlock(0);
  }
  return result;
}

Tree* AbstractTreeStack::pushPointOfInterest(double abscissa, double ordinate,
                                             uint32_t data, uint8_t interest,
                                             bool inverted,
                                             uint8_t subCurveIndex) {
  Tree* result = pushBlock(Type::PointOfInterest);
  PointOfInterest p = {
      abscissa, ordinate,
      data,     static_cast<Solver<double>::Interest>(interest),
      inverted, subCurveIndex};
  // Copy content of data as if it was blocks
  insertBlocks(lastBlock(), reinterpret_cast<Block*>(&p),
               sizeof(CustomTypeStructs::PointOfInterestNode));
  return result;
}

Tree* AbstractTreeStack::pushSequenceLayout(int nbRows, int nbCols,
                                            uint8_t firstRank) {
  Tree* result = pushBlock(Type::SequenceLayout);
  pushValueBlock(nbRows % 256);
  pushValueBlock(nbCols % 256);
  pushValueBlock(firstRank);
  return result;
}

Tree* AbstractTreeStack::pushArbitrary(uint16_t size, const uint8_t* data) {
  Tree* result = pushBlock(Type::Arbitrary);
  pushValueBlock(0);  // nary
  pushValueBlock(size & 0xFF);
  pushValueBlock(size >> 8);
  for (size_t i = 0; i < size; i++) {
    pushValueBlock(data[i]);
  }
  return result;
}

#if POINCARE_TREE_LOG

void AbstractTreeStack::logNode(std::ostream& stream, const Tree* node,
                                bool recursive, bool verbose, int indentation,
                                bool serialize) {
  Indent(stream, indentation);
  stream << "<Reference id=\"";
  m_referenceTable.logIdsForNode(stream, node);
  stream << "\">\n";
  if (serialize) {
    Indent(stream, indentation + 1);
    node->logSerialize(stream);
  } else {
    node->log(stream, recursive, verbose, indentation + 1);
  }
  Indent(stream, indentation);
  stream << "</Reference>" << std::endl;
}

void AbstractTreeStack::log(std::ostream& stream, LogFormat format,
                            bool verbose, int indentation) {
  const char* formatName = format == LogFormat::Tree ? "tree" : "flat";
  Indent(stream, indentation);
  stream << "<AbstractTreeStack format=\"" << formatName << "\" size=\""
         << size() << "\">\n";
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
  stream << "</AbstractTreeStack>" << std::endl;
}

void AbstractTreeStack::logSerialize() {
  std::ostream& stream = std::cout;
  Indent(stream, 0);
  stream << "<TreeStack size=\"" << size() << "\">\n";
  for (const Tree* tree : trees()) {
    logNode(stream, tree, true, false, 1, true);
  }
  Indent(stream, 0);
  stream << "</TreeStack>" << std::endl;
}

#endif

}  // namespace Poincare::Internal

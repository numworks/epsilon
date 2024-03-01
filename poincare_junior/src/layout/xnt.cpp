#include "xnt.h"

#include <poincare/xnt_helpers.h>
#include <poincare_junior/src/expression/parametric.h>

#include "k_tree.h"
#include "parsing/tokenizer.h"
#include "serialize.h"

using namespace Poincare::XNTHelpers;

namespace PoincareJ {

// Parametered functions
constexpr struct {
  LayoutType layoutType;
  const CodePoint *XNTcycle;
} k_parameteredFunctions[] = {
    {LayoutType::Derivative, k_defaultContinuousXNTCycle},
    {LayoutType::NthDerivative, k_defaultContinuousXNTCycle},
    {LayoutType::Integral, k_defaultContinuousXNTCycle},
    {LayoutType::Sum, k_defaultDiscreteXNTCycle},
    {LayoutType::Product, k_defaultDiscreteXNTCycle},
    {LayoutType::ListSequence, k_defaultDiscreteXNTCycle},
};
constexpr int k_numberOfFunctions = std::size(k_parameteredFunctions);
constexpr int k_indexOfParameter = Parametric::k_variableIndex;

static bool findParameteredFunction2D(const Tree *layout, int *functionIndex,
                                      int *childIndex, const Tree *root,
                                      const Tree **parameterLayout) {
  assert(functionIndex && childIndex && parameterLayout);
  *functionIndex = -1;
  *childIndex = -1;
  *parameterLayout = nullptr;
  assert(layout);
  const Tree *child = layout;
  const Tree *parent = root->parentOfDescendant(child, childIndex);
  while (parent) {
    if (parent->isParametricLayout()) {
      if (*childIndex == k_indexOfParameter ||
          *childIndex == Parametric::FunctionIndex(parent)) {
        for (size_t i = 0; i < k_numberOfFunctions; i++) {
          if (parent->layoutType() == k_parameteredFunctions[i].layoutType) {
            *functionIndex = i;
            *parameterLayout = parent->child(k_indexOfParameter);
            return true;
          }
        }
      }
    }
    child = parent;
    parent = root->parentOfDescendant(child, childIndex);
  }
  return false;
}

static bool isValidXNTParameter(const Tree *xnt) {
  for (const Tree *child : xnt->children()) {
    if (!child->isCodePointLayout()) {
      return false;
    }
  }
  RackLayoutDecoder decoder(xnt);
  if (!Tokenizer::CanBeCustomIdentifier(decoder)) {
    return false;
  }
  return true;
}

bool FindXNTSymbol2D(const Tree *layout, const Tree *root, char *buffer,
                     size_t bufferSize, int xntIndex, size_t *cycleSize) {
  assert(cycleSize);
  int functionIndex;
  int childIndex;
  const Tree *parameterLayout;
  buffer[0] = 0;
  *cycleSize = 0;
  if (findParameteredFunction2D(layout, &functionIndex, &childIndex, root,
                                &parameterLayout)) {
    assert(0 <= functionIndex && functionIndex < k_numberOfFunctions);
    CodePoint xnt = CodePointAtIndexInCycle(
        xntIndex, k_parameteredFunctions[functionIndex].XNTcycle, cycleSize);
    Poincare::SerializationHelper::CodePoint(buffer, bufferSize, xnt);
    if (childIndex == Parametric::FunctionIndex(static_cast<BlockType>(
                          k_parameteredFunctions[functionIndex].layoutType))) {
      if (isValidXNTParameter(parameterLayout)) {
        Serialize(parameterLayout, buffer, buffer + bufferSize);
        *cycleSize = 1;
      }
    }
    assert(strlen(buffer) > 0);
    return true;
  }
  assert(strlen(buffer) == 0);
  return false;
}

}  // namespace PoincareJ

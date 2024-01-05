#include "derivation.h"

#include <poincare_junior/src/expression/k_tree.h>
#include <poincare_junior/src/expression/simplification.h>
#include <poincare_junior/src/expression/variables.h>
#include <poincare_junior/src/memory/exception_checkpoint.h>
#include <poincare_junior/src/memory/node_iterator.h>
#include <poincare_junior/src/n_ary.h>

namespace PoincareJ {

bool Derivation::ShallowSimplify(Tree *node) {
  // Reference is expected to have been reduced beforehand.
  assert(node->isDerivative());
  // Diff(Derivand, Symbol, SymbolValue)
  const Tree *symbol = node->child(0);
  const Tree *symbolValue = symbol->nextTree();
  const Tree *derivand = symbolValue->nextTree();
  Tree *result = Derivate(derivand, symbolValue, symbol);
  if (!result) {
    return false;
  }
  node->moveTreeOverTree(result);
  return true;
}

Tree *Derivation::Derivate(const Tree *derivand, const Tree *symbolValue,
                           const Tree *symbol) {
  if (derivand->treeIsIdenticalTo(KVar<0>)) {
    return (1_e)->clone();
  }
  if (derivand->isRandomNode()) {
    // Do not handle random nodes in derivation.
    return nullptr;
  }
  int numberOfChildren = derivand->numberOfChildren();
  if (numberOfChildren == 0) {
    return (0_e)->clone();
  }

  Tree *result = SharedEditionPool->push<BlockType::Addition>(0);
  const Tree *derivandChild = derivand->nextNode();
  /* D(f(g0(x),g1(x), ...)) = Sum(D(gi(x))*Di(f)(g0(x),g1(x), ...))
   * With D being the Derivative and Di being the partial derivative on
   * parameter i. */
  for (int i = 0; i < numberOfChildren; i++) {
    NAry::SetNumberOfChildren(result, i + 1);
    Tree *mult = SharedEditionPool->push<BlockType::Multiplication>(1);
    if (!Derivate(derivandChild, symbolValue, symbol)) {
      // Could not derivate, preserve D(gi(x))
      SharedEditionPool->push(BlockType::Derivative);
      symbol->clone();
      symbolValue->clone();
      derivandChild->clone();
    }
    if (!ShallowPartialDerivate(derivand, symbolValue, i)) {
      // Cancel current derivation.
      result->removeTree();
      return nullptr;
    }
    NAry::SetNumberOfChildren(mult, 2);
    Simplification::SimplifyMultiplication(mult);
    derivandChild = derivandChild->nextTree();
  }
  Simplification::SimplifyAddition(result);
  return result;
}

bool Derivation::ShallowPartialDerivate(const Tree *derivand,
                                        const Tree *symbolValue, int index) {
  switch (derivand->type()) {
    case BlockType::Multiplication: {
      // Di(x0 * x1 * ... * xi * ...) = x0 * x1 * ... * xi-1 * xi+1 * ...
      int numberOfChildren = derivand->numberOfChildren();
      assert(numberOfChildren > 1 && index < numberOfChildren);
      Tree *mult;
      if (numberOfChildren > 2) {
        mult = SharedEditionPool->push<BlockType::Multiplication>(
            numberOfChildren - 1);
      }
      for (std::pair<const Tree *, int> indexedNode :
           NodeIterator::Children<NoEditable>(derivand)) {
        if (indexedNode.second != index) {
          CloneReplacingSymbol(indexedNode.first, symbolValue);
        }
      }
      if (numberOfChildren > 2) {
        Simplification::SimplifyMultiplication(mult);
      }
      return true;
    }
    case BlockType::Complex:
      // TODO: Should we actually handle this ?
      if (index == 1) {
        SharedEditionPool->push(BlockType::Complex);
        SharedEditionPool->push(BlockType::Zero);
        SharedEditionPool->push(BlockType::One);
        return true;
      }
      // Fall through Addition
    case BlockType::Addition:
      // Di(x0 + x1 + ... + xi + ...) = 1
      SharedEditionPool->push(BlockType::One);
      return true;
    case BlockType::Exponential:
      // Di(exp(x)) = exp(x)
      CloneReplacingSymbol(derivand, symbolValue);
      return true;
    case BlockType::Ln: {
      // Di(ln(x)) = 1/x
      Tree *power = SharedEditionPool->push(BlockType::Power);
      CloneReplacingSymbol(derivand->child(0), symbolValue);
      SharedEditionPool->push(BlockType::MinusOne);
      Simplification::SimplifyPower(power);
      return true;
    }
    case BlockType::Trig:
      // Di(Trig(x, n)) = Trig(x, n-1)
    case BlockType::Power: {
      // Di(x^n) = n*x^(n-1)
      // Second parameter cannot depend on symbol.
      assert(!Variables::HasVariables(derivand->child(1)));
      if (index == 1) {
        SharedEditionPool->push(BlockType::Zero);
        return true;
      }
      Tree *multiplication;
      if (derivand->isPower()) {
        multiplication = SharedEditionPool->push<BlockType::Multiplication>(2);
        SharedEditionPool->clone(derivand->child(1));
      }
      Tree *newNode = SharedEditionPool->clone(derivand, false);
      CloneReplacingSymbol(derivand->child(0), symbolValue);
      Tree *addition = SharedEditionPool->push<BlockType::Addition>(2);
      SharedEditionPool->clone(derivand->child(1));
      SharedEditionPool->push(BlockType::MinusOne);
      Simplification::ShallowSystematicReduce(addition);
      Simplification::ShallowSystematicReduce(newNode);
      if (derivand->isPower()) {
        Simplification::ShallowSystematicReduce(multiplication);
      }
      return true;
    }
    default:
      return false;
  }
}

Tree *Derivation::CloneReplacingSymbol(const Tree *expression,
                                       const Tree *symbolValue) {
  Tree *result = expression->clone();
  Variables::LeaveScopeWithReplacement(result, symbolValue);
  return result;
}

// TODO : Diff(g(x,y),y,h(x,y)) -> Diff(g(f(y),z),z,h(f(y),y))
// Diff(g(x),z,h(x)) -> Diff(g(f(y)),z,h(f(y)))

}  // namespace PoincareJ

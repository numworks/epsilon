#include "derivation.h"

#include <poincare_junior/src/expression/k_tree.h>
#include <poincare_junior/src/expression/simplification.h>
#include <poincare_junior/src/memory/node_iterator.h>
#include <poincare_junior/src/n_ary.h>

namespace PoincareJ {

bool Derivation::ShallowSimplify(Tree *node) {
  // Reference is expected to have been reduced beforehand.
  assert(node->type() == BlockType::Derivative);
  // Diff(Derivand, Symbol, SymbolValue)
  const Tree *derivand = node->childAtIndex(0);
  const Tree *symbol = derivand->nextTree();
  const Tree *symbolValue = symbol->nextTree();
  Tree *result = Tree::FromBlocks(SharedEditionPool->lastBlock());
  Derivate(derivand, symbol, symbolValue);
  if (result->treeIsIdenticalTo(node)) {
    result->removeTree();
    return false;
  }
  node->moveTreeOverTree(result);
  return true;
}

void Derivation::Derivate(const Tree *derivand, const Tree *symbol,
                          const Tree *symbolValue) {
  if (symbol->treeIsIdenticalTo(derivand)) {
    SharedEditionPool->push<BlockType::One>();
    return;
  }
  int numberOfChildren = derivand->numberOfChildren();
  if (numberOfChildren == 0) {
    SharedEditionPool->push<BlockType::Zero>();
    return;
  }
  if (!derivand->type().isOfType({BlockType::Multiplication,
                                  BlockType::Addition, BlockType::Complex,
                                  BlockType::Exponential, BlockType::Power,
                                  BlockType::Trig, BlockType::Ln})) {
    // This derivation is not handled
    SharedEditionPool->push<BlockType::Derivative>();
    SharedEditionPool->clone(derivand);
    SharedEditionPool->clone(symbol);
    SharedEditionPool->clone(symbolValue);
    return;
  }

  Tree *result;
  if (numberOfChildren > 1) {
    result = SharedEditionPool->push<BlockType::Addition>(numberOfChildren);
  }
  const Tree *derivandChild = derivand->nextNode();
  /* D(f(g0(x),g1(x), ...)) = Sum(D(gi(x))*Di(f)(g0(x),g1(x), ...))
   * With D being the Derivative and Di being the partial derivative on
   * parameter i. */
  for (int i = 0; i < numberOfChildren; i++) {
    Tree *mult = SharedEditionPool->push<BlockType::Multiplication>(2);
    Derivate(derivandChild, symbol, symbolValue);
    ShallowPartialDerivate(derivand, symbol, symbolValue, i);
    Simplification::SimplifyMultiplication(mult);
    derivandChild = derivandChild->nextTree();
  }
  if (numberOfChildren > 1) {
    Simplification::SimplifyAddition(result);
  }
}

void Derivation::ShallowPartialDerivate(const Tree *derivand,
                                        const Tree *symbol,
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
          CloneReplacingSymbol(indexedNode.first, symbol, symbolValue);
        }
      }
      if (numberOfChildren > 2) {
        Simplification::SimplifyMultiplication(mult);
      }
      return;
    }
    case BlockType::Complex:
      // TODO: Should we actually handle this ?
      if (index == 1) {
        SharedEditionPool->push<BlockType::Complex>();
        SharedEditionPool->push<BlockType::Zero>();
        SharedEditionPool->push<BlockType::One>();
        return;
      }
      // Fall through Addition
    case BlockType::Addition:
      // Di(x0 + x1 + ... + xi + ...) = 1
      SharedEditionPool->push<BlockType::One>();
      return;
    case BlockType::Exponential:
      // Di(exp(x)) = exp(x)
      CloneReplacingSymbol(derivand, symbol, symbolValue);
      return;
    case BlockType::Ln: {
      // Di(ln(x)) = 1/x
      Tree *power = SharedEditionPool->push<BlockType::Power>();
      CloneReplacingSymbol(derivand->childAtIndex(0), symbol, symbolValue);
      SharedEditionPool->push<BlockType::MinusOne>();
      Simplification::SimplifyPower(power);
      return;
    }
    default:
      break;
  }
  // Di(x^n) = n*x^(n-1)
  // Di(Trig(x, n)) = Trig(x, n-1)
  assert(derivand->type() == BlockType::Trig ||
         derivand->type() == BlockType::Power);
  // Second parameter cannot depend on symbol.
  if (index == 1) {
    SharedEditionPool->push<BlockType::Zero>();
    return;
  }
  Tree *multiplication;
  if (derivand->type() == BlockType::Power) {
    multiplication = SharedEditionPool->push<BlockType::Multiplication>(2);
    SharedEditionPool->clone(derivand->childAtIndex(1));
  }
  Tree *newNode = SharedEditionPool->clone(derivand, false);
  CloneReplacingSymbol(derivand->childAtIndex(0), symbol, symbolValue);
  Tree *addition = SharedEditionPool->push<BlockType::Addition>(2);
  SharedEditionPool->clone(derivand->childAtIndex(1));
  SharedEditionPool->push<BlockType::MinusOne>();
  Simplification::ShallowSystematicReduce(addition);
  Simplification::ShallowSystematicReduce(newNode);
  if (derivand->type() == BlockType::Power) {
    Simplification::ShallowSystematicReduce(multiplication);
  }
  return;
}

Tree *Derivation::CloneReplacingSymbol(const Tree *expression,
                                       const Tree *symbol,
                                       const Tree *symbolValue) {
  assert(symbol->type() == BlockType::UserSymbol);
  if (symbol->treeIsIdenticalTo(symbolValue)) {
    // No need to replace anything
    return SharedEditionPool->clone(expression);
  }
  Tree *result = Tree::FromBlocks(SharedEditionPool->lastBlock());
  CloneReplacingSymbolRec(expression, symbol, symbolValue);
  return result;
}

bool Derivation::CloneReplacingSymbolRec(const Tree *expression,
                                         const Tree *symbol,
                                         const Tree *symbolValue) {
  if (symbol->treeIsIdenticalTo(expression)) {
    SharedEditionPool->clone(symbolValue);
    // symbolValue is already expected to be reduced.
    return true;
  }
  Tree *result = SharedEditionPool->clone(expression, false);
  // TODO: Extend this escape case to handle all nodes using local context.
  if (expression->type() == BlockType::Derivative) {
    // With x symbol and f(y) symbolValue :
    const Tree *subSymbol = expression->childAtIndex(1);
    if (subSymbol->treeIsIdenticalTo(symbol)) {
      // Diff(g(x),x,h(x)) -> Diff(g(x),x,h(f(y)))
      SharedEditionPool->clone(expression->nextNode());
      SharedEditionPool->clone(subSymbol);
      CloneReplacingSymbolRec(subSymbol->nextTree(), symbol, symbolValue);
      /* Not calling ShallowSystematicReduce because, since Diff was there after
       * reduction, changing symbolValue will not help further. */
      return true;
    }
    // TODO : Diff(g(x,y),y,h(x,y)) -> Diff(g(f(y),z),z,h(f(y),y))
    // Diff(g(x),z,h(x)) -> Diff(g(f(y)),z,h(f(y)))
  }
  bool changed = false;
  for (std::pair<const Tree *, int> indexedNode :
       NodeIterator::Children<NoEditable>(expression)) {
    changed = CloneReplacingSymbolRec(indexedNode.first, symbol, symbolValue) ||
              changed;
  }
  if (changed) {
    Simplification::ShallowSystematicReduce(result);
  }
  return changed;
}

}  // namespace PoincareJ

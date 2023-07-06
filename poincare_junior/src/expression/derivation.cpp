#include "derivation.h"

#include <poincare_junior/src/expression/k_tree.h>
#include <poincare_junior/src/expression/simplification.h>
#include <poincare_junior/src/memory/node_iterator.h>
#include <poincare_junior/src/n_ary.h>

namespace PoincareJ {

bool Derivation::Reduce(Node *node) {
  // Reference is expected to have been projected beforehand.
  /* TODO: This cannot be asserted since SytematicReduction may introduce powers
   * of additions that would be projected to exponentials. */
  // assert(!Simplification::DeepSystemProjection(ref));
  // Diff(Derivand, Symbol, SymbolValue)
  assert(node->type() == BlockType::Derivative);
  const Node *derivand = node->childAtIndex(0);
  const Node *symbol = derivand->nextTree();
  const Node *symbolValue = symbol->nextTree();
  Node *result = Node::FromBlocks(editionPool->lastBlock());
  Derivate(derivand, symbol, symbolValue);
  node->moveTreeOverTree(result);
  return true;
}

void Derivation::Derivate(const Node *derivand, const Node *symbol,
                          const Node *symbolValue) {
  if (symbol->treeIsIdenticalTo(derivand)) {
    editionPool->push<BlockType::One>();
    return;
  }
  int numberOfChildren = derivand->numberOfChildren();
  if (numberOfChildren == 0) {
    editionPool->push<BlockType::Zero>();
    return;
  }
  if (!derivand->block()->isOfType({BlockType::Multiplication,
                                    BlockType::Addition, BlockType::Exponential,
                                    BlockType::Power, BlockType::Trig,
                                    BlockType::Ln})) {
    // This derivation is not handled
    editionPool->push<BlockType::Derivative>();
    editionPool->clone(derivand);
    editionPool->clone(symbol);
    editionPool->clone(symbolValue);
    return;
  }

  if (numberOfChildren > 1) {
    editionPool->push<BlockType::Addition>(numberOfChildren);
  }
  const Node *derivandChild = derivand->nextNode();
  /* D(f(g0(x),g1(x), ...)) = Sum(D(gi(x))*Di(f)(g0(x),g1(x), ...))
   * With D being the Derivative and Di being the partial derivative on
   * parameter i. */
  for (int i = 0; i < numberOfChildren; i++) {
    editionPool->push<BlockType::Multiplication>(2);
    Derivate(derivandChild, symbol, symbolValue);
    ShallowPartialDerivate(derivand, symbol, symbolValue, i);
    derivandChild = derivandChild->nextTree();
  }
}

void Derivation::ShallowPartialDerivate(const Node *derivand,
                                        const Node *symbol,
                                        const Node *symbolValue, int index) {
  switch (derivand->type()) {
    case BlockType::Multiplication:
      // Di(x0 * x1 * ... * xi * ...) = x0 * x1 * ... * xi-1 * xi+1 * ...
      NAry::RemoveChildAtIndex(
          CloneReplacingSymbol(derivand, symbol, symbolValue), index);
      return;
    case BlockType::Addition:
      // Di(x0 + x1 + ... + xi + ...) = 1
      editionPool->push<BlockType::One>();
      return;
    case BlockType::Exponential:
      // Di(exp(x)) = exp(x)
      CloneReplacingSymbol(derivand, symbol, symbolValue);
      return;
    case BlockType::Ln:
      // Di(ln(x)) = 1/x
      editionPool->push<BlockType::Power>();
      CloneReplacingSymbol(derivand->childAtIndex(0), symbol, symbolValue);
      editionPool->push<BlockType::MinusOne>();
      return;
    default:
      break;
  }
  // Di(x^n) = n*x^(n-1)
  // Di(Trig(x, n)) = Trig(x, n-1)
  assert(derivand->type() == BlockType::Trig ||
         derivand->type() == BlockType::Power);
  // Second parameter cannot depend on symbol.
  if (index == 1) {
    editionPool->push<BlockType::Zero>();
    return;
  }
  if (derivand->type() == BlockType::Power) {
    editionPool->push<BlockType::Multiplication>(2);
    editionPool->clone(derivand->childAtIndex(1));
  }
  editionPool->clone(derivand, false);
  CloneReplacingSymbol(derivand->childAtIndex(0), symbol, symbolValue);
  editionPool->push<BlockType::Addition>(2);
  editionPool->clone(derivand->childAtIndex(1));
  editionPool->push<BlockType::MinusOne>();
  return;
}

Node *Derivation::CloneReplacingSymbol(const Node *expression,
                                       const Node *symbol,
                                       const Node *symbolValue) {
  assert(symbol->type() == BlockType::UserSymbol);
  if (symbol->treeIsIdenticalTo(symbolValue)) {
    // No need to replace anything
    return editionPool->clone(expression);
  }
  Node *result = Node::FromBlocks(editionPool->lastBlock());
  CloneReplacingSymbolRec(expression, symbol, symbolValue);
  return result;
}

void Derivation::CloneReplacingSymbolRec(const Node *expression,
                                         const Node *symbol,
                                         const Node *symbolValue) {
  if (symbol->treeIsIdenticalTo(expression)) {
    editionPool->clone(symbolValue);
    return;
  }
  editionPool->clone(expression, false);
  // TODO: Extend this escape case to handle all nodes using local context.
  if (expression->type() == BlockType::Derivative) {
    // With x symbol and f(y) symbolValue :
    const Node *subSymbol = expression->childAtIndex(1);
    if (subSymbol->treeIsIdenticalTo(symbol)) {
      // Diff(g(x),x,h(x)) -> Diff(g(x),x,h(f(y)))
      editionPool->clone(expression->nextNode());
      editionPool->clone(subSymbol);
      CloneReplacingSymbolRec(subSymbol->nextTree(), symbol, symbolValue);
      return;
    }
    // TODO : Diff(g(x,y),y,h(x,y)) -> Diff(g(f(y),z),z,h(f(y),y))
    // Diff(g(x),z,h(x)) -> Diff(g(f(y)),z,h(f(y)))
  }
  for (std::pair<const Node *, int> indexedNode :
       NodeIterator::Children<NoEditable>(expression)) {
    CloneReplacingSymbolRec(indexedNode.first, symbol, symbolValue);
  }
}

}  // namespace PoincareJ

#ifndef POINCARE_INTERFACES_EXPRESSION_H
#define POINCARE_INTERFACES_EXPRESSION_H

#include "../type_block.h"
#include "../value_block.h"

namespace Poincare {

class ExpressionInterface {
public:
  virtual void basicReduction(TypeBlock * treeBlock) const {}
  virtual float approximate(const TypeBlock * treeBlock) const = 0;
  /* Approximation, defaultReduction, createLayout */
  // IsCommutative?
  // reduceMatrix?
  // diff?
  // Policy based Design:
  // ExpressionInterface<LayoutPolicy, SimplificationPolicy>...
  // What about decreasing v-table sizes?
  // ExpressionInterface
  // InternalExpressionInterface
  // AlgebraicExpressionInterface, FunctionExpressionInterface etc?
protected:
  // TODO: tidy somewhere else?
  void projectionReduction(TypeBlock * block, TypeBlock * (*PushNode)(), TypeBlock * (*PushInverseNode)()) const;
};

}

#endif

/* Approximater implements approximate: division, addition implements are ApproximationInterface but not some specific expressions that are always reduces in Default reduction and that never reappears in reduceForApproximation
 *
 *  InternalExpressionInterface:
 *   - reduceForApproximation (recognize /, - and reintroduce them to ensure the most precise approximation)
 *   - beautify (some change just to return beautiful results - no root at denominator etc)
 *
 * Node::??Interface returns a nullptr if the type was wrong?
 *
 *
 * - matrix reduction
 * - list reduction
 * - unit reduction
 * -
 *
 *  reduceMatrices() {
 *    MatriceReducer m = static_cast<MatrixReducer>(*this);
 *    if (m) {
 *      m.reducematix
 *      }
 *
 * class MatrixReducter
 * MatrixMapper
 * MatrixUndefined
 *
 * class ListReducter
 * */

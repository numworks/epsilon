#include "typed_expression.h"

#include <poincare/src/memory/tree.h>
#include <poincare/src/memory/tree_stack.h>
#include <poincare/src/memory/type_block.h>

using namespace emscripten;
using namespace Poincare::Internal;

namespace Poincare::JSBridge {

/* Copies Javascript Uint8Array bytes on the tree stack and then build
 * a tree from it */
Tree* JsArrayToTree(const val& jsTree) {
  size_t treeSize = jsTree["length"].as<size_t>();
  if (treeSize == 0) {
    return nullptr;
  }

  // Copies the tree on the stack
  /* The Uint8Array is copied block per block on the stack because I couldn't
   * find an easy way to access the raw address of the external tree buffer.
   * I tried to use `jsTree["buffer"].as<uint8_t*>` but emscripten wouldn't let
   * me access the pointer. */
  TreeStack* stack = TreeStack::SharedTreeStack;
  Block* destination = stack->lastBlock();
  for (int i = 0; i < treeSize; i++) {
    stack->insertBlock(destination + i, Block(jsTree[i].as<uint8_t>()), true);
  }
  return Tree::FromBlocks(destination);
}

// Uses the bytes of the tree to build a TypedArray in javascript
val TreeToJsArray(const Tree* tree) {
  if (!tree) {
    uint8_t emptyArray[0];
    return val(typed_memory_view(0, emptyArray));
  }
  return val(typed_memory_view(tree->treeSize(),
                               reinterpret_cast<const uint8_t*>(tree)));
}

template <ExpressionType T>
TypedExpression<T> TypedExpression<T>::BuildFromJsTree(
    const TypedExpression<T>::JsTree& jsTree) {
  Tree* tree = JsArrayToTree(jsTree);
  JuniorExpression result = JuniorExpression::Builder(tree);
  return TypedExpression<T>::Cast(result);
}

// Tree types defined in poincare-partial.js
constexpr const char* JsTreeTypeName(ExpressionType type) {
  switch (type) {
    case ExpressionType::UserExpression:
      return "UserExpressionTree";
    case ExpressionType::SystemExpression:
      return "SystemExpressionTree";
    case ExpressionType::SystemFunction:
      return "SystemFunctionTree";
    default:
      return "Unknown ExpressionType";
  }
}

template <ExpressionType T>
TypedExpression<T>::JsTree TypedExpression<T>::getJsTree() const {
  /* Equivalent to the js code "new UserExpressionTree(typedArray)"$
   * (replace UserExpressionTree with the right JsTreeTypeName)
   * This array will be instantiated on javascript heap */
  return TypedExpression<T>::JsTree(
      val::global(JsTreeTypeName(T)).new_(TreeToJsArray(this->tree())));
}

template <ExpressionType T>
TypedExpression<T> TypedExpression<T>::typedClone() const {
  return TypedExpression<T>::Cast(this->clone());
}

template <ExpressionType T>
TypedExpression<T> TypedExpression<T>::typedCloneChildAtIndex(int i) const {
  return TypedExpression<T>::Cast(this->cloneChildAtIndex(i));
}

template class TypedExpression<ExpressionType::UserExpression>;
template class TypedExpression<ExpressionType::SystemExpression>;
template class TypedExpression<ExpressionType::SystemFunction>;

}  // namespace Poincare::JSBridge

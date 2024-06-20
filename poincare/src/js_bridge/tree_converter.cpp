#include <emscripten/bind.h>
#include <emscripten/val.h>
#include <poincare/old/junior_expression.h>
#include <poincare/src/memory/tree.h>
#include <poincare/src/memory/tree_stack.h>

using namespace emscripten;

namespace Poincare::JSBridge {

/* Bind Uint8Array to JavaScript type
 * https://emscripten.org/docs/porting/connecting_cpp_and_javascript/embind.html#custom-val-definitions
 * */
EMSCRIPTEN_DECLARE_VAL_TYPE(Uint8Array);

// TODO: This method should be differentiated from each type of expression
/* Copy Javascript Uint8Array bytes on the tree stack and then build an
 * expression from it */
JuniorExpression Uint8ArrayToExpression(const Uint8Array& jsTree) {
  size_t treeSize = jsTree["length"].as<size_t>();
  if (treeSize == 0) {
    return JuniorExpression();
  }

  // Copy the tree on the stack
  /* The Uint8Array is copied block per block on the stack because I couldn't
   * find an easy way to access the raw address of the external tree buffer.
   * I tried to use `jsTree["buffer"].as<uint8_t*>` but emscripten wouldn't let
   * me access the pointer. */
  Internal::TreeStack* stack = Internal::TreeStack::SharedTreeStack;
  Internal::Block* destination = stack->lastBlock();
  for (int i = 0; i < treeSize; i++) {
    stack->insertBlock(destination + i,
                       Internal::Block(jsTree[i].as<uint8_t>()), true);
  }
  Internal::Tree* tree = Internal::Tree::FromBlocks(destination);
  return JuniorExpression::Builder(tree);
}

// Create a JavaScript Uint8Array from the given expression tree
Uint8Array ExpressionToUint8Array(const JuniorExpression expression) {
  const Internal::Tree* tree = expression.tree();
  if (!tree) {
    // Equivalent to the js code "new Uint8Array()"
    return Uint8Array(val::global("Uint8Array").new_());
  }
  val typedArray = val(typed_memory_view(
      tree->treeSize(), reinterpret_cast<const uint8_t*>(tree)));
  /* Equivalent to the js code "new Uint8Array(typedArray)"
   * This array will be instantiated on javascript heap, allowing it to be
   * properly handled by the js garbage collector */
  return Uint8Array(val::global("Uint8Array").new_(typedArray));
}

EMSCRIPTEN_BINDINGS(tree_converter) {
  register_type<Uint8Array>("Uint8Array");
  function("Uint8ArrayToPCRExpression", &Uint8ArrayToExpression);
  function("PCRExpressionToUint8Array", &ExpressionToUint8Array);
}

}  // namespace Poincare::JSBridge

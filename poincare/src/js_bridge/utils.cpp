
#include "utils.h"

#include <emscripten/bind.h>
#include <poincare/src/memory/tree.h>
#include <poincare/src/memory/tree_stack.h>
#include <poincare/src/memory/type_block.h>

using namespace Poincare::Internal;
using namespace emscripten;

namespace Poincare::JSBridge {

namespace Utils {

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

bool ArraysHaveSameLength(const FloatArray& array1, const FloatArray& array2) {
  return array1["length"].as<int>() == array2["length"].as<int>();
}

EMSCRIPTEN_BINDINGS(float_array) { register_type<FloatArray>("number[]"); }

}  // namespace Utils
}  // namespace Poincare::JSBridge

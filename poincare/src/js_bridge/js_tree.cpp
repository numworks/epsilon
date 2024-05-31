#include <poincare/js_bridge/js_tree.h>
#include <poincare/src/memory/tree.h>
#include <poincare/src/memory/tree_stack.h>

using namespace emscripten;

namespace Poincare::JSBridge {

bool IsJSTree(const emscripten::val& val) {
  // This is a hack to check if the object is a Uint8Array in javascript
  return val["BYTES_PER_ELEMENT"].isNumber() &&
         val["BYTES_PER_ELEMENT"].as<size_t>() == 1;
}

// Copy Uint8Array bytes on the tree stack
Internal::Tree* CloneJSTreeOnStack(const JSTree& jsTree) {
  if (!IsJSTree(jsTree)) {
    // TODO throw invalid_argument error
    return nullptr;
  }

  size_t treeSize = jsTree["length"].as<size_t>();
  if (treeSize == 0) {
    return nullptr;
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
  return Internal::Tree::FromBlocks(destination);
}

// Build an Uint8Array from the tree bytes
JSTree JSTreeBuilder(const Internal::Tree* tree) {
  if (!tree) {
    // Empty array
    return val::global("Uint8Array").new_();
  }
  val typedArray = val(typed_memory_view(
      tree->treeSize(), reinterpret_cast<const uint8_t*>(tree)));
  /* This array will be instantiated on javascript heap, allowing it to be
   * properly handled by the js garbage collector */
  JSTree jsTree = val::global("Uint8Array").new_(typedArray);
  if (!IsJSTree(jsTree)) {
    // TODO throw error
  }
  return jsTree;
}

}  // namespace Poincare::JSBridge

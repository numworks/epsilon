#ifndef POINCARE_JS_BRIDGE_JS_TREE_H
#define POINCARE_JS_BRIDGE_JS_TREE_H

#include <emscripten/val.h>

/* These methods are used to send trees to JavaScript.
 *
 * === Why not directly bind a PoolHandle ? ===
 * C++ classes could be bound to JavaScript via Embind. A C++ function would
 * then be able to send this type of object to JS.
 * We could have directly bound a PoolHandle class, or a wrapper around it. The
 * problem with this approach is that the C++ objects sent to JS are allocated
 * on the C++/emscripten heap, which means they need to be manually freed by JS
 * (through the delete method). This behavior is cumbersome and error-prone, so
 * we found another solution. More info at:
 * https://emscripten.org/docs/porting/connecting_cpp_and_javascript/embind.html#memory-management
 *
 * === emscripten::val type ===
 * The emscripten::val type is a pointer to any JavaScript object, allocated on
 * the JS heap. To ensure that the Trees we send to JS are garbage-collected, we
 * leverage the emscripten::val::new_ method to build objects on the JS heap,
 * rather than on the C++ heap.
 *
 * === JSTree ===
 * To represent a Tree in JS, we use Uint8Array, which is an array of bytes.
 * Each time JS wants C++ to perform an action on this Tree, it sends the bytes
 * to a static method as emscripten::val and it is retransformed into a Tree by
 * the C++ code. This way, C++ manages by itself its memory on the Pool and
 * Stack and then return the result to the JS heap as another Uint8Array.
 */

namespace Poincare {

namespace Internal {
class Tree;
}

namespace JSBridge {

/* Bind JSTree to JavaScript type Uint8Array
 * https://emscripten.org/docs/porting/connecting_cpp_and_javascript/embind.html#custom-val-definitions
 * */
EMSCRIPTEN_DECLARE_VAL_TYPE(JSTree);

// Copy the bytes of the given JavaScript Uint8Array onto the stack
Internal::Tree* CloneJSTreeOnStack(const JSTree& jsTree);
// Create a JavaScript Uint8Array from the given tree bytes
JSTree JSTreeBuilder(const Internal::Tree* tree);

}  // namespace JSBridge
}  // namespace Poincare
#endif

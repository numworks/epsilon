#ifndef POINCARE_EXPRESSION_COMPLEX_H
#define POINCARE_EXPRESSION_COMPLEX_H

#include <poincare_junior/src/memory/edition_reference.h>

#include "k_tree.h"

namespace PoincareJ {

// A Complex tree C(x,y) represents x+i*y, x and y are real only if sanitized.
struct Complex {
 public:
  // Return first child if tree is a Complex, tree otherwise. May not be real.
  static const Tree* UnSanitizedRealPart(const Tree* tree) {
    return tree->type() == BlockType::Complex ? tree->nextNode() : tree;
  }
  // Return second child if tree is a Complex, 0 otherwise. May not be real.
  static const Tree* UnSanitizedImagPart(const Tree* tree) {
    return tree->type() == BlockType::Complex ? tree->child(1) : 0_e;
  }
  // Return true if tree is real and false if unknown or complex
  static bool IsReal(const Tree* tree);
  // Both its real and imaginary parts are real numbers.
  static bool IsSanitized(const Tree* tree) {
    return IsReal(UnSanitizedRealPart(tree)) &&
           IsReal(UnSanitizedImagPart(tree));
  }
};

}  // namespace PoincareJ

#endif

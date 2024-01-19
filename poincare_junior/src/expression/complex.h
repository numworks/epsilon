#ifndef POINCARE_EXPRESSION_COMPLEX_H
#define POINCARE_EXPRESSION_COMPLEX_H

#include <poincare_junior/src/memory/edition_reference.h>

#include "k_tree.h"
#include "sign.h"

namespace PoincareJ {

// A Complex tree C(x,y) represents x+i*y, x and y are real only if sanitized.
struct Complex {
 public:
  // Return real part of a simplified tree with extractable parts.
  static const Tree* RealPart(const Tree* tree) {
    assert(CanExtractParts(tree));
    return tree->isComplex()                 ? tree->child(0)
           : ComplexSign::Get(tree).isReal() ? tree
                                             : 0_e;
  }
  // Return imaginary part of a simplified tree with extractable parts.
  static const Tree* ImagPart(const Tree* tree) {
    assert(CanExtractParts(tree));
    return tree->isComplex()                 ? tree->child(1)
           : ComplexSign::Get(tree).isReal() ? 0_e
                                             : tree;
  }
  // Return true if real/imaginary part can be extracted
  static bool CanExtractParts(const Tree* tree) {
    assert(IsSanitized(tree));
    return tree->isComplex() || ComplexSign::Get(tree).isPure();
  }
  // Both its real and imaginary parts are real numbers.
  static bool IsSanitized(const Tree* tree) {
    if (!tree->isComplex()) {
      return true;
    }
    ComplexSign RealPartSign = ComplexSign::Get(tree->child(0));
    ComplexSign ImagPartSign = ComplexSign::Get(tree->child(1));
    return RealPartSign.isReal() && ImagPartSign.isReal();
  }
};

}  // namespace PoincareJ

#endif

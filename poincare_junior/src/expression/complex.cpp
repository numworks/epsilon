#include "complex.h"

#include <poincare_junior/src/memory/pattern_matching.h>
#include <poincare_junior/src/memory/placeholder.h>

#include "k_tree.h"
#include "number.h"
#include "simplification.h"

namespace PoincareJ {

using namespace Placeholders;

/* Must at least handle Addition, Multiplications, Numbers and Real/Imaginary
 * parts so that any simplified complex is sanitized. Also handle Exp, Ln and
 * Powers of positive integers so that abs(z) remains real after reduction.
 * TODO: Maybe use dimension analysis ? Handle other obvious types ?
 */
bool Complex::IsReal(const Tree* tree) {
  if (tree->block()->isOfType({
          BlockType::Addition, BlockType::Multiplication,
          BlockType::Exponential, BlockType::Power,
          // TODO: Handle Ln so that reduced abs(z) is always real.
          // BlockType::Ln,
          // BlockType::Cross,
          // BlockType::Derivative,  BlockType::Det,
          // BlockType::Dot,         BlockType::Inverse,
          // BlockType::List,        BlockType::Matrix,
          // BlockType::Polynomial,  BlockType::PowerReal,
          // BlockType::Ref,         BlockType::Rref,
          // BlockType::Set,         BlockType::Trace,
          // BlockType::Transpose,   BlockType::Trig,
      })) {
    // All their children must be reals
    for (const Tree* child : tree->children()) {
      if (!IsReal(child)) {
        return false;
      }
    }
    if (tree->type() == BlockType::Power) {
      const Tree* index = tree->childAtIndex(1);
      assert(index->block()->isNumber());
      return Number::Sign(index) != NonStrictSign::Negative;
    }
    return true;
  }
  return tree->block()->isNumber() ||
         tree->block()->isOfType({
             BlockType::ImaginaryPart, BlockType::RealPart,
             // BlockType::Abs,
             // BlockType::ComplexArgument, BlockType::Dim,
             // BlockType::Factorial, BlockType::Identity,
             // BlockType::Norm, BlockType::TrigDiff
         });
}

}  // namespace PoincareJ

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
          BlockType::Exponential, BlockType::Ln, BlockType::Power,
          // BlockType::Conjugate,   BlockType::Cross,
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

bool Complex::SimplifyComplex(Tree* tree) {
  assert(tree->type() == BlockType::Complex);
  Tree* imag = tree->childAtIndex(1);
  if (Number::IsZero(imag)) {
    // (A+0*i) -> A
    imag->removeTree();
    tree->removeNode();
    return true;
  }
  if (IsSanitized(tree)) {
    return false;
  }
  // x+iy = (re(x)-im(y)) + i*(im(x)+re(y))
  bool result = PatternMatching::MatchReplaceAndSimplify(
      tree, KComplex(KPlaceholder<A>(), KPlaceholder<B>()),
      KComplex(
          KAdd(KRe(KPlaceholder<A>()), KMult(-1_e, KIm(KPlaceholder<B>()))),
          KAdd(KIm(KPlaceholder<A>()), KRe(KPlaceholder<B>()))));
  assert(result && IsSanitized(tree));
  return result;
}

bool Complex::SimplifyComplexArgument(Tree* tree) {
  assert(tree->type() == BlockType::ComplexArgument);
  Tree* child = tree->childAtIndex(0);
  if (child->block()->isNumber()) {
    StrictSign sign = Number::StrictSign(child);
    tree->cloneTreeOverTree(sign == StrictSign::Null       ? KUndef
                            : sign == StrictSign::Positive ? 0_e
                                                           : π_e);
    return true;
  }
  // TODO: Implement for complexes
  return false;
}

bool Complex::SimplifyRealPart(Tree* tree) {
  assert(tree->type() == BlockType::RealPart);
  Tree* child = tree->childAtIndex(0);
  if (child->type() == BlockType::Complex || IsReal(child)) {
    assert(IsSanitized(child));
    // re(x+i*y) = x if x and y are reals
    tree->cloneTreeOverTree(UnSanitizedRealPart(child));
    return true;
  }
  return false;
}

bool Complex::SimplifyImaginaryPart(Tree* tree) {
  assert(tree->type() == BlockType::ImaginaryPart);
  Tree* child = tree->childAtIndex(0);
  if (child->type() == BlockType::Complex || IsReal(child)) {
    assert(IsSanitized(child));
    // im(x+i*y) = y if x and y are reals
    tree->cloneTreeOverTree(UnSanitizedImagPart(child));
    return true;
  }
  return false;
}

bool Complex::SimplifyConjugate(Tree* tree) {
  assert(tree->type() == BlockType::Conjugate);
  Tree* child = tree->childAtIndex(0);
  if (IsReal(child)) {
    tree->removeNode();
    return true;
  }
  if (child->type() != BlockType::Complex) {
    return false;
  }
  assert(IsSanitized(child));
  // conj(x+i*y) = x-i*y if x and y are reals
  return PatternMatching::MatchReplaceAndSimplify(
      tree, KConj(KComplex(KPlaceholder<A>(), KPlaceholder<B>())),
      KComplex(KPlaceholder<A>(), KMult(-1_e, KPlaceholder<B>())));
}

bool Complex::SimplifyAbs(Tree* tree) {
  assert(tree->type() == BlockType::Abs);
  // Anything else than Complexes are handled in Simplification::SimplifyAbs
  assert(tree->childAtIndex(0)->type() == BlockType::Complex);
  assert(IsSanitized(tree->childAtIndex(0)));
  // |x+iy| = √(x^2+y^2) if x and y are reals
  return PatternMatching::MatchReplaceAndSimplify(
      tree, KAbs(KComplex(KPlaceholder<A>(), KPlaceholder<B>())),
      KExp(KMult(KHalf, KLn(KAdd(KPow(KPlaceholder<A>(), 2_e),
                                 KPow(KPlaceholder<B>(), 2_e))))));
}

}  // namespace PoincareJ

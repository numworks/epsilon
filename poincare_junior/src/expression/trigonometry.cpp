#include "trigonometry.h"

#include <poincare_junior/src/expression/k_tree.h>

namespace PoincareJ {

// TODO: tests

bool Trigonometry::IsDirect(const Tree* node) {
  switch (node->type()) {
    case BlockType::Cosine:
    case BlockType::Sine:
    case BlockType::Tangent:
      return true;
    default:
      return false;
  }
}

bool Trigonometry::IsInverse(const Tree* node) {
  switch (node->type()) {
    case BlockType::ArcCosine:
    case BlockType::ArcSine:
    case BlockType::ArcTangent:
      return true;
    default:
      return false;
  }
}

const Tree* Trigonometry::ExactFormula(uint8_t n, bool isSin, bool* isOpposed) {
  // Sin and cos are 2pi periodic. With sin(n*π/12), n goes from 0 to 23.
  n = n % 24;
  // Formula is opposed depending on the quadrant
  if ((isSin && n >= 12) || (!isSin && n >= 6 && n < 18)) {
    *isOpposed = !*isOpposed;
  }
  // Last two quadrant are now equivalent to the first two ones.
  n = n % 12;
  /* In second half of first quadrant and in first half of second quadrant,
   * we can simply swap Sin/Cos formulas. */
  if (n > 3 && n <= 9) {
    isSin = !isSin;
  }
  // Second quadrant is now equivalent to the first one.
  n = n % 6;
  // Second half of the first quadrant is the first half mirrored.
  if (n > 3) {
    n = 6 - n;
  }
  // Only 4 formulas are left for angles 0, π/12, π/6 and π/4.
  assert(n >= 0 && n < 4);
  switch (n) {
    case 0:  // sin(0) / cos(0)
      return isSin ? 0_e : 1_e;
    case 1:  // sin(π/12) / cos(π/12)
      return isSin
                 ? KMult(KAdd(KExp(KMult(KHalf, KLn(3_e))), -1_e),
                         KPow(KMult(KExp(KMult(KHalf, KLn(2_e))), 2_e), -1_e))
                 : KMult(KAdd(KExp(KMult(KHalf, KLn(3_e))), 1_e),
                         KPow(KMult(KExp(KMult(KHalf, KLn(2_e))), 2_e), -1_e));
    case 2:  // sin(π/6) / cos(π/6)
      return isSin ? KHalf : KMult(KExp(KMult(KHalf, KLn(3_e))), KHalf);
    default:  // sin(π/4) = cos(π/4)
      return KExp(KMult(-1_e, KHalf, KLn(2_e)));
  }
}

}  // namespace PoincareJ

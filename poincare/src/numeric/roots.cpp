#include <poincare/numeric/roots.h>
#include <poincare/sign.h>
#include <poincare/src/expression/advanced_reduction.h>
#include <poincare/src/expression/approximation.h>
#include <poincare/src/expression/arithmetic.h>
#include <poincare/src/expression/integer.h>
#include <poincare/src/expression/k_tree.h>
#include <poincare/src/expression/order.h>
#include <poincare/src/expression/rational.h>
#include <poincare/src/expression/sign.h>
#include <poincare/src/memory/n_ary.h>
#include <poincare/src/memory/pattern_matching.h>
#include <poincare/src/memory/tree_ref.h>

namespace Poincare::Internal {

Tree* DefaultEvaluation::polynomial(const Tree* value, const Tree* a,
                                    const Tree* b, const Tree* c,
                                    const Tree* d) {
  // clang-format off
  Tree* e = PatternMatching::CreateSimplify(
    KAdd(
      KMult(KA, KPow(KH, 3_e)),
      KMult(KB, KPow(KH, 2_e)),
      KMult(KC, KH),
      KD),
    {.KA = a, .KB = b, .KC = c, .KD = d, .KH = value});
  // clang-format on
  AdvancedReduction::Reduce(e);
  return e;
}

Tree* RationalEvaluation::polynomial(const Tree* value, const Tree* a,
                                     const Tree* b, const Tree* c,
                                     const Tree* d) {
  Tree* x3 = Rational::IntegerPower(value, 3_e);
  Tree* aTerm = Rational::Multiplication(a, x3);
  Tree* x2 = Rational::IntegerPower(value, 2_e);
  Tree* bTerm = Rational::Multiplication(b, x2);
  Tree* cTerm = Rational::Multiplication(c, value);
  TreeRef result = Rational::Addition(aTerm, bTerm, cTerm, d);
  cTerm->removeTree();
  bTerm->removeTree();
  x2->removeTree();
  aTerm->removeTree();
  x3->removeTree();
  return result;
}

Tree* Roots::Linear(const Tree* a, const Tree* b) {
  assert(a && b);
  TreeRef root = PatternMatching::CreateSimplify(
      KMult(-1_e, KB, KPow(KA, -1_e)), {.KA = a, .KB = b});
  AdvancedReduction::Reduce(root);
  return root;
}

Tree* Roots::QuadraticDiscriminant(const Tree* a, const Tree* b,
                                   const Tree* c) {
  // Δ=B^2-4AC
  Tree* result = PatternMatching::CreateSimplify(
      KAdd(KPow(KB, 2_e), KMult(-4_e, KA, KC)), {.KA = a, .KB = b, .KC = c});
  // Use advanced reduction because systematic reduction doesn't expand powers.
  AdvancedReduction::Reduce(result);
  return result;
}

Tree* Roots::Quadratic(const Tree* a, const Tree* b, const Tree* c,
                       const Tree* discriminant) {
  assert(a && b && c);
  if (!discriminant) {
    Tree* discriminant = Roots::QuadraticDiscriminant(a, b, c);
    TreeRef solutions = Roots::Quadratic(a, b, c, discriminant);
    discriminant->removeTree();
    return solutions;
  }

  if (discriminant->isUndefined()) {
    return discriminant->cloneTree();
  }
  ComplexSign deltaSign = SignOfTreeOrApproximation(discriminant);
  if (deltaSign.isNull()) {
    // -B/2A
    return PatternMatching::CreateSimplify(
        KList(KMult(-1_e / 2_e, KB, KPow(KA, -1_e))), {.KA = a, .KB = b});
  }
  Tree* solutions = SharedTreeStack->pushList(2);
  // {-(B+√Δ)/2A, (-B+√Δ)/2A}
  Tree* root1 = PatternMatching::CreateSimplify(
      KMult(-1_e / 2_e, KAdd(KB, KExp(KMult(1_e / 2_e, KLn(KC)))),
            KPow(KA, -1_e)),
      {.KA = a, .KB = b, .KC = discriminant});
  PatternMatching::CreateSimplify(
      KMult(1_e / 2_e, KAdd(KMult(-1_e, KB), KExp(KMult(1_e / 2_e, KLn(KC)))),
            KPow(KA, -1_e)),
      {.KA = a, .KB = b, .KC = discriminant});
  ComplexSign aSign = SignOfTreeOrApproximation(a);
  if (aSign.isReal() && aSign.realSign().isNegative()) {
    // Switch roots for a consistent order
    root1->detachTree();
  }

  AdvancedReduction::Reduce(solutions);
  return solutions;
}

Tree* Roots::CubicDiscriminant(const Tree* a, const Tree* b, const Tree* c,
                               const Tree* d) {
  /* Δ = b^2*c^2 + 18abcd - 27a^2*d^2 - 4ac^3 - 4db^3
   * If Δ > 0, the cubic has three distinct real roots. If Δ < 0, the cubic has
   * one real root and two non-real complex conjugate roots.
   * If Δ = 0, the cubic has a multiple root. */

  // clang-format off
  Tree* delta = PatternMatching::CreateSimplify(
    KAdd(
      KMult(KPow(KB, 2_e), KPow(KC, 2_e)),
      KMult(18_e, KA, KB, KC, KD),
      KMult(-27_e, KPow(KA, 2_e), KPow(KD, 2_e)),
      KMult(-4_e, KA, KPow(KC, 3_e)),
      KMult(-4_e, KD, KPow(KB, 3_e))),
    {.KA = a, .KB = b, .KC = c, .KD = d});
  // clang-format on
  AdvancedReduction::Reduce(delta);
  return delta;
}

Tree* Roots::Cubic(const Tree* a, const Tree* b, const Tree* c, const Tree* d,
                   const Tree* preComputedDiscriminant) {
  assert(a && b && c && d);

  if (a->isUndefined() || b->isUndefined() || c->isUndefined() ||
      d->isUndefined()) {
    return KUndef->cloneTree();
  }

  /* Cases in which some coefficients are zero. */
  if (a->isZero()) {
    return Roots::Quadratic(b, c, d);
  }
  if (d->isZero()) {
    /* When d is null the obvious root is zero. To avoid complexifying the
     * remaining quadratic polynomial expression with further calculations, we
     * directly call the quadratic solver for a, b, and c. */
    TreeRef allRoots = Roots::Quadratic(a, b, c);
    assert(allRoots->isList());
    if (!NAry::ContainsSame(allRoots, KTree(0_e))) {
      NAry::AddChild(allRoots, KTree(0_e)->cloneTree());
      NAry::Sort(allRoots, Order::OrderType::ComplexLine);
    }
    return allRoots;
  }
  if (b->isZero() && c->isZero()) {
    /* We compute the three solutions here because they are quite simple, and
     * to avoid generating very complex coefficients when creating the
     * remaining quadratic equation. */
    return CubicRootsNullSecondAndThirdCoefficients(a, d);
  }

  /* To avoid applying Cardano's formula right away (because it takes a lot of
   * computation time), we use techniques to find a simple root, based on some
   * particularly common forms of cubic equations in school problems. */
  TreeRef foundRoot = TreeRef();
  foundRoot = SimpleRootSearch(a, b, c, d);
  if (!foundRoot && (a->isRational() && b->isRational() && c->isRational() &&
                     d->isRational())) {
    foundRoot = RationalRootSearch(a, b, c, d);
  }
  if (!foundRoot) {
    foundRoot = SumRootSearch(a, b, c, d);
  }
  if (foundRoot) {
    return CubicRootsKnowingNonZeroRoot(a, b, c, d, foundRoot);
  }

  /* We did not manage to find any simple root: we resort to using Cardano's
   * formula. */
  TreeRef delta = preComputedDiscriminant
                      ? preComputedDiscriminant->cloneTree()
                      : Roots::CubicDiscriminant(a, b, c, d);
  TreeRef cardanoRoots = CardanoMethod(a, b, c, d, delta);
  delta->removeTree();
  return cardanoRoots;
}

Tree* Roots::ApproximateRootsOfRealCubic(const Tree* roots,
                                         const Tree* discriminant) {
  ComplexSign discriminantSign = SignOfTreeOrApproximation(discriminant);
  assert(discriminantSign.isReal());
  TreeRef approximatedRoots = Approximation::RootTreeToTree<double>(roots);
  assert(approximatedRoots->isList());
  if (discriminantSign.realSign().isPositive()) {
    // If the discriminant is positive or zero, all roots are real.
    for (Tree* root : approximatedRoots->children()) {
      Tree* realPart =
          Approximation::extractRealPartIfImaginaryPartNegligible(root);
      assert(realPart != nullptr && realPart->isNumber());
      root->moveTreeOverTree(realPart);
    }
  } else {
    /*  If the discriminant is strictly negative, there are three distinct
     * roots. One root is real and the two others are complex conjugates. */
    assert(approximatedRoots->numberOfChildren() == 3);
    Tree* r1 = approximatedRoots->child(0);
    Tree* r2 = approximatedRoots->child(1);
    Tree* r3 = approximatedRoots->child(2);
    Tree* maybeRealR1 =
        Approximation::extractRealPartIfImaginaryPartNegligible(r1);
    Tree* maybeRealR2 =
        Approximation::extractRealPartIfImaginaryPartNegligible(r2);
    Tree* maybeRealR3 =
        Approximation::extractRealPartIfImaginaryPartNegligible(r3);
    // Only one of the root is real
    assert(((maybeRealR1 != nullptr) + (maybeRealR2 != nullptr) +
            (maybeRealR3 != nullptr)) == 1);
    if (maybeRealR1) {
      r1->moveTreeOverTree(maybeRealR1);
    }
    if (maybeRealR2) {
      r2->moveTreeOverTree(maybeRealR2);
    }
    if (maybeRealR3) {
      r3->moveTreeOverTree(maybeRealR3);
    }
  }
  NAry::Sort(approximatedRoots, Order::OrderType::ComplexLine);
  return approximatedRoots;
}

Tree* Roots::CubicRootsKnowingNonZeroRoot(const Tree* a, const Tree* b,
                                          const Tree* c, const Tree* d,
                                          Tree* r) {
  assert(!GetComplexSign(r).isNull());
  /* If r is a non zero root of "ax^3+bx^2+cx+d", we can factorize the
   * polynomial as "(x-r)*(ax^2+β*x+γ)", with "β =b+a*r" and γ=-d/r */
  TreeRef beta = PatternMatching::CreateSimplify(KAdd(KB, KMult(KA, KH)),
                                                 {.KA = a, .KB = b, .KH = r});
  TreeRef gamma = PatternMatching::CreateSimplify(
      KMult(-1_e, KD, KPow(KH, -1_e)), {.KD = d, .KH = r});
  TreeRef allRoots = Roots::Quadratic(a, beta, gamma);
  beta->removeTree();
  gamma->removeTree();
  if (!NAry::ContainsSame(allRoots, r)) {
    NAry::AddChild(allRoots, r);
    NAry::Sort(allRoots, Order::OrderType::ComplexLine);
  } else {
    r->removeTree();
  }
  return allRoots;
}

Tree* Roots::CubicRootsNullSecondAndThirdCoefficients(const Tree* a,
                                                      const Tree* d) {
  /* Polynoms of the form "ax^3+d=0" have a simple real solution : x1 =
   * sqrt(-d/a,3). Then the two other complex conjugate roots are given by x2
   * = rootsOfUnity[1] * x1 and x3 = rootsOfUnity[[2] * x1. */
  Tree* baseRoot = PatternMatching::CreateSimplify(
      KPow(KMult(-1_e, KPow(KA, -1_e), KD), KPow(3_e, -1_e)),
      {.KA = a, .KD = d});
  TreeRef rootList = PatternMatching::CreateSimplify(
      KList(KA, KMult(KA, k_cubeRootOfUnity1), KMult(KA, k_cubeRootOfUnity2)),
      {.KA = baseRoot});
  baseRoot->removeTree();
  NAry::Sort(rootList, Order::OrderType::ComplexLine);
  AdvancedReduction::Reduce(rootList);
  return rootList;
}

Tree* Roots::SimpleRootSearch(const Tree* a, const Tree* b, const Tree* c,
                              const Tree* d) {
  /* Polynomials which can be written as "kx^2(cx+d)+cx+d" have a simple root:
   * "-d/c". */
  TreeRef simpleRoot = PatternMatching::CreateSimplify(
      KMult(-1_e, KD, KPow(KC, -1_e)), {.KC = c, .KD = d});
  if (IsRoot(simpleRoot, a, b, c, d)) {
    return simpleRoot;
  }
  simpleRoot->removeTree();
  return nullptr;
}

Tree* Roots::RationalRootSearch(const Tree* a, const Tree* b, const Tree* c,
                                const Tree* d) {
  assert(a->isRational() && b->isRational() && c->isRational() &&
         d->isRational());

  /* The equation can be written with integer coefficients. Under that form,
   * since d/a = -x1*x2*x3, a rational root p/q must be so that p divides d
   * and q divides a. */

  Tree* denominatorA = Rational::Denominator(a).pushOnTreeStack();
  Tree* denominatorB = Rational::Denominator(b).pushOnTreeStack();
  Tree* denominatorC = Rational::Denominator(c).pushOnTreeStack();
  Tree* denominatorD = Rational::Denominator(d).pushOnTreeStack();
  TreeRef lcm = PatternMatching::CreateSimplify(KLCM(KA, KB, KC, KD),
                                                {.KA = denominatorA,
                                                 .KB = denominatorB,
                                                 .KC = denominatorC,
                                                 .KD = denominatorD});
  denominatorD->removeTree();
  denominatorC->removeTree();
  denominatorB->removeTree();
  denominatorA->removeTree();
  assert(lcm->isRational());
  TreeRef A = Rational::Multiplication(a, lcm);
  TreeRef D = Rational::Multiplication(d, lcm);
  lcm->removeTree();

  IntegerHandler AHandler = Integer::Handler(A);
  IntegerHandler DHandler = Integer::Handler(D);
  A->removeTree();
  D->removeTree();
  AHandler.setSign(NonStrictSign::Positive);
  DHandler.setSign(NonStrictSign::Positive);

  /* The absolute value of A or D might be above the uint32_t maximum
   * representable value. As the ListPositiveDivisors function only accepts
   * uint32_t input, we must prevent potential overflows. */
  if (IntegerHandler::Compare(AHandler, IntegerHandler(UINT32_MAX)) == 1) {
    return nullptr;
  }
  Arithmetic::Divisors divisorsA =
      Arithmetic::ListPositiveDivisors(AHandler.to<uint32_t>());
  Arithmetic::Divisors divisorsD =
      Arithmetic::ListPositiveDivisors(DHandler.to<uint32_t>());

  if (divisorsA.numberOfDivisors == Arithmetic::Divisors::k_divisorListFailed ||
      divisorsD.numberOfDivisors == Arithmetic::Divisors::k_divisorListFailed) {
    return nullptr;
  }

  for (int8_t i = 0; i < divisorsA.numberOfDivisors; i++) {
    for (int8_t j = 0; j < divisorsD.numberOfDivisors; j++) {
      /* If i and j are not coprime, i/j has already been tested. */
      uint32_t p = divisorsA.list[i];
      uint32_t q = divisorsD.list[j];
      if (Arithmetic::GCD(p, q) == 1) {
        Tree* r = Rational::Push(IntegerHandler(p), IntegerHandler(q));
        if (IsRoot<RationalEvaluation>(r, a, b, c, d)) {
          return r;
        }

        Rational::SetSign(r, NonStrictSign::Negative);
        if (IsRoot<RationalEvaluation>(r, a, b, c, d)) {
          return r;
        }
        r->removeTree();
      }
    }
  }
  return nullptr;
}

Tree* Roots::SumRootSearch(const Tree* a, const Tree* b, const Tree* c,
                           const Tree* d) {
  /* b is the opposite of the sum of all roots counted with their
   * multiplicity, multiplied by a. As additions containing roots or powers
   * are in general not reducible, if there exists an irrational root, it
   * might still be explicit in the expression for b. */

  if (b->isMult() || b->isAdd()) {
    /* If b is a product, it might contain a triple root. If b is an addition,
     * the different terms might be roots. */
    for (const Tree* productOrSumTerm : b->children()) {
      Tree* r = PatternMatching::CreateSimplify(
          KMult(-1_e, KB, KPow(KA, -1_e)), {.KA = a, .KB = productOrSumTerm});
      if (IsRoot(r, a, b, c, d)) {
        return r;
      }
      /* Test the opposite sign. The minus sign could be in any of the
       * different factors of a product. */
      r->moveTreeOverTree(
          PatternMatching::CreateSimplify(KMult(-1_e, KA), {.KA = r}));
      if (IsRoot(r, a, b, c, d)) {
        return r;
      }
      r->removeTree();
    }
  }
  return nullptr;
}

Tree* Roots::CardanoMethod(const Tree* a, const Tree* b, const Tree* c,
                           const Tree* d, const Tree* delta) {
  if (SignOfTreeOrApproximation(delta).isNull()) {
    Tree* rootList = CubicRootsNullDiscriminant(a, b, c, d);
    AdvancedReduction::Reduce(rootList);
    return rootList;
  }

  TreeRef delta0 = Delta0(a, b, c);
  TreeRef delta1 = Delta1(a, b, c, d);
  TreeRef cardano = CardanoNumber(delta0, delta1);
  delta1->removeTree();
  /* If the discriminant is not zero, then the Cardano number cannot be zero. */
  assert(!SignOfTreeOrApproximation(cardano).isNull());

  TreeRef cardanoRoot0 = CardanoRoot(a, b, cardano, delta0, 0);
  TreeRef cardanoRoot1 = CardanoRoot(a, b, cardano, delta0, 1);
  TreeRef cardanoRoot2 = CardanoRoot(a, b, cardano, delta0, 2);
  cardano->removeTree();
  delta0->removeTree();

  TreeRef rootList = PatternMatching::CreateSimplify(
      KList(KA, KB, KC),
      {.KA = cardanoRoot0, .KB = cardanoRoot1, .KC = cardanoRoot2});
  cardanoRoot2->removeTree();
  cardanoRoot1->removeTree();
  cardanoRoot0->removeTree();

  AdvancedReduction::Reduce(rootList);

  /* We do not sort the roots obtained with Cardano here, because their exact
   * form is complicated to read and does not allow one to see directly if the
   * value is real or complex. Sorting the roots will be handled by
   * approximation later on. */
  return rootList;
}

Tree* Roots::CubicRootsNullDiscriminant(const Tree* a, const Tree* b,
                                        const Tree* c, const Tree* d) {
  /* If the discriminant is zero, the cubic has a multiple root.
   * Furthermore, if Δ_0 = b^2 - 3ac is zero, the cubic has a triple root. */
  TreeRef delta0 = Delta0(a, b, c);

  // clang-format off
  TreeRef rootList = SignOfTreeOrApproximation(delta0).isNull()
    ?
      // "-b/3a" is a triple root
      PatternMatching::CreateSimplify(
          KList(
              KMult(-1_e, KB, KPow(KMult(3_e, KA), -1_e))),
          {.KA = a, .KB = b})
    :
      /* "(9ad - bc)/(2*delta0)" is a double root and
      * (4abc - 9da^2 - b^3)/(a*delta0)" is a simple root */
      PatternMatching::CreateSimplify(
          KList(
              KMult(
                  KAdd(KMult(9_e, KA, KD), KMult(-1_e, KB, KC)),
                  KPow(KMult(2_e, KH), -1_e)),
              KMult(
                  KAdd(
                      KMult(4_e, KA, KB, KC),
                      KMult(-9_e, KD, KPow(KA, 2_e)),
                      KMult(-1_e, KPow(KB, 3_e))),
                  KPow(KMult(KA, KH), -1_e))),
          {.KA = a, .KB = b, .KC = c, .KD = d, .KH = delta0});
  // clang-format on

  delta0->removeTree();

  NAry::Sort(rootList);
  return rootList;
}

Tree* Roots::Delta0(const Tree* a, const Tree* b, const Tree* c) {
  // Δ_0 = b^2 - 3ac
  Tree* delta0 = PatternMatching::CreateSimplify(
      KAdd(KPow(KB, 2_e), KMult(-3_e, KA, KC)), {.KA = a, .KB = b, .KC = c});

  AdvancedReduction::Reduce(delta0);
  return delta0;
}

Tree* Roots::Delta1(const Tree* a, const Tree* b, const Tree* c,
                    const Tree* d) {
  {
    //  Δ_1 = 2b^3 - 9abc + 27da^2
    // clang-format off
    Tree* delta1 = PatternMatching::CreateSimplify(
        KAdd(
          KMult(2_e, KPow(KB, 3_e)),
          KMult(-9_e, KA, KB, KC),
          KMult(27_e, KD, KPow(KA, 2_e))
        ),
        {.KA = a, .KB = b, .KC = c, .KD = d});
    // clang-format on

    AdvancedReduction::Reduce(delta1);
    return delta1;
  }
}

Tree* Roots::CardanoNumber(const Tree* delta0, const Tree* delta1) {
  /* C = root((delta1 ± sqrt(delta1^2 - 4*delta0^3)) / 2, 3)
   * The sign of ± must be chosen so that C is not null:
   *   - if delta0 is null, we enforce C = root(delta1, 3).
   *   - otherwise, ± takes the sign of delta1. This way, we do not run the
   *     risk of subtracting two very close numbers when delta0 << delta1. */

  if (SignOfTreeOrApproximation(delta0).isNull()) {
    return PatternMatching::CreateSimplify(KPow(KA, KPow(3_e, -1_e)),
                                           {.KA = delta1});
  }

  Tree* signDelta1 = SignOfTreeOrApproximation(delta1).realSign().isPositive()
                         ? (1_e)->cloneTree()
                         : (-1_e)->cloneTree();
  // clang-format off
  TreeRef cardano = PatternMatching::CreateSimplify(
    KPow(
      KMult(
        KAdd(
          KB,
          KMult(
            KC,
            KPow(
              KAdd(
                KPow(KB, 2_e),
                KMult(-4_e, KPow(KA, 3_e))),
              KPow(2_e, -1_e)))),
        KPow(2_e, -1_e)),
      KPow(3_e, -1_e)),
    {.KA = delta0, .KB = delta1, .KC = signDelta1});
  // clang-format on

  signDelta1->removeTree();

  AdvancedReduction::Reduce(cardano);
  return cardano;
}

Tree* Roots::CardanoRoot(const Tree* a, const Tree* b, const Tree* cardano,
                         const Tree* delta0, uint8_t k) {
  assert(k == 0 || k == 1 || k == 2);
  assert(!SignOfTreeOrApproximation(cardano).isNull());

  /* -(b + C + delta0/(C)/(3a) is a root of the cubic, where C is the Cardano
   * number multiplied by any cubic root of unity. All roots are listed by
   * computing the three possibilities for C, i.e. with the three cubic roots
   * of unity. */

  Tree* C = (k == 0)   ? cardano->cloneTree()
            : (k == 1) ? PatternMatching::CreateSimplify(
                             KMult(KA, k_cubeRootOfUnity1), {.KA = cardano})
                       : PatternMatching::CreateSimplify(
                             KMult(KA, k_cubeRootOfUnity2), {.KA = cardano});

  // clang-format off
  TreeRef root = PatternMatching::CreateSimplify(
      KMult(
        KPow(KMult(-3_e, KA), -1_e),
        KAdd(KB, KC, KMult(KD, KPow(KC, -1_e)))
      ),
      {.KA = a, .KB = b, .KC = C, .KD = delta0});
  // clang-format on

  C->removeTree();

  AdvancedReduction::Reduce(root);
  return root;
}

}  // namespace Poincare::Internal

#include <omg/list.h>
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

Tree* Roots::Linear(const Tree* a, const Tree* b) {
  assert(a && b);
  Tree* root = PatternMatching::CreateSimplify(KMult(-1_e, KB, KPow(KA, -1_e)),
                                               {.KA = a, .KB = b});
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
    /* Switch roots for a consistent order.
     * Quadratic may be called from grapher with coefficients b an c that cannot
     * be approximated (because they depend on variables. We therefore use a's
     * sign instead of ComplexLine order like cubic solver. */
    root1->detachTree();
  }

  AdvancedReduction::Reduce(solutions);
  return solutions;
}

Tree* Roots::CubicDiscriminant(const Tree* a, const Tree* b, const Tree* c,
                               const Tree* d) {
  /* Δ = b^2*c^2 + 18abcd - 27a^2*d^2 - 4ac^3 - 4db^3
   * If Δ > 0, the cubic has three distinct real roots. If Δ < 0, the cubic has
   * one real root and two non-real complex conjugate roots. If Δ = 0, the cubic
   * has a multiple root. */

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
                   const Tree* discriminant, bool fastCardanoMethod) {
  assert(a && b && c && d);

  if (a->isUndefined() || b->isUndefined() || c->isUndefined() ||
      d->isUndefined()) {
    return KUndef->cloneTree();
  }

  /* Cases in which some coefficients are zero. */
  if (GetComplexSign(a).isNull()) {
    return Roots::Quadratic(b, c, d);
  }
  if (GetComplexSign(d).isNull()) {
    /* When d is null the obvious root is zero. To avoid complexifying the
     * remaining quadratic polynomial expression with further calculations, we
     * directly call the quadratic solver for a, b, and c. */
    Tree* allRoots = Roots::Quadratic(a, b, c);
    assert(allRoots->isList());
    /* TODO: We could refactor this by either:
     * - creating an NAry::RemoveDuplicates function, that would be called right
     * after NAry::Sort. We could even make a shortcut that calls both steps:
     * NAry::SortAndRemoveDuplicates.
     * - designing and using a Set structure instead of a List. */
    if (!NAry::ContainsSame(allRoots, KTree(0_e))) {
      NAry::AddChild(allRoots, KTree(0_e)->cloneTree());
      NAry::Sort(allRoots, Order::OrderType::ComplexLine);
    }
    return allRoots;
  }
  if (GetComplexSign(b).isNull() && GetComplexSign(c).isNull()) {
    /* We compute the three solutions here because they are quite simple, and
     * to avoid generating very complex coefficients when creating the remaining
     * quadratic equation. */
    return CubicRootsNullSecondAndThirdCoefficients(a, d);
  }

  /* To avoid applying Cardano's formula right away (because it takes a lot of
   * computation time), we use techniques to find a simple root, based on some
   * particularly common forms of cubic equations in school problems. */
  TreeRef foundRoot = SimpleRootSearch(a, b, c, d);
  if (!foundRoot) {
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
  return CubicRootsCardanoMethod(a, b, c, d, discriminant, fastCardanoMethod);
}

Tree* Roots::ApproximateRootsOfRealCubic(const Tree* roots,
                                         const Tree* discriminant) {
  ComplexSign discriminantSign = SignOfTreeOrApproximation(discriminant);
  assert(discriminantSign.isReal());
  TreeRef approximatedRoots =
      Approximation::ToTree<double>(roots, Approximation::Parameters{});
  assert(approximatedRoots->isList());
  if (discriminantSign.realSign().isPositive()) {
    // If the discriminant is positive or zero, all roots are real.
    for (Tree* root : approximatedRoots->children()) {
      Tree* realPart =
          Approximation::ExtractRealPartIfImaginaryPartNegligible(root);
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
        Approximation::ExtractRealPartIfImaginaryPartNegligible(r1);
    Tree* maybeRealR2 =
        Approximation::ExtractRealPartIfImaginaryPartNegligible(r2);
    Tree* maybeRealR3 =
        Approximation::ExtractRealPartIfImaginaryPartNegligible(r3);
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

Tree* Roots::PolynomialEvaluation(const Tree* value, const Tree* a,
                                  const Tree* b, const Tree* c, const Tree* d) {
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

bool Roots::IsRoot(const Tree* value, const Tree* a, const Tree* b,
                   const Tree* c, const Tree* d) {
  Tree* e = PolynomialEvaluation(value, a, b, c, d);
  bool isZero = e->isZero();
  e->removeTree();
  return isZero;
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
   * sqrt(-d/a,3). Then the two other complex conjugate roots are given by x2 =
   * rootsOfUnity[1] * x1 and x3 = rootsOfUnity[[2] * x1. */
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
  /* Polynomials which can be written as "kx^2(cx+d)+cx+d" have a simple
   * root: "-d/c". */
  /* TODO: check the "kx(bx^2+d)+bx^2+d" pattern, with root √(-d/b) */
  Tree* simpleRoot = PatternMatching::CreateSimplify(
      KMult(-1_e, KD, KPow(KC, -1_e)), {.KC = c, .KD = d});
  if (IsRoot(simpleRoot, a, b, c, d)) {
    return simpleRoot;
  }
  simpleRoot->removeTree();
  return nullptr;
}

Tree* Roots::RationalRootSearch(const Tree* a, const Tree* b, const Tree* c,
                                const Tree* d) {
  if (!(a->isRational() && b->isRational() && c->isRational() &&
        d->isRational())) {
    return nullptr;
  }

  /* The equation can be written with integer coefficients. Under that form,
   * since d/a = -x1*x2*x3, a rational root p/q must be so that p divides d
   * and q divides a. */

  Tree* denominatorA = Rational::Denominator(a).pushOnTreeStack();
  Tree* denominatorB = Rational::Denominator(b).pushOnTreeStack();
  Tree* denominatorC = Rational::Denominator(c).pushOnTreeStack();
  Tree* denominatorD = Rational::Denominator(d).pushOnTreeStack();
  Tree* lcm = PatternMatching::CreateSimplify(KLCM(KA, KB, KC, KD),
                                              {.KA = denominatorA,
                                               .KB = denominatorB,
                                               .KC = denominatorC,
                                               .KD = denominatorD});

  assert(lcm->isRational());
  TreeRef A = Rational::Multiplication(a, lcm);
  TreeRef D = Rational::Multiplication(d, lcm);

  lcm->removeTree();
  denominatorD->removeTree();
  denominatorC->removeTree();
  denominatorB->removeTree();
  denominatorA->removeTree();

  IntegerHandler AHandler = Integer::Handler(A);
  IntegerHandler DHandler = Integer::Handler(D);

  AHandler.setSign(NonStrictSign::Positive);
  DHandler.setSign(NonStrictSign::Positive);

  /* The absolute value of A or D might be above the uint32_t maximum
   * representable value. As the ListPositiveDivisors function only accepts
   * uint32_t input, we must prevent potential overflows. */
  if (IntegerHandler::Compare(AHandler, IntegerHandler(UINT32_MAX)) == 1) {
    A->removeTree();
    D->removeTree();
    return nullptr;
  }
  Arithmetic::Divisors divisorsA =
      Arithmetic::ListPositiveDivisors(AHandler.to<uint32_t>());
  Arithmetic::Divisors divisorsD =
      Arithmetic::ListPositiveDivisors(DHandler.to<uint32_t>());

  A->removeTree();
  D->removeTree();

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
        if (IsRoot(r, a, b, c, d)) {
          return r;
        }

        Rational::SetSign(r, NonStrictSign::Negative);
        if (IsRoot(r, a, b, c, d)) {
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
  /* b is the opposite of the sum of all roots counted with their multiplicity,
   * multiplied by a. As additions containing roots or powers are in general not
   * reducible, if there exists an irrational root, it might still be explicit
   * in the expression for b. */

  if (b->isAdd()) {
    /* If b is an addition, then its expression might be "b = (-r1*a) + (-r2*a)
     * + (-r3*a)", when roots are irreducible. We can test for each term of the
     * addition whether  "-1 * addTerm / a" is a root. */
    for (const Tree* sumTerm : b->children()) {
      Tree* r = PatternMatching::CreateSimplify(KMult(-1_e, KH, KPow(KA, -1_e)),
                                                {.KA = a, .KH = sumTerm});
      if (IsRoot(r, a, b, c, d)) {
        return r;
      }
      r->removeTree();
    }
  }
  /* If the polynomial has a triple root, then the expression of b might be "b
   * = -a*3*r, with r being a triple root. */
  Tree* r = PatternMatching::CreateSimplify(
      KMult(-1_e / 3_e, KB, KPow(KA, -1_e)), {.KA = a, .KB = b});
  if (IsRoot(r, a, b, c, d)) {
    return r;
  }
  r->removeTree();
  return nullptr;
}

Tree* Roots::CubicRootsCardanoMethod(const Tree* a, const Tree* b,
                                     const Tree* c, const Tree* d,
                                     const Tree* delta,
                                     bool approximateCardanoNumber) {
  if (!delta) {
    Tree* discriminant = Roots::CubicDiscriminant(a, b, c, d);
    TreeRef roots = Roots::CubicRootsCardanoMethod(a, b, c, d, discriminant);
    discriminant->removeTree();
    return roots;
  }

  if (SignOfTreeOrApproximation(delta).isNull()) {
    Tree* rootList = CubicRootsNullDiscriminant(a, b, c, d);
    AdvancedReduction::Reduce(rootList);
    return rootList;
  }

  Tree* delta0 = Delta0(a, b, c);
  Tree* delta1 = Delta1(a, b, c, d);
  Tree* cardano = CardanoNumber(delta0, delta1);
  if (approximateCardanoNumber) {
    cardano->moveTreeOverTree(
        Approximation::ToTree<double>(cardano, Approximation::Parameters{}));
  }

  /* If the discriminant is not zero, then the Cardano number cannot be zero. */
  assert(!SignOfTreeOrApproximation(cardano).isNull());

  TreeRef rootList = SharedTreeStack->pushList(3);
  CardanoRoot(a, b, cardano, delta0, 0);
  CardanoRoot(a, b, cardano, delta0, 1);
  CardanoRoot(a, b, cardano, delta0, 2);
  cardano->removeTree();
  delta1->removeTree();
  delta0->removeTree();

  if (!approximateCardanoNumber) {
    AdvancedReduction::Reduce(rootList);
  } else {
    // Some rounding errors in Cardano's method calculations may lead to small
    // imaginary parts. If we know that some roots are pure real numbers because
    // the polynomial coefficients are all real, remove these small imaginary
    // parts.
    MoveTreeOverTree(rootList,
                     (SignOfTreeOrApproximation(a).isReal() &&
                      SignOfTreeOrApproximation(b).isReal() &&
                      SignOfTreeOrApproximation(c).isReal() &&
                      SignOfTreeOrApproximation(d).isReal())
                         ? Roots::ApproximateRootsOfRealCubic(rootList, delta)
                         : Approximation::ToTree<double>(
                               rootList, Approximation::Parameters{}));
  }
  NAry::Sort(rootList, Order::OrderType::ComplexLine);
  return rootList;
}

Tree* Roots::CubicRootsNullDiscriminant(const Tree* a, const Tree* b,
                                        const Tree* c, const Tree* d) {
  /* If the discriminant is zero, the cubic has a multiple root. Furthermore, if
   * Δ_0 = b^2 - 3ac is zero, the cubic has a triple root. */
  Tree* delta0 = Delta0(a, b, c);

  // clang-format off
  TreeRef rootList = SignOfTreeOrApproximation(delta0).isNull()
    ?
      // "-b/3a" is a triple root
      PatternMatching::CreateSimplify(
          KList(
              KMult(-1_e, KB, KPow(KMult(3_e, KA), -1_e))),
          {.KA = a, .KB = b})
    :
      /* "(9ad - bc)/(2*delta0)" is a double root and (4abc - 9da^2 - b^3)/(a*delta0)" is a simple root */
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

  NAry::Sort(rootList, Order::OrderType::ComplexLine);
  return rootList;
}

Tree* Roots::Delta0(const Tree* a, const Tree* b, const Tree* c) {
  // Δ_0 = b^2 - 3ac
  return PatternMatching::CreateSimplify(
      KAdd(KPow(KB, 2_e), KMult(-3_e, KA, KC)), {.KA = a, .KB = b, .KC = c});
}

Tree* Roots::Delta1(const Tree* a, const Tree* b, const Tree* c,
                    const Tree* d) {
  {
    //  Δ_1 = 2b^3 - 9abc + 27da^2
    // clang-format off
    return PatternMatching::CreateSimplify(
        KAdd(
          KMult(2_e, KPow(KB, 3_e)),
          KMult(-9_e, KA, KB, KC),
          KMult(27_e, KD, KPow(KA, 2_e))
        ),
        {.KA = a, .KB = b, .KC = c, .KD = d});
    // clang-format on
  }
}

Tree* Roots::CardanoNumber(const Tree* delta0, const Tree* delta1) {
  /* C = ∛((delta1 ± √(delta1^2 - 4*delta0^3)) / 2)
   * The sign of ± must be chosen so that C is not null:
   * - if delta0 is null, we enforce C = ∛(delta1).
   * - otherwise, ± takes the sign of delta1. This way, we do not run the risk
   * of subtracting two very close numbers when delta0 << delta1. */

  if (SignOfTreeOrApproximation(delta0).isNull()) {
    return PatternMatching::CreateSimplify(KPow(KA, KPow(3_e, -1_e)),
                                           {.KA = delta1});
  }

  const Tree* signDelta1 =
      SignOfTreeOrApproximation(delta1).realSign().isPositive() ? 1_e : -1_e;
  // clang-format off
  return PatternMatching::CreateSimplify(
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
}

Tree* Roots::CardanoRoot(const Tree* a, const Tree* b, const Tree* cardano,
                         const Tree* delta0, uint8_t k) {
  assert(k == 0 || k == 1 || k == 2);
  assert(!SignOfTreeOrApproximation(cardano).isNull());

  /* -(b + C + delta0/(C))/(3a) is a root of the cubic, where C is the Cardano
   * number multiplied by any cubic root of unity. All roots are listed by
   * computing the three possibilities for C, i.e. with the three cubic roots of
   * unity. */

  const Tree* cubicRoot = (k == 0)   ? 1_e
                          : (k == 1) ? k_cubeRootOfUnity1
                                     : k_cubeRootOfUnity2;

  // clang-format off
  return  PatternMatching::CreateSimplify(
      KMult(
        KPow(KMult(-3_e, KA), -1_e),
        KAdd(
          KB,
          KMult(KE, KC),
          KMult(KD, KPow(KMult(KE, KC), -1_e))
        )
      ),
      {.KA = a, .KB = b, .KC = cardano, .KD = delta0, .KE = cubicRoot});
  // clang-format on
}

}  // namespace Poincare::Internal

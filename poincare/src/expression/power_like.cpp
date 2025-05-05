#include "power_like.h"

#include "k_tree.h"
#include "poincare/src/memory/pattern_matching.h"
#include "rational.h"

namespace Poincare::Internal {

const Tree* PowerLike::Base(const Tree* e, bool ignorePowReal) {
  return GetBaseAndExponent(e, ignorePowReal).base;
}

const Tree* PowerLike::Exponent(const Tree* e, bool ignorePowReal) {
  return GetBaseAndExponent(e, ignorePowReal).exponent;
}

PowerLike::BaseAndExponent PowerLike::GetBaseAndExponent(const Tree* e,
                                                         bool ignorePowReal) {
  PatternMatching::Context ctx;
  // TODO: handle matching KExp(KMult(KA_p, KLn(KB)))
  if (PatternMatching::Match(e, KExp(KMult(KA, KLn(KB))), &ctx) &&
      ctx.getTree(KA)->isRational()) {
    return {.base = ctx.getTree(KB), .exponent = ctx.getTree(KA)};
  }
  if (e->isPow() || (e->isPowReal() && !ignorePowReal)) {
    const Tree* base = e->child(0);
    const Tree* exponent = base->nextTree();
    return {base, exponent};
  }
  return {e, 1_e};
}

bool PowerLike::ExpandRationalPower(Tree* e) {
  PowerLike::BaseAndExponent parameters = GetBaseAndExponent(e, true);
  return ExpandRationalPower(e, parameters.base, parameters.exponent);
}

bool PowerLike::ExpandRationalPower(Tree* e, const Tree* base,
                                    const Tree* power) {
  if (power->isInteger() || !power->isRational() ||
      Rational::IsStrictlyPositiveUnderOne(power)) {
    return false;
  }
  assert(PowerLike::Exponent(e, true)->treeIsIdenticalTo(power));
  assert(PowerLike::Base(e, true)->treeIsIdenticalTo(base));
  // e = x^(p/q) is expanded into x^n * x^(r/q) where r is the remainder of p/q
  TreeRef p = Rational::Numerator(power).pushOnTreeStack();
  TreeRef q = Rational::Denominator(power).pushOnTreeStack();
  assert(!q->isOne());
  TreeRef r =
      IntegerHandler::Remainder(Integer::Handler(p), Integer::Handler(q));
  // n = (p-r)/q
  TreeRef n = PatternMatching::CreateSimplify(
      KMult(KAdd(KA, KMult(-1_e, KC)), KPow(KB, -1_e)),
      {.KA = p, .KB = q, .KC = r});
  assert(n->isInteger());
  // result = base^n * exp(r/q * ln(base))
  TreeRef result = PatternMatching::CreateSimplify(
      KMult(KPow(KA, KB), KExp(KMult(KC, KPow(KD, -1_e), KLn(KA)))),
      {.KA = base, .KB = n, .KC = r, .KD = q});
  n->removeTree();
  r->removeTree();
  q->removeTree();
  p->removeTree();
  e->moveTreeOverTree(result);
  return true;
}

bool PowerLike::ReducePowerOfZero(Tree* e, const Tree* power) {
  ComplexSign indexSign = GetComplexSign(power);
  if (indexSign.realSign().isStrictlyPositive()) {
    // 0^x is always defined.
    e->cloneTreeOverTree(0_e);
    return true;
  }
  if (!indexSign.realSign().canBeStrictlyPositive()) {
    // 0^x cannot be defined
    e->cloneTreeOverTree(KOutOfDefinition);
    return true;
  }
  // Use a dependency as a fallback.
  return PatternMatching::MatchReplace(e, KA, KDep(0_e, KDepList(KA)));
}

}  // namespace Poincare::Internal

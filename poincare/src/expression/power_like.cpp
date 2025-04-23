#include "power_like.h"

#include "k_tree.h"
#include "poincare/src/memory/pattern_matching.h"

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

}  // namespace Poincare::Internal

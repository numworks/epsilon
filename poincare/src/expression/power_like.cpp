#include "power_like.h"

#include "k_tree.h"
#include "poincare/src/memory/pattern_matching.h"

namespace Poincare::Internal {

const Tree* PowerLike::Base(const Tree* e) {
  return e->isPow() ? e->child(0) : e;
}

const Tree* PowerLike::Exponent(const Tree* e) {
  return e->isPow() ? e->child(1) : 1_e;
}

PowerLike::BaseAndExponent PowerLike::GetExpBaseAndExponent(const Tree* e) {
  assert(e->isExp());
  PatternMatching::Context ctx;
  if (PatternMatching::Match(e, KExp(KMult(KA, KLn(KB))), &ctx) &&
      ctx.getTree(KA)->isRational()) {
    return {.base = ctx.getTree(KB), .exponent = ctx.getTree(KA)};
  }
  return {nullptr, nullptr};
}

}  // namespace Poincare::Internal

#include "simplification.h"

#include <poincare_junior/src/expression/approximation.h>
#include <poincare_junior/src/expression/k_creator.h>
#include <poincare_junior/src/expression/p_pusher.h>
#include <poincare_junior/src/expression/rational.h>
#include <poincare_junior/src/memory/node_iterator.h>
#include <poincare_junior/src/memory/pattern_matching.h>
#include <poincare_junior/src/memory/placeholder.h>
#include <poincare_junior/src/n_ary.h>

#include "derivation.h"
#include "number.h"

namespace PoincareJ {

using namespace Placeholders;

bool IsInteger(const Node* u) { return u->block()->isInteger(); }
bool IsNumber(const Node* u) { return u->block()->isNumber(); }
bool IsRational(const Node* u) { return u->block()->isRational(); }
bool IsConstant(const Node* u) { return IsNumber(u); }
bool IsZero(const Node* u) { return u->type() == BlockType::Zero; }
bool IsUndef(const Node* u) { return u->type() == BlockType::Undefined; }

void DropNode(EditionReference* u) {
  Node* previousU = *u;
  u->removeNode();
  *u = previousU;
}

bool AnyChildren(Node* u, bool test(const Node*)) {
  for (auto [child, index] : NodeIterator::Children<Forward, NoEditable>(u)) {
    if (test(child)) {
      return true;
    }
  }
  return false;
}

bool AllChildren(Node* u, bool test(const Node*)) {
  for (auto [child, index] : NodeIterator::Children<Forward, NoEditable>(u)) {
    if (!test(child)) {
      return false;
    }
  }
  return true;
}

bool Simplification::SystematicReduce(EditionReference* u) {
  if (IsRational(*u)) {
    ReplaceTreeByTree(u, Rational::IrreducibleForm(*u));
    return true;  // TODO
  }
  if (u->numberOfChildren() == 0) {
    return false;
  }
  bool childChanged = false;
  for (auto [child, index] : NodeIterator::Children<Forward, Editable>(*u)) {
    childChanged = SystematicReduce(&child) || childChanged;
    if (IsUndef(child)) {
      CloneNodeOverTree(u, KUndef);
      return true;
    }
  }

  switch (u->type()) {
    case BlockType::Power:
      return SimplifyPower(u) || childChanged;
    case BlockType::Addition:
      return SimplifySum(u) || childChanged;
    case BlockType::Multiplication:
      return SimplifyProduct(u) || childChanged;
    case BlockType::TrigDiff:
      return SimplifyTrigDiff(u) || childChanged;
    case BlockType::Trig:
      return SimplifyTrig(u) || childChanged;
    case BlockType::Derivative:
      return Derivation::Reduce(u) || childChanged;
    default:
      return childChanged;
  }
}

bool Simplification::SimplifyTrigDiff(EditionReference* u) {
  /* TrigDiff(x,y) = { 0 if x=y, 1 otherwise }
   * TODO: ContractTrigonometric is the only place this is used. It might not be
   * worth it. */
  Node* x = u->childAtIndex(0);
  Node* y = u->childAtIndex(1);
  assert(x->block()->isOfType({BlockType::Zero, BlockType::One}));
  assert(y->block()->isOfType({BlockType::Zero, BlockType::One}));
  CloneTreeOverTree(u, x->treeIsIdenticalTo(y) ? 0_e : 1_e);
  return true;
}

bool Simplification::SimplifyTrig(EditionReference* u) {
  // Trig(x,y) = {-Sin(x) if y=-1, Cos(x) if y=0, Sin(x) if y=1, -Cos(x) if y=2}
  EditionReference secondArgument = u->childAtIndex(1);
  /* Trig second element is always expected to be reduced. This will call
   * SimplifyTrigDiff if needed. */
  bool changed = SystematicReduce(&secondArgument);
  if (secondArgument.block()->isOfType({BlockType::MinusOne, BlockType::Two})) {
    // Simplify second argument to either 0 or 1 and oppose the tree.
    CloneTreeOverTree(&secondArgument,
                      secondArgument.type() == BlockType::Two ? 0_e : 1_e);
    EditionPool* editionPool(EditionPool::sharedEditionPool());
    MoveNodeBeforeNode(u, editionPool->push<BlockType::MinusOne>());
    MoveNodeBeforeNode(u, editionPool->push<BlockType::Multiplication>(2));
    return true;
  }
  assert(secondArgument.block()->isOfType({BlockType::Zero, BlockType::One}));
  return changed;
}

bool Simplification::SimplifyPower(EditionReference* u) {
  EditionReference v = u->childAtIndex(0);
  EditionReference n = u->childAtIndex(1);
  // 0^n -> 0
  if (v.type() == BlockType::Zero) {
    if (n.type() != BlockType::Zero &&
        Rational::RationalStrictSign(n) == StrictSign::Positive) {
      CloneNodeOverTree(u, 0_e);
      return true;
    }
    CloneNodeOverTree(u, KUndef);
    return true;
  }
  // 1^n -> 1
  if (v.type() == BlockType::One) {
    CloneNodeOverTree(u, 1_e);
    return true;
  }
  if (IsRational(v)) {
    return SimplifyRationalTree(u);
  }
  assert(IsInteger(n));
  // v^0 -> 1
  if (n.type() == BlockType::Zero) {
    CloneNodeOverTree(u, 1_e);
    return true;
  }
  // v^1 -> v
  if (n.type() == BlockType::One) {
    ReplaceTreeByTree(u, v);
    return true;
  }
  // (w^p)^n -> w^(p*n)
  if (v.type() == BlockType::Power) {
    EditionReference p = v.childAtIndex(1);
    assert(p.nextTree() == static_cast<Node*>(n));
    EditionReference m =
        EditionPool::sharedEditionPool()->push<BlockType::Multiplication>(2);
    MoveNodeBeforeNode(&p, m);
    DropNode(u);
    SimplifyProduct(&p);
    assert(IsInteger(p));
    return SimplifyPower(u);
  }
  // (w1*...*wk)^n -> w1^n * ... * wk^n
  if (v.type() == BlockType::Multiplication) {
    for (auto [w, index] : NodeIterator::Children<Forward, Editable>(v)) {
      EditionReference m =
          EditionPool::sharedEditionPool()->push<BlockType::Power>();
      w.clone();
      n.clone();
      ReplaceTreeByTree(&w, m);
      SimplifyPower(&w);
    }
    n.removeTree();
    DropNode(u);
    return SimplifyProduct(u);
  }
  return false;
}

EditionReference PushBase(Node* u) {
  if (IsNumber(u)) {
    return P_UNDEF();
  }
  if (u->type() == BlockType::Power) {
    return u->childAtIndex(0)->clone();
  }
  return u->clone();
}

EditionReference PushExponent(Node* u) {
  if (IsNumber(u)) {
    return P_UNDEF();
  }
  if (u->type() == BlockType::Power) {
    return u->childAtIndex(1)->clone();
  }
  return P_ONE();
}

bool WrapWithUnary(EditionReference* u, const Node* n) {
  CloneNodeBeforeNode(u, n);
  NAry::SetNumberOfChildren(*u, 1);
  return true;
}

bool Reorder(EditionReference* u, EditionReference* v) {
  if (Comparison::Compare(*u, *v) > 0) {
    SwapTrees(u, v);
    return true;
  }
  return false;
}

void MultPopFirst(EditionReference* l) {
  assert(l->type() == BlockType::Multiplication);
  NAry::RemoveChildAtIndex(*l, 0);
}

void MultPushFirst(EditionReference* l, EditionReference* e) {
  assert(l->type() == BlockType::Multiplication);
  NAry::AddChildAtIndex(*l, *e, 0);
}

bool Simplification::SimplifyProductRec(EditionReference* l) {
  if (l->numberOfChildren() != 2) {
    EditionReference u1 = NAry::DetachChildAtIndex(*l, 0);
    SimplifyProductRec(l);
    if (u1.type() == BlockType::Multiplication) {
      /* TODO merge products consume its second children so we can't pass it l
       * which needs to be kept at the same place. But since the order counts we
       * can't pass l as the first argument either. Need merge on right ? */
      EditionReference l2 = l->clone();
      MergeProducts(&u1, &l2);
      ReplaceTreeByTree(l, u1);
      return true;
    }
    WrapWithUnary(&u1, KMult());
    EditionReference l2 = l->clone();
    MergeProducts(&u1, &l2);
    ReplaceTreeByTree(l, u1);
    return true;
  }
  EditionReference u1 = l->childAtIndex(0);
  EditionReference u2 = l->childAtIndex(1);
  if (u1.type() == BlockType::Multiplication ||
      u2.type() == BlockType::Multiplication) {
    l->removeNode();
    if (u1.type() != BlockType::Multiplication) {
      WrapWithUnary(&u1, KMult());
    }
    if (u2.type() != BlockType::Multiplication) {
      WrapWithUnary(&u2, KMult());
    }
    MergeProducts(&u1, &u2);
    *l = u1;
    return true;
  }
  // Merge constants
  if (IsConstant(u1) && IsConstant(u2)) {
    SimplifyRationalTree(l);
    if (l->type() == BlockType::One) {
      CloneNodeOverNode(l, KMult());
      return true;
    }
    WrapWithUnary(l, KMult());
    return true;
  }
  // 1 * u2 -> u2
  if (u1.type() == BlockType::One) {
    NAry::RemoveChildAtIndex(*l, 0);
    return true;
  }
  // u1 * 1 -> u1
  if (u2.type() == BlockType::One) {
    NAry::RemoveChildAtIndex(*l, 1);
    return true;
  }
  EditionReference t1 = PushBase(u1);
  EditionReference t2 = PushBase(u2);
  int basesAreEqual = Comparison::AreEqual(t1, t2);
  t1.removeTree();
  t2.removeTree();
  // t^m * t^n -> t^(m+n)
  if (basesAreEqual) {
    EditionReference P =
        P_POW(PushBase(u1), P_ADD(PushExponent(u1), PushExponent(u2)));
    EditionReference S = P.childAtIndex(1);
    SimplifySum(&S);
    SimplifyPower(&P);
    if (P.type() == BlockType::One) {
      P.removeTree();
      CloneNodeOverTree(l, KMult());
      return true;
    }
    ReplaceTreeByTree(l, P);
    WrapWithUnary(l, KMult());
    return true;
  }
  return Reorder(&u1, &u2);
}

bool Simplification::MergeProducts(EditionReference* p, EditionReference* q) {
  if (q->numberOfChildren() == 0) {
    q->removeNode();
    return true;
  }
  if (p->numberOfChildren() == 0) {
    ReplaceNodeByTree(p, *q);
    return true;
  }
  Node* p1 = p->childAtIndex(0);
  Node* q1 = q->childAtIndex(0);
  EditionReference h = P_MULT(p1->clone(), q1->clone());
  SimplifyProductRec(&h);
  if (h.numberOfChildren() == 0) {
    h.removeNode();
    MultPopFirst(p);
    MultPopFirst(q);
    return MergeProducts(p, q);
  }
  if (h.numberOfChildren() == 1) {
    MultPopFirst(p);
    MultPopFirst(q);
    MergeProducts(p, q);
    ReplaceTreeByTree(&h, h.childAtIndex(0));
    MultPushFirst(p, &h);
    return true;
  }
  if (Comparison::AreEqual(h.childAtIndex(0), p1)) {
    assert(Comparison::AreEqual(h.childAtIndex(1), q1));
    h.removeTree();
    EditionReference pc = p1->clone();
    MultPopFirst(p);
    MergeProducts(p, q);
    MultPushFirst(p, &pc);
    return true;
  }
  if (Comparison::AreEqual(h.childAtIndex(0), q1)) {
    assert(Comparison::AreEqual(h.childAtIndex(1), p1));
    h.removeTree();
    EditionReference qc = q1->clone();
    MultPopFirst(q);
    MergeProducts(p, q);
    MultPushFirst(p, &qc);
    return true;
  }
  assert(false);
}

bool Simplification::SimplifyProduct(EditionReference* u) {
  // ... * 0 * ... -> 0
  if (AnyChildren(*u, IsZero)) {
    CloneNodeOverTree(u, 0_e);
    return true;
  }
  if (NAry::SquashIfUnary(u)) {
    return true;
  }
  if (!SimplifyProductRec(u)) {
    return false;
  }
  NAry::Sanitize(u);
  return true;
}

// The term of 2ab is ab
EditionReference PushTerm(Node* u) {
  if (IsNumber(u)) {
    return P_UNDEF();
  }
  EditionReference c = u->clone();
  if (u->type() == BlockType::Multiplication) {
    if (IsConstant(u->childAtIndex(0))) {
      MultPopFirst(&c);
      return c;
    }
    return c;
  }
  WrapWithUnary(&c, KMult());
  return c;
}

// The constant of 2ab is 2
EditionReference PushConstant(Node* u) {
  if (IsNumber(u)) {
    return P_UNDEF();
  }
  if (u->type() == BlockType::Multiplication &&
      IsConstant(u->childAtIndex(0))) {
    return u->childAtIndex(0)->clone();
  }
  return P_ONE();
}

void AddPopFirst(EditionReference* l) {
  assert(l->type() == BlockType::Addition);
  NAry::RemoveChildAtIndex(*l, 0);
}

void AddPushFirst(EditionReference* l, EditionReference* e) {
  assert(l->type() == BlockType::Addition);
  NAry::AddChildAtIndex(*l, *e, 0);
}

bool Simplification::SimplifySumRec(EditionReference* l) {
  if (l->numberOfChildren() != 2) {
    EditionReference u1 = NAry::DetachChildAtIndex(*l, 0);
    SimplifySumRec(l);
    if (u1.type() == BlockType::Addition) {
      EditionReference l2 = l->clone();
      MergeSums(&u1, &l2);
      ReplaceTreeByTree(l, u1);
      return true;
    }
    WrapWithUnary(&u1, KAdd());
    EditionReference l2 = l->clone();
    MergeSums(&u1, &l2);
    ReplaceTreeByTree(l, u1);
    return true;
  }
  EditionReference u1 = l->childAtIndex(0);
  EditionReference u2 = l->childAtIndex(1);
  if (u1.type() == BlockType::Addition || u2.type() == BlockType::Addition) {
    l->removeNode();
    if (u1.type() != BlockType::Addition) {
      WrapWithUnary(&u1, KAdd());
    }
    if (u2.type() != BlockType::Addition) {
      WrapWithUnary(&u2, KAdd());
    }
    MergeSums(&u1, &u2);
    *l = u1;
    return true;
  }
  // Merge constants
  if (IsConstant(u1) && IsConstant(u2)) {
    SimplifyRationalTree(l);
    if (l->type() == BlockType::Zero) {
      CloneNodeOverNode(l, KAdd());
      return true;
    }
    WrapWithUnary(l, KAdd());
    return true;
  }
  if (u1.type() == BlockType::Zero) {
    ReplaceTreeByTree(l, u2);
    WrapWithUnary(l, KAdd());
    return true;
  }
  if (u2.type() == BlockType::Zero) {
    ReplaceTreeByTree(l, u1);
    WrapWithUnary(l, KAdd());
    return true;
  }
  EditionReference t1 = PushTerm(u1);
  EditionReference t2 = PushTerm(u2);
  int termsAreEqual = Comparison::AreEqual(t1, t2);
  t1.removeTree();
  t2.removeTree();
  // k1 * a + k2 * a -> (k1+k2) * a
  if (termsAreEqual) {
    EditionReference P =
        P_MULT(P_ADD(PushConstant(u1), PushConstant(u2)), PushTerm(u1));
    EditionReference S = P.childAtIndex(0);
    SimplifySum(&S);
    SimplifyProduct(&P);
    if (P.type() == BlockType::Zero) {
      P.removeTree();
      CloneNodeOverTree(l, KAdd());
      return true;
    }
    ReplaceTreeByTree(l, P);
    WrapWithUnary(l, KAdd());
    return true;
  }
  return Reorder(&u1, &u2);
}

bool Simplification::MergeSums(EditionReference* p, EditionReference* q) {
  if (q->numberOfChildren() == 0) {
    q->removeNode();
    return true;
  }
  if (p->numberOfChildren() == 0) {
    ReplaceNodeByTree(p, *q);
    return true;
  }
  Node* p1 = p->childAtIndex(0);
  Node* q1 = q->childAtIndex(0);
  EditionReference h = P_ADD(p1->clone(), q1->clone());
  SimplifySumRec(&h);
  if (h.numberOfChildren() == 0) {
    h.removeNode();
    AddPopFirst(p);
    AddPopFirst(q);
    return MergeSums(p, q);
  }
  if (h.numberOfChildren() == 1) {
    AddPopFirst(p);
    AddPopFirst(q);
    MergeSums(p, q);
    ReplaceTreeByTree(&h, h.childAtIndex(0));
    AddPushFirst(p, &h);
    return true;
  }
  if (Comparison::AreEqual(h.childAtIndex(0), p1)) {
    assert(Comparison::AreEqual(h.childAtIndex(1), q1));
    EditionReference pc = p1->clone();
    h.removeTree();
    AddPopFirst(p);
    MergeSums(p, q);
    AddPushFirst(p, &pc);
    return true;
  }
  if (Comparison::AreEqual(h.childAtIndex(0), q1)) {
    assert(Comparison::AreEqual(h.childAtIndex(1), p1));
    EditionReference qc = q1->clone();
    h.removeTree();
    AddPopFirst(q);
    MergeSums(p, q);
    AddPushFirst(p, &qc);
    return true;
  }
  assert(false);
}

bool Simplification::SimplifySum(EditionReference* u) {
  if (NAry::SquashIfUnary(u)) {
    return true;
  }
  if (SimplifySumRec(u)) {
    NAry::Sanitize(u);
    return true;
  }
  return false;
}

bool Simplification::SimplifyRationalTree(EditionReference* u) {
  if (IsInteger(*u)) {
    return false;
  }
  if (IsRational(*u)) {
    if (Rational::Denominator(*u).isZero()) {
      ReplaceTreeByNode(u, P_UNDEF());
      return true;
    }
    return false;
  }
  if (u->numberOfChildren() == 1) {
    assert(u->type() == BlockType::Addition ||
           u->type() == BlockType::Multiplication);
    ReplaceNodeByTree(u, u->childAtIndex(0));
    return SimplifyRationalTree(u);
  }
  if (u->numberOfChildren() == 2) {
    if (u->type() == BlockType::Addition ||
        u->type() == BlockType::Multiplication) {
      EditionReference v = u->childAtIndex(0);
      SimplifyRationalTree(&v);
      if (IsUndef(v)) {
        CloneNodeOverTree(u, KUndef);
        return true;
      }
      EditionReference w = u->childAtIndex(1);
      SimplifyRationalTree(&w);
      if (IsUndef(w)) {
        CloneNodeOverTree(u, KUndef);
        return true;
      }
      ReplaceTreeByTree(u, (u->type() == BlockType::Addition
                                ? Rational::Addition
                                : Rational::Multiplication)(v, w));
      ReplaceTreeByTree(u, Rational::IrreducibleForm(*u));
      return true;
    }
    if (u->type() == BlockType::Power) {
      EditionReference v = u->childAtIndex(0);
      SimplifyRationalTree(&v);
      if (IsUndef(v)) {
        CloneNodeOverTree(u, KUndef);
        return true;
      }
      assert(IsInteger(u->childAtIndex(1)));
      ReplaceTreeByTree(u, Rational::IntegerPower(v, u->childAtIndex(1)));
      ReplaceTreeByTree(u, Rational::IrreducibleForm(*u));
      return true;
    }
  }
  assert(false);
}

bool Simplification::Simplify(EditionReference* reference) {
  bool changed = false;
  /* TODO: If simplification fails, come back to this step with a simpler
   * projection context. */
  changed = DeepSystemProjection(reference) || changed;
  changed = SystematicReduce(reference) || changed;
  // TODO: Bubble up Matrices, complexes, units, lists and dependencies.
  changed = AdvancedReduction(reference) || changed;
  changed = SystematicReduce(reference) || changed;
  changed = DeepBeautify(reference) || changed;
  return changed;
}

bool Simplification::AdvancedReduction(EditionReference* reference) {
  bool changed = false;
  for (std::pair<EditionReference, int> indexedNode :
       NodeIterator::Children<Forward, Editable>(*reference)) {
    changed =
        AdvancedReduction(&std::get<EditionReference>(indexedNode)) || changed;
  }
  return ShallowAdvancedReduction(reference, changed) || changed;
}

bool Simplification::ShallowAdvancedReduction(EditionReference* reference,
                                              bool change) {
  return (reference->block()->isAlgebraic()
              ? AdvanceReduceOnAlgebraic
              : AdvanceReduceOnTranscendental)(reference, change);
}

// Reverse most system projections to display better expressions
bool Simplification::ShallowBeautify(EditionReference* reference,
                                     void* context) {
  ProjectionContext* projectionContext =
      static_cast<ProjectionContext*>(context);
  if (reference->type() == BlockType::Trig) {
    const Node* k_angles[3] = {
        KPlaceholder<A>(), KMult(KPlaceholder<A>(), 180_e, KPow(π_e, -1_e)),
        KMult(KPlaceholder<A>(), 200_e, KPow(π_e, -1_e))};
    EditionReference child(reference->childAtIndex(0));
    child.matchAndReplace(
        KPlaceholder<A>(),
        k_angles[static_cast<uint8_t>(projectionContext->m_angleUnit)]);
    SystematicReduce(&child);
  }
  return
      // A + B? + (-1)*C + D?-> ((A + B) - C) + D
      reference->matchAndReplace(
          KAdd(KPlaceholder<A>(), KAnyTreesPlaceholder<B>(),
               KMult(-1_e, KAnyTreesPlaceholder<C>()),
               KAnyTreesPlaceholder<D>()),
          KAdd(KSub(KAdd(KPlaceholder<A>(), KPlaceholder<B>()),
                    KMult(KPlaceholder<C>())),
               KPlaceholder<D>())) ||
      // trig(A, 0) -> cos(A)
      reference->matchAndReplace(KTrig(KPlaceholder<A>(), 0_e),
                                 KCos(KPlaceholder<A>())) ||
      // trig(A, 1) -> sin(A)
      reference->matchAndReplace(KTrig(KPlaceholder<A>(), 1_e),
                                 KSin(KPlaceholder<A>())) ||
      // exp(ln(A) * B?) -> A^B
      reference->matchAndReplace(
          KExp(KMult(KLn(KPlaceholder<A>()), KAnyTreesPlaceholder<B>())),
          KPow(KPlaceholder<A>(), KMult(KPlaceholder<B>()))) ||
      // exp(A) -> e^A
      reference->matchAndReplace(KExp(KPlaceholder<A>()),
                                 KPow(e_e, KPlaceholder<A>())) ||
      // ln(A) * ln(B)^(-1) -> log(A, B)
      reference->matchAndReplace(
          KMult(KLn(KPlaceholder<A>()), KPow(KLn(KPlaceholder<B>()), -1_e)),
          KLogarithm(KPlaceholder<A>(), KPlaceholder<B>()));
}

EditionReference Simplification::DistributeMultiplicationOverAddition(
    EditionReference reference) {
  EditionPool* editionPool = EditionPool::sharedEditionPool();
  for (auto [child, index] :
       NodeIterator::Children<Forward, Editable>(reference)) {
    if (child.type() == BlockType::Addition) {
      // Create new addition that will be filled in the following loop
      EditionReference add = EditionReference(
          editionPool->push<BlockType::Addition>(child.numberOfChildren()));
      for (auto [additionChild, additionIndex] :
           NodeIterator::Children<Forward, Editable>(child)) {
        // Copy a multiplication
        EditionReference multCopy = editionPool->clone(reference);
        // Find the addition to be replaced
        EditionReference additionCopy =
            EditionReference(multCopy.childAtIndex(index));
        // Find addition child to replace with
        EditionReference additionChildCopy =
            EditionReference(additionCopy.childAtIndex(additionIndex));
        // Replace addition per its child
        additionCopy.replaceTreeByTree(additionChildCopy);
        assert(multCopy.type() == BlockType::Multiplication);
        DistributeMultiplicationOverAddition(multCopy);
      }
      reference.replaceTreeByTree(add);
      return add;
    }
  }
  return reference;
}

bool Simplification::DeepSystemProjection(EditionReference* reference,
                                          ProjectionContext projectionContext) {
  if (projectionContext.m_strategy == Strategy::ApproximateToFloat) {
    *reference = Approximation::ReplaceWithApproximation(*reference);
  }
  return ApplyShallowInDepth(reference, ShallowSystemProjection,
                             static_cast<void*>(&projectionContext));
}

/* The order of nodes in NAry is not a concern here. They will be sorted before
 * SystemReduction. */
bool Simplification::ShallowSystemProjection(EditionReference* ref,
                                             void* context) {
  /* TODO: Most of the projections could be optimized by simply replacing and
   * inserting nodes. This optimization could be applied in matchAndReplace. See
   * comment in matchAndReplace. */
  ProjectionContext* projectionContext =
      static_cast<ProjectionContext*>(context);
  if (projectionContext->m_strategy == Strategy::NumbersToFloat &&
      ref->block()->isInteger()) {
    *ref = Approximation::ReplaceWithApproximation(*ref);
    return true;
  }

  if (ref->block()->isOfType(
          {BlockType::Sine, BlockType::Cosine, BlockType::Tangent})) {
    const Node* k_angles[3] = {
        KPlaceholder<A>(), KMult(KPlaceholder<A>(), π_e, KPow(180_e, -1_e)),
        KMult(KPlaceholder<A>(), π_e, KPow(200_e, -1_e))};
    EditionReference(ref->childAtIndex(0))
        .matchAndReplace(
            KPlaceholder<A>(),
            k_angles[static_cast<uint8_t>(projectionContext->m_angleUnit)]);
  }

  /* All replaced structure do not not need further shallow projection.
   * Operator || only is used. */
  return
      // A - B -> A + (-1)*B
      ref->matchAndReplace(
          KSub(KPlaceholder<A>(), KPlaceholder<B>()),
          KAdd(KPlaceholder<A>(), KMult(-1_e, KPlaceholder<B>()))) ||
      // A / B -> A * B^-1
      ref->matchAndReplace(
          KDiv(KPlaceholder<A>(), KPlaceholder<B>()),
          KMult(KPlaceholder<A>(), KPow(KPlaceholder<B>(), -1_e))) ||
      // cos(A) -> trig(A, 0)
      ref->matchAndReplace(KCos(KPlaceholder<A>()),
                           KTrig(KPlaceholder<A>(), 0_e)) ||
      // sin(A) -> trig(A, 1)
      ref->matchAndReplace(KSin(KPlaceholder<A>()),
                           KTrig(KPlaceholder<A>(), 1_e)) ||
      // tan(A) -> sin(A) * cos(A)^(-1)
      /* TODO: Tangent will duplicate its yet to be projected children,
       * replacing it after everything else may be an optimization. */
      ref->matchAndReplace(KTan(KPlaceholder<A>()),
                           KMult(KTrig(KPlaceholder<A>(), 1_e),
                                 KPow(KTrig(KPlaceholder<A>(), 0_e), -1_e))) ||
      // log(A, e) -> ln(e)
      ref->matchAndReplace(KLogarithm(KPlaceholder<A>(), e_e),
                           KLn(KPlaceholder<A>())) ||
      // log(A) -> ln(A) * ln(10)^(-1)
      // TODO: Maybe log(A) -> log(A, 10) and rely on next matchAndReplace
      ref->matchAndReplace(
          KLog(KPlaceholder<A>()),
          KMult(KLn(KPlaceholder<A>()), KPow(KLn(10_e), -1_e))) ||
      // log(A, B) -> ln(A) * ln(B)^(-1)
      ref->matchAndReplace(
          KLogarithm(KPlaceholder<A>(), KPlaceholder<B>()),
          KMult(KLn(KPlaceholder<A>()), KPow(KLn(KPlaceholder<B>()), -1_e))) ||
      // Power of non-integers
      // TODO: Maybe add exp(A) -> e^A with A integer
      (ref->type() == BlockType::Power &&
       !ref->nextNode()->nextTree()->block()->isInteger() &&
       (  // e^A -> exp(A)
           ref->matchAndReplace(KPow(e_e, KPlaceholder<A>()),
                                KExp(KPlaceholder<A>())) ||
           // A^B -> exp(ln(A)*B)
           ref->matchAndReplace(
               KPow(KPlaceholder<A>(), KPlaceholder<B>()),
               KExp(KMult(KLn(KPlaceholder<A>()), KPlaceholder<B>())))));
}

bool Simplification::ApplyShallowInDepth(EditionReference* reference,
                                         ShallowOperation shallowOperation,
                                         void* context) {
  bool changed = shallowOperation(reference, context);
  int treesToProject = reference->numberOfChildren();
  Node* node = reference->nextNode();
  while (treesToProject > 0) {
    treesToProject--;
    EditionReference subRef(node);
    changed = shallowOperation(&subRef, context) || changed;
    treesToProject += node->numberOfChildren();
    node = node->nextNode();
  }
  return changed;
}

bool Simplification::AdvanceReduceOnTranscendental(EditionReference* reference,
                                                   bool change) {
  if (change && ReduceInverseFunction(reference)) {
    return true;
  }
  if (ShallowExpand(reference)) {
    // TODO: Define a metric to validate (or not) the contraction
    SystematicReduce(reference);
    ShallowAdvancedReduction(reference, true);
    return true;
  }
  return false;
}

bool Simplification::AdvanceReduceOnAlgebraic(EditionReference* reference,
                                              bool change) {
  if (ShallowContract(reference)) {
    /* TODO: Define a metric to validate (or not) the contraction. It could be :
     * - Number of node decreased
     * - Success of SystematicReduce */
    SystematicReduce(reference);
    return true;
  }
  return ExpandTranscendentalOnRational(reference) +
         ShallowAlgebraicExpand(reference) +
         PolynomialInterpretation(reference);
}

bool Simplification::ReduceInverseFunction(EditionReference* e) {
  // TODO : Add more
  return e->matchAndReplace(KExp(KLn(KPlaceholder<A>())), KPlaceholder<A>()) ||
         e->matchAndReplace(KLn(KExp(KPlaceholder<A>())), KPlaceholder<A>());
}

bool Simplification::ExpandTranscendentalOnRational(EditionReference* e) {
  // TODO : Implement
  return false;
}

bool Simplification::PolynomialInterpretation(EditionReference* e) {
  // TODO : Implement
  return false;
}

bool Simplification::DistributeOverNAry(EditionReference* reference,
                                        BlockType target, BlockType naryTarget,
                                        BlockType naryOutput, int childIndex) {
  assert(naryTarget == BlockType::Addition ||
         naryTarget == BlockType::Multiplication);
  assert(naryOutput == BlockType::Addition ||
         naryOutput == BlockType::Multiplication);
  if (reference->type() != target) {
    return false;
  }
  int numberOfChildren = reference->numberOfChildren();
  assert(childIndex < numberOfChildren);
  EditionReference children = reference->childAtIndex(childIndex);
  if (children.type() != naryTarget) {
    return false;
  }
  EditionPool* editionPool(EditionPool::sharedEditionPool());
  int numberOfGrandChildren = children.numberOfChildren();
  size_t childIndexOffset = children.block() - reference->block();
  // f(+(A,B,C),E)
  children.cloneNodeBeforeNode(0_e);
  children.detachTree();
  // f(0,E) ... +(A,B,C)
  Node* grandChild = children.nextNode();
  EditionReference output =
      naryOutput == BlockType::Addition
          ? editionPool->push<BlockType::Addition>(numberOfGrandChildren)
          : editionPool->push<BlockType::Multiplication>(numberOfGrandChildren);
  // f(0,E) ... +(A,B,C) ... *(,,)
  for (int i = 0; i < numberOfGrandChildren; i++) {
    Node* clone = editionPool->clone(*reference, true);
    // f(0,E) ... +(A,B,C) ... *(f(0,E),,)
    /* Since it is constant, use a childIndexOffset to avoid childAtIndex calls:
     * clone.childAtIndex(childIndex)=Node(clone.block()+childIndexOffset) */
    EditionReference(clone->block() + childIndexOffset)
        .replaceTreeByTree(grandChild);
    // f(0,E) ... +(,B,C) ... *(f(A,E),,)
  }
  // f(0,E) ... +(,,) ... *(f(A,E), f(B,E), f(C,E))
  children.removeNode();
  // f(0,E) ... *(f(A,E), f(B,E), f(C,E))
  *reference = reference->replaceTreeByTree(output);
  // *(f(A,E), f(B,E), f(C,E))
  return true;
}

bool Simplification::TryAllOperations(EditionReference* e,
                                      const Operation* operations,
                                      int numberOfOperations) {
  /* For example :
   * Most contraction operations are very shallow.
   * exp(A)*exp(B)*exp(C)*|D|*|E| = exp(A+B)*exp(C)*|D|*|E|
   *                              = exp(A+B)*exp(C)*|D*E|
   *                              = exp(A+B+C)*|D*E|
   * Most expansion operations have to handle themselves smartly.
   * exp(A+B+C) = exp(A)*exp(B)*exp(C) */
  int failures = 0;
  int i = 0;
  while (failures < numberOfOperations) {
    failures = operations[i % numberOfOperations](e) ? 0 : failures + 1;
    i++;
  }
  return i > numberOfOperations;
}

bool Simplification::ContractAbs(EditionReference* reference) {
  // A*|B|*|C|*D = A*|BC|*D
  return reference->matchAndReplace(
      KMult(KAnyTreesPlaceholder<A>(), KAbs(KPlaceholder<B>()),
            KAbs(KPlaceholder<C>()), KAnyTreesPlaceholder<D>()),
      KMult(KPlaceholder<A>(),
            KAbs(KMult(KPlaceholder<B>(), KAnyTreesPlaceholder<C>())),
            KPlaceholder<D>()));
}

bool Simplification::ExpandAbs(EditionReference* reference) {
  // |A*B*...| = |A|*|B|*...
  return DistributeOverNAry(reference, BlockType::Abs,
                            BlockType::Multiplication,
                            BlockType::Multiplication);
}

bool Simplification::ContractLn(EditionReference* reference) {
  // A? + Ln(B) + Ln(C) + D? = A + ln(BC) + D
  return reference->matchAndReplace(
      KAdd(KAnyTreesPlaceholder<A>(), KLn(KPlaceholder<B>()),
           KLn(KPlaceholder<C>()), KAnyTreesPlaceholder<D>()),
      KAdd(KPlaceholder<A>(),
           KLn(KMult(KPlaceholder<B>(), KAnyTreesPlaceholder<C>())),
           KPlaceholder<D>()));
}

bool Simplification::ExpandLn(EditionReference* reference) {
  // ln(A*B*...) = ln(A) + ln(B) + ...
  return DistributeOverNAry(reference, BlockType::Ln, BlockType::Multiplication,
                            BlockType::Addition);
}

bool Simplification::ExpandExp(EditionReference* reference) {
  // TODO: exp(A?*B) = exp(A)^(B) if B is an integer only
  return
      // exp(A+B+...) = exp(A) * exp(B) * ...
      DistributeOverNAry(reference, BlockType::Exponential, BlockType::Addition,
                         BlockType::Multiplication);
}

bool Simplification::ContractExpMult(EditionReference* reference) {
  // A? * exp(B) * exp(C) * D? = A * exp(B+C) * D
  return reference->matchAndReplace(
      KMult(KAnyTreesPlaceholder<A>(), KExp(KPlaceholder<B>()),
            KExp(KPlaceholder<C>()), KAnyTreesPlaceholder<D>()),
      KMult(KPlaceholder<A>(), KExp(KAdd(KPlaceholder<B>(), KPlaceholder<C>())),
            KPlaceholder<D>()));
}

bool Simplification::ContractExpPow(EditionReference* reference) {
  // exp(A)^B = exp(A*B)
  return reference->matchAndReplace(
      KPow(KExp(KPlaceholder<A>()), KPlaceholder<B>()),
      KExp(KMult(KPlaceholder<A>(), KPlaceholder<B>())));
}

bool Simplification::ExpandTrigonometric(EditionReference* reference) {
  /* Trig(A?+B, C) = Trig(A, 0)*Trig(B, C) + Trig(A, 1)*Trig(B, C-1)
   * ExpandTrigonometric is more complex than other expansions and cannot be
   * factorized with DistributeOverNAry. */
  if (!reference->matchAndReplace(
          KTrig(KAdd(KAnyTreesPlaceholder<A>(), KPlaceholder<B>()),
                KPlaceholder<C>()),
          KAdd(KMult(KTrig(KAdd(KPlaceholder<A>()), 0_e),
                     KTrig(KPlaceholder<B>(), KPlaceholder<C>())),
               KMult(
                   KTrig(KAdd(KPlaceholder<A>()), 1_e),
                   KTrig(KPlaceholder<B>(), KAdd(KPlaceholder<C>(), -1_e)))))) {
    return false;
  }
  EditionReference newTrig1(reference->nextNode()->nextNode());
  EditionReference newMult2(reference->nextNode()->nextTree());
  EditionReference newTrig3(newMult2.nextNode());
  EditionReference newTrig4(newMult2.nextNode()->nextTree());
  // Trig(A, 0) and Trig(A, 1) may be expanded again, do it recursively
  // Addition is expected to have been squashed if unary.
  assert(newTrig1.nextNode()->type() != BlockType::Addition ||
         newTrig1.nextNode()->numberOfChildren() > 1);
  if (ExpandTrigonometric(&newTrig1)) {
    if (!ExpandTrigonometric(&newTrig3)) {
      assert(false);
    }
  }
  /* Shallow reduce last Trig and the multiplication (in case it is opposed).
   * This step must be performed after sub-expansions since SimplifyProduct
   * may invalidate newTrig0 and newTrig3. */
  SimplifyTrig(&newTrig4);
  SimplifyProduct(&newMult2);
  return true;
}

bool Simplification::ContractTrigonometric(EditionReference* reference) {
  // A?+cos(B)^2+C?+sin(D)^2+E? = A + 1 + C + E
  if (reference->matchAndReplace(
          KAdd(KAnyTreesPlaceholder<A>(),
               KPow(KTrig(KPlaceholder<B>(), 0_e), 2_e),
               KAnyTreesPlaceholder<C>(),
               KPow(KTrig(KPlaceholder<D>(), 1_e), 2_e),
               KAnyTreesPlaceholder<E>()),
          KAdd(1_e, KAnyTreesPlaceholder<A>(), KAnyTreesPlaceholder<C>(),
               KAnyTreesPlaceholder<E>()))) {
    return true;
  }
  /* A?*Trig(B, C)*Trig(D, E)*F?
   * = (Trig(B-D, TrigDiff(C,E))*F + Trig(B+D, E+C))*F)*A*0.5
   * F is duplicated in case it contains other Trig trees that could be
   * contracted as well. ContractTrigonometric is therefore more complex than
   * other contractions. It handles nested trees itself. */
  if (!reference->matchAndReplace(
          KMult(KAnyTreesPlaceholder<A>(),
                KTrig(KPlaceholder<B>(), KPlaceholder<C>()),
                KTrig(KPlaceholder<D>(), KPlaceholder<E>()),
                KAnyTreesPlaceholder<F>()),
          KMult(
              KAdd(KMult(KTrig(KAdd(KPlaceholder<B>(),
                                    KMult(-1_e, KPlaceholder<D>())),
                               KTrigDiff(KPlaceholder<C>(), KPlaceholder<E>())),
                         KPlaceholder<F>()),
                   KMult(KTrig(KAdd(KPlaceholder<B>(), KPlaceholder<D>()),
                               KAdd(KPlaceholder<E>(), KPlaceholder<C>())),
                         KPlaceholder<F>())),
              KPlaceholder<A>(), 0.5_e))) {
    return false;
  }
  EditionReference newMult1(reference->nextNode()->nextNode());
  if (newMult1.type() != BlockType::Multiplication) {
    // F is empty, Multiplications have been squashed.
    EditionReference newTrig1 = newMult1;
    EditionReference newTrig2 = newTrig1.nextTree();
    assert(newTrig1.type() == BlockType::Trig &&
           newTrig2.type() == BlockType::Trig);
    SimplifyTrig(&newTrig1);
    SimplifyTrig(&newTrig2);
    return true;
  }
  EditionReference newTrig1(newMult1.nextNode());
  EditionReference newMult2(newMult1.nextTree());
  EditionReference newTrig2(newMult2.nextNode());
  // Shallow reduce new trigs and multiplications (in case one is opposed)
  SimplifyTrig(&newTrig1);
  SimplifyProduct(&newMult1);
  SimplifyTrig(&newTrig2);
  SimplifyProduct(&newMult2);

  // Contract newly created multiplications :
  // - Trig(B-D, TrigDiff(C,E))*F
  if (ContractTrigonometric(&newMult1)) {
    // - Trig(B+D, E+C))*F
    if (!ContractTrigonometric(&newMult2)) {
      assert(false);
    }
  }
  return true;
}

bool Simplification::ExpandMult(EditionReference* reference) {
  // A?*(B?+C)*D? = A*B*D + A*C*D
  if (reference->type() != BlockType::Multiplication) {
    return false;
  }
  Node* child = reference->nextNode();
  int numberOfChildren = reference->numberOfChildren();
  // Find the NAry in children
  int childIndex = 0;
  while (childIndex < numberOfChildren &&
         child->type() != BlockType::Addition) {
    childIndex++;
    child = child->nextTree();
  }
  if (childIndex >= numberOfChildren) {
    return false;
  }
  return DistributeOverNAry(reference, BlockType::Multiplication,
                            BlockType::Addition, BlockType::Addition,
                            childIndex);
}

bool Simplification::ExpandPower(EditionReference* reference) {
  // (A?*B)^C = A^C * B^C is currently in SystematicSimplification
  // (A? + B)^2 = (A^2 + 2*A*B + B^2)
  // TODO: Implement a more general (A + B)^C expand.
  /* This isn't factorized with DistributeOverNAry because of the necessary
   * second term expansion. */
  if (!reference->matchAndReplace(
          KPow(KAdd(KAnyTreesPlaceholder<A>(), KPlaceholder<B>()), 2_e),
          KAdd(KPow(KAdd(KPlaceholder<A>()), 2_e),
               KMult(2_e, KAdd(KPlaceholder<A>()), KPlaceholder<B>()),
               KPow(KPlaceholder<B>(), 2_e)))) {
    return false;
  }
  // A^2 and 2*A*B may be expanded again, do it recursively
  EditionReference newPow(reference->nextNode());
  // Addition is expected to have been squashed if unary.
  assert(newPow.nextNode()->type() != BlockType::Addition ||
         newPow.nextNode()->numberOfChildren() > 1);
  if (ExpandPower(&newPow)) {
    EditionReference newMult(newPow.nextTree());
    ExpandMult(&newMult);
    *reference = NAry::Flatten(*reference);
  }
  return true;
}

}  // namespace PoincareJ

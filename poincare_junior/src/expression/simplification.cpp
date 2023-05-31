#include "simplification.h"

#include <poincare_junior/src/expression/approximation.h>
#include <poincare_junior/src/expression/k_creator.h>
#include <poincare_junior/src/expression/p_pusher.h>
#include <poincare_junior/src/expression/rational.h>
#include <poincare_junior/src/memory/node_iterator.h>
#include <poincare_junior/src/memory/pattern_matching.h>
#include <poincare_junior/src/memory/placeholder.h>
#include <poincare_junior/src/n_ary.h>

#include "number.h"

namespace PoincareJ {

using namespace Placeholders;

bool IsInteger(Node u) { return u.block()->isInteger(); }
bool IsNumber(Node u) { return u.block()->isNumber(); }
bool IsRational(Node u) { return u.block()->isRational(); }
bool IsConstant(Node u) { return IsNumber(u); }
bool IsZero(Node u) { return u.type() == BlockType::Zero; }
bool IsUndef(Node u) { return u.type() == BlockType::Undefined; }

void DropNode(EditionReference* u) {
  Node previousU = *u;
  u->removeNode();
  *u = previousU;
}

bool AnyChildren(Node u, bool test(Node)) {
  for (auto [child, index] : NodeIterator::Children<Forward, NoEditable>(u)) {
    if (test(child)) {
      return true;
    }
  }
  return false;
}

bool AllChildren(Node u, bool test(Node)) {
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
      ReplaceTreeByNode(u, KUndef);
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
    default:
      return childChanged;
  }
}

bool Simplification::SimplifyPower(EditionReference* u) {
  EditionReference v = u->childAtIndex(0);
  EditionReference n = u->childAtIndex(1);
  // 0^n -> 0
  if (v.type() == BlockType::Zero) {
    if (n.type() != BlockType::Zero &&
        Rational::RationalStrictSign(n) == StrictSign::Positive) {
      ReplaceTreeByNode(u, 0_e);
      return true;
    }
    ReplaceTreeByNode(u, KUndef);
    return true;
  }
  // 1^n -> 1
  if (v.type() == BlockType::One) {
    ReplaceTreeByNode(u, 1_e);
    return true;
  }
  if (IsRational(v)) {
    return SimplifyRationalTree(u);
  }
  assert(IsInteger(n));
  // v^0 -> 1
  if (n.type() == BlockType::Zero) {
    ReplaceTreeByNode(u, 1_e);
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
    assert(p.nextTree() == static_cast<Node>(n));
    EditionReference m =
        EditionPool::sharedEditionPool()->push<BlockType::Multiplication>(2);
    InsertNodeBeforeNode(&p, m);
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

EditionReference PushBase(Node u) {
  if (IsNumber(u)) {
    return P_UNDEF();
  }
  if (u.type() == BlockType::Power) {
    return u.childAtIndex(0).clone();
  }
  return u.clone();
}

EditionReference PushExponent(Node u) {
  if (IsNumber(u)) {
    return P_UNDEF();
  }
  if (u.type() == BlockType::Power) {
    return u.childAtIndex(1).clone();
  }
  return P_ONE();
}

bool WrapWithUnary(EditionReference* u, Node n) {
  InsertNodeBeforeNode(u, n);
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
      ReplaceNodeByNode(l, KMult());
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
      ReplaceTreeByNode(l, KMult());
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
  Node p1 = p->childAtIndex(0);
  Node q1 = q->childAtIndex(0);
  EditionReference h = P_MULT(p1.clone(), q1.clone());
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
    EditionReference pc = p1.clone();
    MultPopFirst(p);
    MergeProducts(p, q);
    MultPushFirst(p, &pc);
    return true;
  }
  if (Comparison::AreEqual(h.childAtIndex(0), q1)) {
    assert(Comparison::AreEqual(h.childAtIndex(1), p1));
    h.removeTree();
    EditionReference qc = q1.clone();
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
    ReplaceTreeByNode(u, 0_e);
    return true;
  }
  if (NAry::SquashIfUnary(u)) {
    return true;
  }
  if (!SimplifyProductRec(u)) {
    return false;
  }
  return NAry::Sanitize(u);
}

// The term of 2ab is ab
EditionReference PushTerm(Node u) {
  if (IsNumber(u)) {
    return P_UNDEF();
  }
  EditionReference c = u.clone();
  if (u.type() == BlockType::Multiplication) {
    if (IsConstant(u.childAtIndex(0))) {
      MultPopFirst(&c);
      return c;
    }
    return c;
  }
  WrapWithUnary(&c, KMult());
  return c;
}

// The constant of 2ab is 2
EditionReference PushConstant(Node u) {
  if (IsNumber(u)) {
    return P_UNDEF();
  }
  if (u.type() == BlockType::Multiplication && IsConstant(u.childAtIndex(0))) {
    return u.childAtIndex(0).clone();
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
      ReplaceNodeByNode(l, KAdd());
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
      ReplaceTreeByNode(l, KAdd());
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
  Node p1 = p->childAtIndex(0);
  Node q1 = q->childAtIndex(0);
  EditionReference h = P_ADD(p1.clone(), q1.clone());
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
    EditionReference pc = p1.clone();
    h.removeTree();
    AddPopFirst(p);
    MergeSums(p, q);
    AddPushFirst(p, &pc);
    return true;
  }
  if (Comparison::AreEqual(h.childAtIndex(0), q1)) {
    assert(Comparison::AreEqual(h.childAtIndex(1), p1));
    EditionReference qc = q1.clone();
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
  if (!SimplifySumRec(u)) {
    return false;
  }
  assert(u->type() == BlockType::Addition);
  return NAry::Sanitize(u);
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
        ReplaceTreeByNode(u, KUndef);
        return true;
      }
      EditionReference w = u->childAtIndex(1);
      SimplifyRationalTree(&w);
      if (IsUndef(w)) {
        ReplaceTreeByNode(u, KUndef);
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
        ReplaceTreeByNode(u, KUndef);
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

EditionReference Simplification::SystematicReduction(
    EditionReference reference) {
  // TODO: Macro to automatically generate switch
  switch (reference.type()) {
    case BlockType::Division:
      return DivisionReduction(reference);
    case BlockType::Subtraction:
      return SubtractionReduction(reference);
    case BlockType::Addition:
      ReduceNumbersInNAry(reference, Number::Addition);
      return NAry::SquashIfUnary(reference);
    case BlockType::Multiplication:
      ReduceNumbersInNAry(reference, Number::Multiplication);
      return NAry::SquashIfUnary(reference);
    default:
      return reference;
  }
}

// Reverse most system projections to display better expressions
bool Simplification::ShallowBeautify(EditionReference* reference,
                                     void* context) {
  return
      // A + B? + (-1)*C + D?-> ((A + B) - C) + D
      reference->matchAndReplace(
          KAdd(KPlaceholder<A>(), KAnyTreesPlaceholder<B>(),
               KMult(-1_e, KAnyTreesPlaceholder<C>()),
               KAnyTreesPlaceholder<D>()),
          KAdd(KSub(KAdd(KPlaceholder<A>(), KPlaceholder<B>()),
                    KPlaceholder<C>()),
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

EditionReference Simplification::DivisionReduction(EditionReference reference) {
  assert(reference.type() == BlockType::Division);
  return ProjectionReduction(
      reference,
      []() {
        return EditionPool::sharedEditionPool()
            ->push<BlockType::Multiplication>(2);
      },
      []() {
        return EditionPool::sharedEditionPool()->push<BlockType::Power>();
      });
}

EditionReference Simplification::SubtractionReduction(
    EditionReference reference) {
  assert(reference.type() == BlockType::Subtraction);
  return ProjectionReduction(
      reference,
      []() {
        return EditionPool::sharedEditionPool()->push<BlockType::Addition>(2);
      },
      []() {
        return EditionPool::sharedEditionPool()
            ->push<BlockType::Multiplication>(2);
      });
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

EditionReference Simplification::DeepSystemProjection(
    EditionReference reference, ProjectionContext projectionContext) {
  if (projectionContext == ProjectionContext::ApproximateToFloat) {
    return Approximation::ReplaceWithApproximation(reference);
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
  if (*static_cast<ProjectionContext*>(context) ==
          ProjectionContext::NumbersToFloat &&
      ref->block()->isInteger()) {
    *ref = Approximation::ReplaceWithApproximation(*ref);
    return true;
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
       !ref->nextNode().nextTree().block()->isInteger() &&
       (  // e^A -> exp(A)
          // TODO: Maybe remove it and rely on next matchAndReplace
           ref->matchAndReplace(KPow(e_e, KPlaceholder<A>()),
                                KExp(KPlaceholder<A>())) ||
           // A^B -> exp(ln(A)*B)
           ref->matchAndReplace(
               KPow(KPlaceholder<A>(), KPlaceholder<B>()),
               KExp(KMult(KLn(KPlaceholder<A>()), KPlaceholder<B>())))));
}

void Simplification::ReduceNumbersInNAry(EditionReference reference,
                                         NumberOperation operation) {
  size_t index = 0;
  size_t nbOfChildren = reference.numberOfChildren();
  assert(nbOfChildren > 0);
  EditionReference child0 = reference.nextNode();
  EditionReference child1 = child0.nextTree();
  while (index + 1 < nbOfChildren && child0.block()->isNumber() &&
         child1.block()->isNumber()) {
    EditionReference reducedChild = operation(child0, child1);
    child0 = child0.replaceTreeByTree(reducedChild);
    child1.removeTree();
    child1 = child0.nextTree();
    index++;
  }
  NAry::SetNumberOfChildren(reference, nbOfChildren - index);
}

EditionReference Simplification::ProjectionReduction(
    EditionReference division, Node (*PushProjectedEExpression)(),
    Node (*PushInverse)()) {
  /* Rule a / b --> a * b^-1 (or a - b --> a + b * -1) */
  // Create empty * (or +)
  EditionReference multiplication(PushProjectedEExpression());
  // Get references to children
  assert(division.numberOfChildren() == 2);
  EditionReference childrenReferences[2];
  for (auto [child, index] :
       NodeIterator::Children<Forward, Editable>(division)) {
    childrenReferences[index] = child;
  }
  // Move first child
  multiplication.insertTreeAfterNode(childrenReferences[0]);
  // Create empty ^ (or *)
  EditionReference power(PushInverse());
  // Move second child
  power.insertTreeAfterNode(childrenReferences[1]);
  // Complete: a * b^-1 (or a + b * -1)
  EditionPool::sharedEditionPool()->push<BlockType::IntegerShort>(
      static_cast<int8_t>(-1));
  // Replace single-noded division (or subtraction) by the new multiplication
  // (or addition)
  division.replaceNodeByTree(multiplication);
  return multiplication;
}

EditionReference Simplification::ApplyShallowInDepth(
    EditionReference reference, ShallowOperation shallowOperation,
    void* context) {
  const Node root = reference.block();
  Node node = root;
  int treesToProject = 1;
  while (treesToProject > 0) {
    treesToProject--;
    EditionReference subRef(node);
    shallowOperation(&subRef, context);
    treesToProject += node.numberOfChildren();
    node = node.nextNode();
  }
  return EditionReference(root);
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
  // |AB| = |A|*|B|
  return reference->matchAndReplace(
      KAbs(KMult(KPlaceholder<A>(), KAnyTreesPlaceholder<B>())),
      KMult(KAbs(KPlaceholder<A>()), KAbs(KMult(KPlaceholder<B>()))));
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
  // ln(AB) = Ln(A) + Ln(B)
  return reference->matchAndReplace(
      KLn(KMult(KPlaceholder<A>(), KAnyTreesPlaceholder<B>())),
      KAdd(KLn(KPlaceholder<A>()), KLn(KMult(KPlaceholder<B>()))));
}

bool Simplification::ExpandExp(EditionReference* reference) {
  return
      // e^(A+B+C?) = e^A * e^(B+C)
      reference->matchAndReplace(
          KExp(KAdd(KPlaceholder<A>(), KPlaceholder<B>(),
                    KAnyTreesPlaceholder<C>())),
          KMult(KExp(KPlaceholder<A>()),
                KExp(KAdd(KPlaceholder<B>(), KPlaceholder<C>())))) ||
      // e^ABC? = (e^A)^(BC)
      reference->matchAndReplace(
          KExp(KMult(KPlaceholder<A>(), KPlaceholder<B>(),
                     KAnyTreesPlaceholder<C>())),
          KPow(KExp(KPlaceholder<A>()),
               KMult(KPlaceholder<B>(), KPlaceholder<C>())));
}

bool Simplification::ContractExpMult(EditionReference* reference) {
  // A? * e^B * e^C * D? = A * e^(B+C) * D
  return reference->matchAndReplace(
      KMult(KAnyTreesPlaceholder<A>(), KExp(KPlaceholder<B>()),
            KExp(KPlaceholder<C>()), KAnyTreesPlaceholder<D>()),
      KMult(KPlaceholder<A>(), KExp(KAdd(KPlaceholder<B>(), KPlaceholder<C>())),
            KPlaceholder<D>()));
}

bool Simplification::ContractExpPow(EditionReference* reference) {
  // (e^A)^B = e^AB
  return reference->matchAndReplace(
      KPow(KExp(KPlaceholder<A>()), KPlaceholder<B>()),
      KExp(KMult(KPlaceholder<A>(), KPlaceholder<B>())));
}

bool Simplification::ExpandTrigonometric(EditionReference* reference) {
  // If second element is -1/0/1/2, KTrig is -sin/cos/sin/-cos
  // TODO : Ensure trig second element is reduced before and after.
  // Trig(A+B+C?, D) = Trig(A, D)*Trig(B+C, 0) + Trig(A, D-1)*Trig(B+C, 1)
  return reference->matchAndReplace(
      KTrig(
          KAdd(KPlaceholder<A>(), KPlaceholder<B>(), KAnyTreesPlaceholder<C>()),
          KPlaceholder<D>()),
      KAdd(KMult(KTrig(KPlaceholder<A>(), KPlaceholder<D>()),
                 KTrig(KAdd(KPlaceholder<B>(), KPlaceholder<C>()), 0_e)),
           KMult(KTrig(KPlaceholder<A>(), KAdd(KPlaceholder<D>(), -1_e)),
                 KTrig(KAdd(KPlaceholder<B>(), KPlaceholder<C>()), 1_e))));
}

bool Simplification::ContractTrigonometric(EditionReference* reference) {
  /* KTrigDiff : If booth elements are 1 or both are 0, return 0. 1 Otherwise.
   * TODO: This is the only place this is used. It might not be worth it. */
  // A?*Trig(B, C)*Trig(D, E)*F? = A*0.5*(Trig(B-D, C+E-2CE) + Trig(B+D, E+C))*F
  return reference->matchAndReplace(
      KMult(KAnyTreesPlaceholder<A>(),
            KTrig(KPlaceholder<B>(), KPlaceholder<C>()),
            KTrig(KPlaceholder<D>(), KPlaceholder<E>()),
            KAnyTreesPlaceholder<F>()),
      KMult(KPlaceholder<A>(), 0.5_e,
            KAdd(KTrig(KAdd(KPlaceholder<B>(), KMult(-1_e, KPlaceholder<D>())),
                       KTrigDiff(KPlaceholder<C>(), KPlaceholder<E>())),
                 KTrig(KAdd(KPlaceholder<B>(), KPlaceholder<D>()),
                       KAdd(KPlaceholder<E>(), KPlaceholder<C>()))),
            KPlaceholder<F>()));
}

bool Simplification::ExpandMult(EditionReference* reference) {
  // A?*(B+C)*D? = A*B*D + A*C*D
  return reference->matchAndReplace(
      KMult(KAnyTreesPlaceholder<A>(),
            KAdd(KPlaceholder<B>(), KAnyTreesPlaceholder<C>()),
            KAnyTreesPlaceholder<D>()),
      KAdd(KMult(KPlaceholder<A>(), KPlaceholder<B>(), KPlaceholder<D>()),
           KMult(KPlaceholder<A>(), KAdd(KPlaceholder<C>()),
                 KPlaceholder<D>())));
}

bool Simplification::ExpandPower(EditionReference* reference) {
  return
      // (A*B)^C = A^C * B^C is currently in SystematicSimplification
      // (A + B)^2 = (A^2 + 2*A*B + B^2)
      // TODO: Implement a more general (A + B)^C expand.
      reference->matchAndReplace(
          KPow(KAdd(KPlaceholder<A>(), KAnyTreesPlaceholder<B>()), 2_e),
          KAdd(KPow(KPlaceholder<A>(), 2_e),
               KMult(2_e, KPlaceholder<A>(), KAdd(KPlaceholder<B>())),
               KPow(KAdd(KPlaceholder<B>()), 2_e)));
}

}  // namespace PoincareJ

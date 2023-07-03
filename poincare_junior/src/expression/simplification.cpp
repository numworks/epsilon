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
  for (const Node* child : u->children()) {
    if (test(child)) {
      return true;
    }
  }
  return false;
}

bool AllChildren(Node* u, bool test(const Node*)) {
  for (const Node* child : u->children()) {
    if (!test(child)) {
      return false;
    }
  }
  return true;
}

bool Simplification::SystematicReduce(EditionReference* u) {
  if (IsRational(*u)) {
    MoveTreeOverTree(u, Rational::IrreducibleForm(*u));
    return true;  // TODO
  }
  if (u->numberOfChildren() == 0) {
    return false;
  }

  if (u->type() == BlockType::Multiplication ||
      u->type() == BlockType::Addition) {
    NAry::Flatten(u);
  }

  bool childChanged = false;
  for (auto [child, index] : NodeIterator::Children<Editable>(*u)) {
    childChanged = SystematicReduce(&child) || childChanged;
    if (IsUndef(child)) {
      CloneNodeOverTree(u, KUndef);
      return true;
    }
  }

  if (u->type() == BlockType::Multiplication ||
      u->type() == BlockType::Addition) {
    Node* n = *u;
    NAry::Sort(n);
    *u = n;
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
    MoveTreeOverTree(u, v);
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
    for (auto [w, index] : NodeIterator::Children<Editable>(v)) {
      EditionReference m =
          EditionPool::sharedEditionPool()->push<BlockType::Power>();
      w.clone();
      n.clone();
      MoveTreeOverTree(&w, m);
      SimplifyPower(&w);
    }
    n.removeTree();
    DropNode(u);
    return SimplifyProduct(u);
  }
  return false;
}

Node* PushBase(const Node* u) {
  if (IsNumber(u)) {
    return P_UNDEF();
  }
  if (u->type() == BlockType::Power) {
    return u->childAtIndex(0)->clone();
  }
  return u->clone();
}

Node* PushExponent(const Node* u) {
  if (IsNumber(u)) {
    return P_UNDEF();
  }
  if (u->type() == BlockType::Power) {
    return u->childAtIndex(1)->clone();
  }
  return P_ONE();
}

// returns true if they have been merged in u1
bool Simplification::MergeProductChildren(Node* u1, Node* u2) {
  // Merge constants
  if (IsConstant(u1) && IsConstant(u2)) {
    Node* mult = Rational::Multiplication(u1, u2);
    mult->moveTreeOverTree(Rational::IrreducibleForm(mult));
    u1->moveTreeOverTree(mult);
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
    SimplifyProduct(&P);
    u1->moveTreeOverTree(P);
    return true;
  }
  return false;
}

bool Simplification::SimplifyProduct(EditionReference* u) {
  if (NAry::SquashIfUnary(u)) {
    return true;
  }
  bool markerAdded = false;
  if (u->nextTree()->block() == EditionPool::sharedEditionPool()->lastBlock()) {
    EditionPool::sharedEditionPool()->pushBlock(BlockType::Zero);
    markerAdded = true;
  }
  int n = u->numberOfChildren();
  EditionReference end = u->nextTree();
  Node* child = u->nextNode();
  Node* next;
  while ((next = child->nextTree()) < end) {
    // ... * 0 * ... -> 0
    if (child->type() == BlockType::Zero) {
      NAry::SetNumberOfChildren(*u, n);
      CloneTreeOverTree(u, 0_e);
      if (markerAdded) {
        end.removeNode();
      }
      return true;
    }
    if (child->type() == BlockType::One) {
      child->removeTree();
      n--;
      continue;
    }
    if (MergeProductChildren(child, next)) {
      child->nextTree()->removeTree();
      n--;
    } else {
      child = next;
    }
  }
  if (markerAdded) {
    end.removeNode();
  }
  if (n == u->numberOfChildren()) {
    return false;
  }
  NAry::SetNumberOfChildren(*u, n);
  NAry::Sanitize(u);
  return true;
}

// The term of 2ab is ab
Node* PushTerm(const Node* u) {
  if (IsNumber(u)) {
    return P_UNDEF();
  }
  Node* c = u->clone();
  if (u->type() == BlockType::Multiplication) {
    if (IsConstant(u->childAtIndex(0))) {
      NAry::RemoveChildAtIndex(c, 0);
      NAry::SquashIfUnary(c);
      return c;
    }
    return c;
  }
  return c;
}

// The constant of 2ab is 2
Node* PushConstant(const Node* u) {
  if (IsNumber(u)) {
    return P_UNDEF();
  }
  if (u->type() == BlockType::Multiplication &&
      IsConstant(u->childAtIndex(0))) {
    return u->childAtIndex(0)->clone();
  }
  return P_ONE();
}

// returns true if they have been merged in u1
bool Simplification::MergeSumChildren(Node* u1, Node* u2) {
  // Merge constants
  if (IsConstant(u1) && IsConstant(u2)) {
    Node* add = Rational::Addition(u1, u2);
    add->moveTreeOverTree(Rational::IrreducibleForm(add));
    u1->moveTreeOverTree(add);
    return true;
  }
  EditionReference t1 = PushTerm(u1);
  EditionReference t2 = PushTerm(u2);
  // todo terms are equal
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
    u1->moveTreeOverTree(P);
    return true;
  }
  return false;
}

bool Simplification::SimplifySum(EditionReference* u) {
  if (NAry::SquashIfUnary(u)) {
    return true;
  }
  bool markerAdded = false;
  if (u->nextTree()->block() == EditionPool::sharedEditionPool()->lastBlock()) {
    EditionPool::sharedEditionPool()->pushBlock(BlockType::Zero);
    markerAdded = true;
  }
  int n = u->numberOfChildren();
  EditionReference end = u->nextTree();
  Node* child = u->nextNode();
  Node* next;
  while ((next = child->nextTree()) < end) {
    if (child->type() == BlockType::Zero) {
      child->removeTree();
      n--;
      continue;
    }
    if (MergeSumChildren(child, next)) {
      child->nextTree()->removeTree();
      n--;
    } else {
      child = next;
    }
  }
  if (markerAdded) {
    end.removeNode();
  }
  if (n == u->numberOfChildren()) {
    return false;
  }
  NAry::SetNumberOfChildren(*u, n);
  NAry::Sanitize(u);
  return true;
}

bool Simplification::SimplifyRationalTree(EditionReference* u) {
  if (IsInteger(*u)) {
    return false;
  }
  if (IsRational(*u)) {
    if (Rational::Denominator(*u).isZero()) {
      MoveNodeOverTree(u, P_UNDEF());
      return true;
    }
    return false;
  }
  if (u->numberOfChildren() == 1) {
    assert(u->type() == BlockType::Addition ||
           u->type() == BlockType::Multiplication);
    MoveTreeOverNode(u, u->childAtIndex(0));
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
      MoveTreeOverTree(u, (u->type() == BlockType::Addition
                               ? Rational::Addition
                               : Rational::Multiplication)(v, w));
      MoveTreeOverTree(u, Rational::IrreducibleForm(*u));
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
      MoveTreeOverTree(u, Rational::IntegerPower(v, u->childAtIndex(1)));
      MoveTreeOverTree(u, Rational::IrreducibleForm(*u));
      return true;
    }
  }
  assert(false);
}

bool Simplification::Simplify(EditionReference* ref) {
  bool changed = false;
  /* TODO: If simplification fails, come back to this step with a simpler
   * projection context. */
  changed = DeepSystemProjection(ref) || changed;
  changed = SystematicReduce(ref) || changed;
  // TODO: Bubble up Matrices, complexes, units, lists and dependencies.
  changed = AdvancedReduction(ref) || changed;
  changed = SystematicReduce(ref) || changed;
  changed = DeepBeautify(ref) || changed;
  return changed;
}

bool Simplification::AdvancedReduction(EditionReference* ref) {
  bool changed = false;
  for (std::pair<EditionReference, int> indexedNode :
       NodeIterator::Children<Editable>(*ref)) {
    changed =
        AdvancedReduction(&std::get<EditionReference>(indexedNode)) || changed;
  }
  return ShallowAdvancedReduction(ref, changed) || changed;
}

bool Simplification::ShallowAdvancedReduction(EditionReference* ref,
                                              bool change) {
  return (ref->block()->isAlgebraic()
              ? AdvanceReduceOnAlgebraic
              : AdvanceReduceOnTranscendental)(ref, change);
}

// Reverse most system projections to display better expressions
bool Simplification::ShallowBeautify(EditionReference* ref, void* context) {
  ProjectionContext* projectionContext =
      static_cast<ProjectionContext*>(context);
  if (ref->type() == BlockType::Trig) {
    const Node* k_angles[3] = {
        KPlaceholder<A>(), KMult(KPlaceholder<A>(), 180_e, KPow(π_e, -1_e)),
        KMult(KPlaceholder<A>(), 200_e, KPow(π_e, -1_e))};
    EditionReference child(ref->childAtIndex(0));
    child.matchAndReplace(
        KPlaceholder<A>(),
        k_angles[static_cast<uint8_t>(projectionContext->m_angleUnit)]);
    SystematicReduce(&child);
  }
  bool changed = false;
  // A + B? + (-1)*C + D?-> ((A + B) - C) + D
  // Applied as much as necessary while preserving the order.
  while (ref->matchAndReplace(
      KAdd(KPlaceholder<A>(), KAnyTreesPlaceholder<B>(),
           KMult(-1_e, KAnyTreesPlaceholder<C>()), KAnyTreesPlaceholder<D>()),
      KAdd(KSub(KAdd(KPlaceholder<A>(), KAnyTreesPlaceholder<B>()),
                KMult(KAnyTreesPlaceholder<C>())),
           KAnyTreesPlaceholder<D>()))) {
    changed = true;
  }
  return changed ||
         // trig(A, 0) -> cos(A)
         ref->matchAndReplace(KTrig(KPlaceholder<A>(), 0_e),
                              KCos(KPlaceholder<A>())) ||
         // trig(A, 1) -> sin(A)
         ref->matchAndReplace(KTrig(KPlaceholder<A>(), 1_e),
                              KSin(KPlaceholder<A>())) ||
         // exp(0.5*ln(A)) -> Sqrt(A)
         ref->matchAndReplace(KExp(KMult(KHalf, KLn(KPlaceholder<A>()))),
                              KSqrt(KPlaceholder<A>())) ||
         // exp(A? * ln(B) * C?) -> B^(A*C)
         ref->matchAndReplace(
             KExp(KMult(KAnyTreesPlaceholder<A>(), KLn(KPlaceholder<B>()),
                        KAnyTreesPlaceholder<C>())),
             KPow(KPlaceholder<B>(), KMult(KAnyTreesPlaceholder<A>(),
                                           KAnyTreesPlaceholder<C>()))) ||
         // exp(A) -> e^A
         ref->matchAndReplace(KExp(KPlaceholder<A>()),
                              KPow(e_e, KPlaceholder<A>())) ||
         // ln(A) * ln(B)^(-1) -> log(A, B)
         ref->matchAndReplace(
             KMult(KLn(KPlaceholder<A>()), KPow(KLn(KPlaceholder<B>()), -1_e)),
             KLogarithm(KPlaceholder<A>(), KPlaceholder<B>()));
}

EditionReference Simplification::DistributeMultiplicationOverAddition(
    EditionReference ref) {
  EditionPool* editionPool = EditionPool::sharedEditionPool();
  for (auto [child, index] : NodeIterator::Children<Editable>(ref)) {
    if (child.type() == BlockType::Addition) {
      // Create new addition that will be filled in the following loop
      EditionReference add = EditionReference(
          editionPool->push<BlockType::Addition>(child.numberOfChildren()));
      for (auto [additionChild, additionIndex] :
           NodeIterator::Children<Editable>(child)) {
        // Copy a multiplication
        EditionReference multCopy = editionPool->clone(ref);
        // Find the addition to be replaced
        EditionReference additionCopy =
            EditionReference(multCopy.childAtIndex(index));
        // Find addition child to replace with
        EditionReference additionChildCopy =
            EditionReference(additionCopy.childAtIndex(additionIndex));
        // Replace addition per its child
        additionCopy.moveTreeOverTree(additionChildCopy);
        assert(multCopy.type() == BlockType::Multiplication);
        DistributeMultiplicationOverAddition(multCopy);
      }
      ref.moveTreeOverTree(add);
      return add;
    }
  }
  return ref;
}

bool Simplification::DeepSystemProjection(EditionReference* ref,
                                          ProjectionContext projectionContext) {
  if (projectionContext.m_strategy == Strategy::ApproximateToFloat) {
    *ref = Approximation::ReplaceWithApproximation(*ref);
  }
  return ApplyShallowInDepth(ref, ShallowSystemProjection,
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
      // Sqrt(A) -> exp(0.5*ln(A))
      ref->matchAndReplace(KSqrt(KPlaceholder<A>()),
                           KExp(KMult(KHalf, KLn(KPlaceholder<A>())))) ||
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

bool Simplification::ApplyShallowInDepth(EditionReference* ref,
                                         ShallowOperation shallowOperation,
                                         void* context) {
  bool changed = shallowOperation(ref, context);
  int treesToProject = ref->numberOfChildren();
  Node* node = ref->nextNode();
  while (treesToProject > 0) {
    treesToProject--;
    EditionReference subRef(node);
    changed = shallowOperation(&subRef, context) || changed;
    treesToProject += node->numberOfChildren();
    node = node->nextNode();
  }
  return changed;
}

bool Simplification::AdvanceReduceOnTranscendental(EditionReference* ref,
                                                   bool change) {
  if (change + ReduceInverseFunction(ref)) {
    return true;
  }
  size_t treeSize = ref->treeSize();
  EditionReference tempClone(ref->clone());
  if (ShallowExpand(&tempClone)) {
    SystematicReduce(&tempClone);
    assert(tempClone.block()->isAlgebraic());
    ShallowAdvancedReduction(&tempClone, true);
    // TODO: Decide on the metric to use here. Factor 3 allow (x+y)^2 expansion.
    if (static_cast<const Node*>(tempClone)->treeSize() < 3 * treeSize) {
      // Validate the expansion.
      MoveTreeOverTree(ref, tempClone);
      return true;
    }
  }
  tempClone.removeTree();
  return false;
}

bool Simplification::AdvanceReduceOnAlgebraic(EditionReference* ref,
                                              bool change) {
  size_t treeSize = ref->treeSize();
  EditionReference tempClone(ref->clone());
  if (ShallowContract(&tempClone)) {
    SystematicReduce(&tempClone);
    // TODO: Decide on the metric to use here.
    if (static_cast<const Node*>(tempClone)->treeSize() < 3 * treeSize) {
      // Validate the contraction.
      MoveTreeOverTree(ref, tempClone);
      return true;
    }
    // Reset the clone
    tempClone.removeTree();
    tempClone = ref->clone();
  }
  if (ExpandTranscendentalOnRational(&tempClone) +
      ShallowAlgebraicExpand(&tempClone) +
      PolynomialInterpretation(&tempClone)) {
    // TODO: Decide on the metric to use here.
    if (static_cast<const Node*>(tempClone)->treeSize() < 3 * treeSize) {
      // Validate the contraction.
      MoveTreeOverTree(ref, tempClone);
      return true;
    }
  }
  tempClone.removeTree();
  return false;
}

bool Simplification::ReduceInverseFunction(EditionReference* e) {
  // TODO : Add more
  return e->matchAndReplace(KExp(KLn(KPlaceholder<A>())), KPlaceholder<A>()) ||
         e->matchAndReplace(KLn(KExp(KPlaceholder<A>())), KPlaceholder<A>());
}

bool Simplification::ExpandTranscendentalOnRational(EditionReference* e) {
  // ln(18/5) = 3ln(3)+ln(2)-ln(5)
  // TODO : Implement
  return false;
}

bool Simplification::PolynomialInterpretation(EditionReference* e) {
  // TODO : Implement
  return false;
}

bool Simplification::DistributeOverNAry(EditionReference* ref, BlockType target,
                                        BlockType naryTarget,
                                        BlockType naryOutput, int childIndex) {
  assert(naryTarget == BlockType::Addition ||
         naryTarget == BlockType::Multiplication);
  assert(naryOutput == BlockType::Addition ||
         naryOutput == BlockType::Multiplication);
  if (ref->type() != target) {
    return false;
  }
  int numberOfChildren = ref->numberOfChildren();
  assert(childIndex < numberOfChildren);
  EditionReference children = ref->childAtIndex(childIndex);
  if (children.type() != naryTarget) {
    return false;
  }
  EditionPool* editionPool(EditionPool::sharedEditionPool());
  int numberOfGrandChildren = children.numberOfChildren();
  size_t childIndexOffset = children.block() - ref->block();
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
    Node* clone = editionPool->clone(*ref, true);
    // f(0,E) ... +(A,B,C) ... *(f(0,E),,)
    /* Since it is constant, use a childIndexOffset to avoid childAtIndex calls:
     * clone.childAtIndex(childIndex)=Node(clone.block()+childIndexOffset) */
    EditionReference(clone->block() + childIndexOffset)
        .moveTreeOverTree(grandChild);
    // f(0,E) ... +(,B,C) ... *(f(A,E),,)
  }
  // f(0,E) ... +(,,) ... *(f(A,E), f(B,E), f(C,E))
  children.removeNode();
  // f(0,E) ... *(f(A,E), f(B,E), f(C,E))
  *ref = ref->moveTreeOverTree(output);
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

bool Simplification::ContractAbs(EditionReference* ref) {
  // A*|B|*|C|*D = A*|BC|*D
  return ref->matchAndReplace(
      KMult(KAnyTreesPlaceholder<A>(), KAbs(KPlaceholder<B>()),
            KAbs(KPlaceholder<C>()), KAnyTreesPlaceholder<D>()),
      KMult(KAnyTreesPlaceholder<A>(),
            KAbs(KMult(KPlaceholder<B>(), KPlaceholder<C>())),
            KAnyTreesPlaceholder<D>()));
}

bool Simplification::ExpandAbs(EditionReference* ref) {
  // |A*B*...| = |A|*|B|*...
  return DistributeOverNAry(ref, BlockType::Abs, BlockType::Multiplication,
                            BlockType::Multiplication);
}

bool Simplification::ContractLn(EditionReference* ref) {
  // A? + Ln(B) + Ln(C) + D? = A + ln(BC) + D
  return ref->matchAndReplace(
      KAdd(KAnyTreesPlaceholder<A>(), KLn(KPlaceholder<B>()),
           KLn(KPlaceholder<C>()), KAnyTreesPlaceholder<D>()),
      KAdd(KAnyTreesPlaceholder<A>(),
           KLn(KMult(KPlaceholder<B>(), KPlaceholder<C>())),
           KAnyTreesPlaceholder<D>()));
}

bool Simplification::ExpandLn(EditionReference* ref) {
  // ln(A*B*...) = ln(A) + ln(B) + ...
  return DistributeOverNAry(ref, BlockType::Ln, BlockType::Multiplication,
                            BlockType::Addition);
}

bool Simplification::ExpandExp(EditionReference* ref) {
  // TODO: exp(A?*B) = exp(A)^(B) if B is an integer only
  return
      // exp(A+B+...) = exp(A) * exp(B) * ...
      DistributeOverNAry(ref, BlockType::Exponential, BlockType::Addition,
                         BlockType::Multiplication);
}

bool Simplification::ContractExpMult(EditionReference* ref) {
  // A? * exp(B) * exp(C) * D? = A * exp(B+C) * D
  return ref->matchAndReplace(
      KMult(KAnyTreesPlaceholder<A>(), KExp(KPlaceholder<B>()),
            KExp(KPlaceholder<C>()), KAnyTreesPlaceholder<D>()),
      KMult(KAnyTreesPlaceholder<A>(),
            KExp(KAdd(KPlaceholder<B>(), KPlaceholder<C>())),
            KAnyTreesPlaceholder<D>()));
}

bool Simplification::ContractExpPow(EditionReference* ref) {
  // exp(A)^B = exp(A*B)
  return ref->matchAndReplace(
      KPow(KExp(KPlaceholder<A>()), KPlaceholder<B>()),
      KExp(KMult(KPlaceholder<A>(), KPlaceholder<B>())));
}

bool Simplification::ExpandTrigonometric(EditionReference* ref) {
  /* Trig(A?+B, C) = Trig(A, 0)*Trig(B, C) + Trig(A, 1)*Trig(B, C-1)
   * ExpandTrigonometric is more complex than other expansions and cannot be
   * factorized with DistributeOverNAry. */
  if (!ref->matchAndReplace(
          KTrig(KAdd(KAnyTreesPlaceholder<A>(), KPlaceholder<B>()),
                KPlaceholder<C>()),
          KAdd(KMult(KTrig(KAdd(KAnyTreesPlaceholder<A>()), 0_e),
                     KTrig(KPlaceholder<B>(), KPlaceholder<C>())),
               KMult(
                   KTrig(KAdd(KAnyTreesPlaceholder<A>()), 1_e),
                   KTrig(KPlaceholder<B>(), KAdd(KPlaceholder<C>(), -1_e)))))) {
    return false;
  }
  EditionReference newTrig1(ref->nextNode()->nextNode());
  EditionReference newMult2(ref->nextNode()->nextTree());
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

bool Simplification::ContractTrigonometric(EditionReference* ref) {
  // A?+cos(B)^2+C?+sin(D)^2+E? = A + 1 + C + E
  if (ref->matchAndReplace(
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
  if (!ref->matchAndReplace(
          KMult(KAnyTreesPlaceholder<A>(),
                KTrig(KPlaceholder<B>(), KPlaceholder<C>()),
                KTrig(KPlaceholder<D>(), KPlaceholder<E>()),
                KAnyTreesPlaceholder<F>()),
          KMult(
              KAdd(KMult(KTrig(KAdd(KPlaceholder<B>(),
                                    KMult(-1_e, KPlaceholder<D>())),
                               KTrigDiff(KPlaceholder<C>(), KPlaceholder<E>())),
                         KAnyTreesPlaceholder<F>()),
                   KMult(KTrig(KAdd(KPlaceholder<B>(), KPlaceholder<D>()),
                               KAdd(KPlaceholder<E>(), KPlaceholder<C>())),
                         KAnyTreesPlaceholder<F>())),
              KAnyTreesPlaceholder<A>(), KHalf))) {
    return false;
  }
  EditionReference newMult1(ref->nextNode()->nextNode());
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

bool Simplification::ExpandMult(EditionReference* ref) {
  // A?*(B?+C)*D? = A*B*D + A*C*D
  if (ref->type() != BlockType::Multiplication) {
    return false;
  }
  Node* child = ref->nextNode();
  int numberOfChildren = ref->numberOfChildren();
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
  return DistributeOverNAry(ref, BlockType::Multiplication, BlockType::Addition,
                            BlockType::Addition, childIndex);
}

bool Simplification::ExpandPower(EditionReference* ref) {
  // (A?*B)^C = A^C * B^C is currently in SystematicSimplification
  // (A? + B)^2 = (A^2 + 2*A*B + B^2)
  // TODO: Implement a more general (A + B)^C expand.
  /* This isn't factorized with DistributeOverNAry because of the necessary
   * second term expansion. */
  if (!ref->matchAndReplace(
          KPow(KAdd(KAnyTreesPlaceholder<A>(), KPlaceholder<B>()), 2_e),
          KAdd(KPow(KAdd(KAnyTreesPlaceholder<A>()), 2_e),
               KMult(2_e, KAdd(KAnyTreesPlaceholder<A>()), KPlaceholder<B>()),
               KPow(KPlaceholder<B>(), 2_e)))) {
    return false;
  }
  // A^2 and 2*A*B may be expanded again, do it recursively
  EditionReference newPow(ref->nextNode());
  // Addition is expected to have been squashed if unary.
  assert(newPow.nextNode()->type() != BlockType::Addition ||
         newPow.nextNode()->numberOfChildren() > 1);
  if (ExpandPower(&newPow)) {
    EditionReference newMult(newPow.nextTree());
    ExpandMult(&newMult);
    *ref = NAry::Flatten(*ref);
  }
  return true;
}

}  // namespace PoincareJ

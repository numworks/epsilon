#include "simplification.h"

#include <poincare_junior/src/expression/approximation.h>
#include <poincare_junior/src/expression/k_creator.h>
#include <poincare_junior/src/memory/node_iterator.h>
#include <poincare_junior/src/memory/pattern_matching.h>
#include <poincare_junior/src/memory/placeholder.h>
#include <poincare_junior/src/n_ary.h>

#include "number.h"

namespace PoincareJ {

using namespace Placeholders;

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

bool Simplification::Contract(EditionReference* e) {
  switch (e->type()) {
    case BlockType::Addition:
      // Replace with an Addition, which cannot be contracted further.
      return ContractLn(e);
    case BlockType::Multiplication:
      /* These contract methods replace with a Multiplication.
       * They must be called successively, so a | is used instead of || so that
       * there are all evaluated. */
      return ContractAbs(e) | ContractTrigonometric(e) | ContractExpMult(e);
    case BlockType::Power:
      // Replace with an Exponential, which cannot be contracted further.
      return ContractExpPow(e);
    default:
      return false;
  }
}

bool Simplification::Expand(EditionReference* e) {
  /* None of these Expand methods replace with a BlockType that can be expanded
   * again. Otherwise, one would have to call Expand(e) again upon success. */
  switch (e->type()) {
    case BlockType::Abs:
      return ExpandAbs(e);
    case BlockType::Ln:
      return ExpandLn(e);
    case BlockType::Exponential:
      return ExpandExp(e);
    case BlockType::Trig:
      return ExpandTrigonometric(e);
    default:
      return false;
  }
}

bool Simplification::ContractAbs(EditionReference* reference) {
  // TODO : Ensure NAry children are sorted before and after.
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
  // TODO : Ensure NAry children are sorted before and after.
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
  // TODO : Ensure NAry children are sorted before and after.
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

EditionReference Simplification::SystemProjection(EditionReference reference,
                                                  ProjectionContext context) {
  if (context == ProjectionContext::ApproximateToFloat) {
    return Approximation::ReplaceWithApproximation(reference);
  }
  const Node root = reference.block();
  Node node = root;
  /* TODO: Most of the projections could be optimized by simply replacing and
   * inserting nodes. This optimization could be applied in matchAndReplace. See
   * comment in matchAndReplace. */
  int treesToProject = 1;
  while (treesToProject > 0) {
    treesToProject--;
    BlockType type = node.type();
    EditionReference ref(node);
    if (context == ProjectionContext::NumbersToFloat &&
        ref.block()->isInteger()) {
      ref = Approximation::ReplaceWithApproximation(ref);
      node = node.nextTree();
      continue;
    }
    switch (type) {
      case BlockType::Subtraction:
        ref.matchAndReplace(
            KSub(KPlaceholder<A>(), KPlaceholder<B>()),
            KAdd(KPlaceholder<A>(), KMult(-1_e, KPlaceholder<B>())));
        break;
      case BlockType::Cosine:
        ref.matchAndReplace(KCos(KPlaceholder<A>()),
                            KTrig(KPlaceholder<A>(), 0_e));
        break;
      case BlockType::Sine:
        ref.matchAndReplace(KSin(KPlaceholder<A>()),
                            KTrig(KPlaceholder<A>(), 1_e));
        break;
      case BlockType::Tangent:
        /* TODO: Tangent will duplicate its children, replacing it after
         * everything else may be an optimization. */
        ref.matchAndReplace(KTan(KPlaceholder<A>()),
                            KMult(KTrig(KPlaceholder<A>(), 1_e),
                                  KPow(KTrig(KPlaceholder<A>(), 0_e), -1_e)));
        break;
      case BlockType::Power:
        if (node.nextNode().treeIsIdenticalTo(e_e)) {
          ref.matchAndReplace(KPow(e_e, KPlaceholder<A>()),
                              KExp(KPlaceholder<A>()));
        } else if (!node.nextNode().nextTree().block()->isInteger()) {
          ref.matchAndReplace(
              KPow(KPlaceholder<A>(), KPlaceholder<B>()),
              KExp(KMult(KLn(KPlaceholder<A>()), KPlaceholder<B>())));
        }
        break;
      case BlockType::Logarithm:
        if (!ref.matchAndReplace(KLogarithm(KPlaceholder<A>(), e_e),
                                 KLn(KPlaceholder<A>()))) {
          ref.matchAndReplace(KLogarithm(KPlaceholder<A>(), KPlaceholder<B>()),
                              KMult(KLn(KPlaceholder<A>()),
                                    KPow(KLn(KPlaceholder<B>()), -1_e)));
        }
        break;
      case BlockType::Log:
        ref.matchAndReplace(
            KLog(KPlaceholder<A>()),
            KMult(KLn(KPlaceholder<A>()), KPow(KLn(10_e), -1_e)));
        break;
      default:
        break;
    }
    treesToProject += node.numberOfChildren();
    node = node.nextNode();
  }
  return EditionReference(root);
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

}  // namespace PoincareJ

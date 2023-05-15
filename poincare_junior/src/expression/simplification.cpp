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

EditionReference Simplification::ContractAbs(EditionReference reference) {
  Node AbsExpanded = KMult(KAnyTreesPlaceholder<A>(), KAbs(KPlaceholder<B>()),
                           KAbs(KPlaceholder<C>()), KAnyTreesPlaceholder<D>());
  Node AbsContracted =
      KMult(KPlaceholder<A>(),
            KAbs(KMult(KPlaceholder<B>(), KAnyTreesPlaceholder<C>())),
            KPlaceholder<D>());
  reference.matchAndReplace(AbsExpanded, AbsContracted);
  return reference;
}

EditionReference Simplification::ExpandAbs(EditionReference reference) {
  Node AbsContracted =
      KAbs(KMult(KPlaceholder<A>(), KAnyTreesPlaceholder<B>()));
  Node AbsExpanded =
      KMult(KAbs(KPlaceholder<A>()), KAbs(KMult(KPlaceholder<B>())));
  reference.matchAndReplace(AbsContracted, AbsExpanded);
  return reference;
}

EditionReference Simplification::ContractLn(EditionReference reference) {
  Node LnExpanded = KAdd(KAnyTreesPlaceholder<A>(), KLn(KPlaceholder<B>()),
                         KLn(KPlaceholder<C>()), KAnyTreesPlaceholder<D>());
  Node LnContracted =
      KAdd(KPlaceholder<A>(),
           KLn(KMult(KPlaceholder<B>(), KAnyTreesPlaceholder<C>())),
           KPlaceholder<D>());
  reference.matchAndReplace(LnExpanded, LnContracted);
  return reference;
}

EditionReference Simplification::ExpandLn(EditionReference reference) {
  Node LnContracted = KLn(KMult(KPlaceholder<A>(), KAnyTreesPlaceholder<B>()));
  Node LnExpanded = KAdd(KLn(KPlaceholder<A>()), KLn(KMult(KPlaceholder<B>())));
  reference.matchAndReplace(LnContracted, LnExpanded);
  return reference;
}

EditionReference Simplification::ExpandExp(EditionReference reference) {
  Node expMulContracted = KExp(
      KAdd(KPlaceholder<A>(), KPlaceholder<B>(), KAnyTreesPlaceholder<C>()));
  Node expMulExpanded = KMult(KExp(KPlaceholder<A>()),
                              KExp(KAdd(KPlaceholder<B>(), KPlaceholder<C>())));
  reference.matchAndReplace(expMulContracted, expMulExpanded);

  Node expExpContracted = KExp(
      KMult(KPlaceholder<A>(), KPlaceholder<B>(), KAnyTreesPlaceholder<C>()));
  Node expExpExpanded = KPow(KExp(KPlaceholder<A>()),
                             KMult(KPlaceholder<B>(), KPlaceholder<C>()));
  reference.matchAndReplace(expExpContracted, expExpExpanded);
  return reference;
}

EditionReference Simplification::ContractExp(EditionReference reference) {
  Node expMulExpanded =
      KMult(KAnyTreesPlaceholder<C>(), KExp(KPlaceholder<A>()),
            KAnyTreesPlaceholder<D>(), KExp(KPlaceholder<B>()),
            KAnyTreesPlaceholder<E>());
  Node expMulContracted =
      KMult(KPlaceholder<C>(), KExp(KAdd(KPlaceholder<A>(), KPlaceholder<B>())),
            KPlaceholder<D>(), KPlaceholder<E>());
  reference.matchAndReplace(expMulExpanded, expMulContracted);

  Node expExpExpanded = KPow(KExp(KPlaceholder<A>()), KPlaceholder<B>());
  Node expExpContracted = KExp(KMult(KPlaceholder<A>(), KPlaceholder<B>()));
  reference.matchAndReplace(expExpExpanded, expExpContracted);
  return reference;
}

EditionReference Simplification::ExpandTrigonometric(
    EditionReference reference) {
  /* KTrig : If second element is __, return ___ :
   * (-1,-sin),(0,cos),(1,sin),(2,-cos) */
  Node contracted = KTrig(
      KAdd(KPlaceholder<A>(), KPlaceholder<B>(), KAnyTreesPlaceholder<C>()),
      KPlaceholder<D>());
  Node expanded =
      KAdd(KMult(KTrig(KPlaceholder<A>(), KPlaceholder<D>()),
                 KTrig(KAdd(KPlaceholder<B>(), KPlaceholder<C>()), 0_e)),
           KMult(KTrig(KPlaceholder<A>(), KAdd(KPlaceholder<D>(), -1_e)),
                 KTrig(KAdd(KPlaceholder<B>(), KPlaceholder<C>()), 1_e)));
  reference.matchAndReplace(contracted, expanded);
  // TODO: If replaced, simplify resulting KTrigs
  return reference;
}

EditionReference Simplification::ContractTrigonometric(
    EditionReference reference) {
  Node expanded = KMult(
      KAnyTreesPlaceholder<E>(), KTrig(KPlaceholder<A>(), KPlaceholder<C>()),
      KAnyTreesPlaceholder<F>(), KTrig(KPlaceholder<B>(), KPlaceholder<D>()),
      KAnyTreesPlaceholder<G>());
  /* KTrigDiff : If booth elements are 1 or both are 0, return 0. 1 Otherwise.
   * TODO: This is the only place this is used. It might not be worth it.  */
  Node contracted =
      KMult(KPlaceholder<E>(), 0.5_e,
            KAdd(KTrig(KAdd(KPlaceholder<A>(), KMult(-1_e, KPlaceholder<B>())),
                       KTrigDiff(KPlaceholder<C>(), KPlaceholder<D>())),
                 KTrig(KAdd(KPlaceholder<A>(), KPlaceholder<B>()),
                       KAdd(KPlaceholder<D>(), KPlaceholder<C>()))),
            KPlaceholder<F>(), KPlaceholder<G>());
  reference.matchAndReplace(expanded, contracted);
  // TODO: If replaced, simplify resulting KTrigs
  return reference;
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

#include "simplification.h"

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

EditionReference Simplification::ExpandExp(EditionReference reference) {
  Node expMulContracted = KExp(KPlaceholder<A, FilterAddition>());
  Node expMulExpanded = KMult(KExp(KPlaceholder<A, FilterFirstChild>()),
                              KExp(KPlaceholder<A, FilterNonFirstChild>()));
  reference = reference.matchAndReplace(expMulContracted, expMulExpanded);
  Node expExpContracted = KExp(KPlaceholder<A, FilterMultiplication>());
  Node expExpExpanded = KPow(KExp(KPlaceholder<A, FilterFirstChild>()),
                             KPlaceholder<A, FilterNonFirstChild>());
  reference = reference.matchAndReplace(expExpContracted, expExpExpanded);
  return reference;
}

EditionReference Simplification::ContractExp(EditionReference reference) {
  /* TODO : Make it so that we could have this with KPlaceholder<C>() being any
   *        (or none) other children at any place in the KMult :
   * Node expMulExpanded = KMult(KExp(KPlaceholder<A>()),
   *                             KExp(KPlaceholder<B>()), KPlaceholder<C>());
   * Node expMulContracted = KMult(
   *  KExp(KAdd(KPlaceholder<A>(), KPlaceholder<B>())), KPlaceholder<C>());
   */
  Node expMulExpanded = KMult(KExp(KPlaceholder<A>()), KExp(KPlaceholder<B>()));
  Node expMulContracted = KExp(KAdd(KPlaceholder<A>(), KPlaceholder<B>()));
  reference = reference.matchAndReplace(expMulExpanded, expMulContracted);
  Node expExpExpanded = KPow(KExp(KPlaceholder<A>()), KPlaceholder<B>());
  Node expExpContracted = KExp(KMult(KPlaceholder<A>(), KPlaceholder<B>()));
  return reference.matchAndReplace(expExpExpanded, expExpContracted);
}

EditionReference Simplification::ExpandTrigonometric(
    EditionReference reference) {
  /* KTrig : If second element is __, return ___ :
   * (-1,-sin),(0,cos),(1,sin),(2,-cos) */
  Node contracted = KTrig(KPlaceholder<A, FilterAddition>(), KPlaceholder<C>());
  Node expanded =
      KAdd(KMult(KTrig(KPlaceholder<A, FilterFirstChild>(), KPlaceholder<C>()),
                 KTrig(KPlaceholder<A, FilterNonFirstChild>(), 0_e)),
           KMult(KTrig(KPlaceholder<A, FilterFirstChild>(),
                       KAdd(KPlaceholder<C>(), -1_e)),
                 KTrig(KPlaceholder<A, FilterNonFirstChild>(), 1_e)));
  return reference.matchAndReplace(contracted, expanded);
  // TODO: If replaced, simplify resulting KTrigs
}

EditionReference Simplification::ContractTrigonometric(
    EditionReference reference) {
  Node expanded = KMult(KTrig(KPlaceholder<A>(), KPlaceholder<C>()),
                        KTrig(KPlaceholder<B>(), KPlaceholder<D>()));
  // (C+D-2CD) Is equivalent to (C!=D) when C and D are either 0 or 1
  Node contracted =
      KMult(0.5_e,
            KAdd(KTrig(KAdd(KPlaceholder<A>(), KMult(-1_e, KPlaceholder<B>())),
                       KAdd(KPlaceholder<C>(), KPlaceholder<D>(),
                            KMult(-2_e, KPlaceholder<C>(), KPlaceholder<D>()))),
                 KTrig(KAdd(KPlaceholder<A>(), KPlaceholder<B>()),
                       KAdd(KPlaceholder<D>(), KPlaceholder<C>()))));
  return reference.matchAndReplace(expanded, contracted);
  // TODO: If replaced, simplify resulting KTrigs
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

EditionReference Simplification::SystemProjection(EditionReference reference) {
  // Use an EditionReference to track the end since the tree is being edited.
  EditionReference nextTree = reference.nextTree();
  const Node root = reference.block();
  Node node = root;
  /* TODO: Most of the projections could be optimized by simply replacing and
   * inserting nodes. This optimization could be applied in matchAndReplace. See
   * comment in matchAndReplace. */
  while (nextTree.block() > node.block()) {
    BlockType type = node.type();
    EditionReference ref(node);
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
        ref = ref.matchAndReplace(
            KTan(KPlaceholder<A>()),
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
        ref.matchAndReplace(KLogarithm(KPlaceholder<A>(), e_e),
                            KLn(KPlaceholder<A>()))
            .matchAndReplace(KLogarithm(KPlaceholder<A>(), KPlaceholder<B>()),
                             KMult(KLn(KPlaceholder<A>()),
                                   KPow(KLn(KPlaceholder<B>()), -1_e)));
        break;
      case BlockType::Log:
        ref.matchAndReplace(
            KLog(KPlaceholder<A>()),
            KMult(KLn(KPlaceholder<A>()), KPow(KLn(10_e), -1_e)));
        break;
      default:
        break;
    }
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

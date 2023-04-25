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
  Node expMulContracted = KPow(e_e, KPlaceholder<A, FilterAddition>());
  Node expMulExpanded =
      KMult(KPow(e_e, KPlaceholder<A, FilterFirstChild>()),
            KPow(e_e, KPlaceholder<A, FilterExcludeFirstChild>()));
  reference = reference.matchAndReplace(expMulContracted, expMulExpanded);
  Node expExpContracted = KPow(e_e, KPlaceholder<A, FilterMultiplication>());
  Node expExpExpanded = KPow(KPow(e_e, KPlaceholder<A, FilterFirstChild>()),
                             KPlaceholder<A, FilterExcludeFirstChild>());
  reference = reference.matchAndReplace(expExpContracted, expExpExpanded);
  return reference;
}

EditionReference Simplification::ContractExp(EditionReference reference) {
  /* TODO : Make it so that we could have this with KPlaceholder<C>() being any
   *        (or none) other children at any place in the KMult :
   * Node expMulExpanded = KMult(KPow(e_e, KPlaceholder<A>()),
   *                           KPow(e_e, KPlaceholder<B>()), KPlaceholder<C>());
   * Node expMulContracted = KMult(
   *  KPow(e_e, KAdd(KPlaceholder<A>(), KPlaceholder<B>())), KPlaceholder<C>());
   */
  Node expMulExpanded =
      KMult(KPow(e_e, KPlaceholder<A>()), KPow(e_e, KPlaceholder<B>()));
  Node expMulContracted = KPow(e_e, KAdd(KPlaceholder<A>(), KPlaceholder<B>()));
  reference = reference.matchAndReplace(expMulExpanded, expMulContracted);
  Node expExpExpanded = KPow(KPow(e_e, KPlaceholder<A>()), KPlaceholder<B>());
  Node expExpContracted =
      KPow(e_e, KMult(KPlaceholder<A>(), KPlaceholder<B>()));
  reference = reference.matchAndReplace(expExpExpanded, expExpContracted);
  return reference;
}

EditionReference Simplification::ExpandTrigonometric(
    EditionReference reference) {
  Node sinContracted = KSin(KPlaceholder<A, FilterAddition>());
  Node sinExpanded =
      KAdd(KMult(KSin(KPlaceholder<A, FilterFirstChild>()),
                 KCos(KPlaceholder<A, FilterExcludeFirstChild>())),
           KMult(KCos(KPlaceholder<A, FilterFirstChild>()),
                 KSin(KPlaceholder<A, FilterExcludeFirstChild>())));
  reference = reference.matchAndReplace(sinContracted, sinExpanded);
  Node cosContracted = KCos(KPlaceholder<A, FilterAddition>());
  Node cosExpanded =
      KAdd(KMult(KCos(KPlaceholder<A, FilterFirstChild>()),
                 KCos(KPlaceholder<A, FilterExcludeFirstChild>())),
           KMult(-1_e, KSin(KPlaceholder<A, FilterFirstChild>()),
                 KSin(KPlaceholder<A, FilterExcludeFirstChild>())));
  reference = reference.matchAndReplace(cosContracted, cosExpanded);
  return reference;
}

EditionReference Simplification::ContractTrigonometric(
    EditionReference reference) {
  /* TODO : Similarly to ContractExp's comment, make it so that we could
   *        handle any (or none) other children at any place in this KMult. */
  Node sinSinExpanded = KMult(KSin(KPlaceholder<A>()), KSin(KPlaceholder<B>()));
  Node sinSinContracted = KMult(
      0.5_e,
      KAdd(KCos(KAdd(KPlaceholder<A>(), KMult(-1_e, KPlaceholder<B>()))),
           KMult(-1_e, KCos(KAdd(KPlaceholder<A>(), KPlaceholder<B>())))));
  reference = reference.matchAndReplace(sinSinExpanded, sinSinContracted);
  Node cosCosExpanded = KMult(KCos(KPlaceholder<A>()), KCos(KPlaceholder<B>()));
  Node cosCosContracted = KMult(
      0.5_e, KAdd(KCos(KAdd(KPlaceholder<A>(), KMult(-1_e, KPlaceholder<B>()))),
                  KCos(KAdd(KPlaceholder<A>(), KPlaceholder<B>()))));
  reference = reference.matchAndReplace(cosCosExpanded, cosCosContracted);
  Node sinCosExpanded = KMult(KSin(KPlaceholder<A>()), KCos(KPlaceholder<B>()));
  Node sinCosContracted = KMult(
      0.5_e, KAdd(KSin(KAdd(KPlaceholder<A>(), KMult(-1_e, KPlaceholder<B>()))),
                  KSin(KAdd(KPlaceholder<A>(), KPlaceholder<B>()))));
  reference = reference.matchAndReplace(sinCosExpanded, sinCosContracted);
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

#include "simplification.h"

#include <poincare_junior/src/expression/k_creator.h>
#include <poincare_junior/src/memory/node_iterator.h>
#include <poincare_junior/src/memory/pattern_matching.h>
#include <poincare_junior/src/n_ary.h>

#include "number.h"

namespace PoincareJ {

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
    child0.replaceTreeByTree(reducedChild);
    child0 = reducedChild;
    child1.removeTree();
    child1 = child0.nextTree();
    index++;
  }
  NAry::SetNumberOfChildren(reference, nbOfChildren - index);
}

EditionReference Simplification::ExpandReduction(EditionReference reference) {
  Node expMulContracted = KPow(e_e, AAdd_e);
  Node expMulExpanded = KMult(KPow(e_e, A1_e), KPow(e_e, A2_e));

  Node expExpContracted = KPow(e_e, AMul_e);
  Node expExpExpanded = KPow(KPow(e_e, A1_e), A2_e);

  Node sinContracted = KSin(AAdd_e);
  Node sinExpanded =
      KAdd(KMult(KSin(A1_e), KCos(A2_e)), KMult(KCos(A1_e), KSin(A2_e)));

  Node cosContracted = KCos(AAdd_e);
  Node cosExpanded =
      KSub(KMult(KCos(A1_e), KCos(A2_e)), KMult(KSin(A1_e), KSin(A2_e)));

  reference = reference.matchAndReplace(expMulContracted, expMulExpanded);
  reference = reference.matchAndReplace(expExpContracted, expExpExpanded);
  reference = reference.matchAndReplace(sinContracted, sinExpanded);
  reference = reference.matchAndReplace(cosContracted, cosExpanded);
  return reference;
}

EditionReference Simplification::ContractReduction(EditionReference reference) {
  /* TODO : Make it so that we could have this with C_e being any (or none)
   *        other children at any place in the KMult.
   * Node expMulExpanded = KMult(KPow(e_e, A_e), KPow(e_e, B_e), C_e);
   * Node expMulContracted = KMult(KPow(e_e, KAdd(A_e, B_e)), C_e);
   */

  Node expMulExpanded = KMult(KPow(e_e, A_e), KPow(e_e, B_e));
  Node expMulContracted = KPow(e_e, KAdd(A_e, B_e));

  Node expExpExpanded = KPow(KPow(e_e, A_e), B_e);
  Node expExpContracted = KPow(e_e, KMult(A_e, B_e));

  Node sinSinExpanded = KMult(KSin(A_e), KSin(B_e));
  Node sinSinContracted =
      KDiv(KSub(KCos(KSub(A_e, B_e)), KCos(KAdd(A_e, B_e))), 2_e);

  Node cosCosExpanded = KMult(KCos(A_e), KCos(B_e));
  Node cosCosContracted =
      KDiv(KAdd(KCos(KSub(A_e, B_e)), KCos(KAdd(A_e, B_e))), 2_e);

  Node sinCosExpanded = KMult(KSin(A_e), KCos(B_e));
  Node sinCosContracted =
      KDiv(KAdd(KSin(KSub(A_e, B_e)), KSin(KAdd(A_e, B_e))), 2_e);

  reference = reference.matchAndReplace(expMulExpanded, expMulContracted);
  reference = reference.matchAndReplace(expExpExpanded, expExpContracted);
  reference = reference.matchAndReplace(sinSinExpanded, sinSinContracted);
  reference = reference.matchAndReplace(cosCosExpanded, cosCosContracted);
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

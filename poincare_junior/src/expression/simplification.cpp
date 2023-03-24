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

Node expContracted1 = KPow(e_e, KAdd(Placeholders::A, Placeholders::B));
Node expExpanded1 =
    KMult(KPow(e_e, Placeholders::A), KPow(e_e, Placeholders::B));

Node expContracted2 = KPow(e_e, KMult(Placeholders::A, Placeholders::B));
Node expExpanded2 = KPow(KPow(e_e, Placeholders::A), Placeholders::B);

Node sinContracted = KSin(KAdd(Placeholders::A, Placeholders::B));
Node sinExpanded = KAdd(KMult(KSin(Placeholders::A), KCos(Placeholders::B)),
                        KMult(KCos(Placeholders::A), KSin(Placeholders::B)));

Node cosContracted = KCos(KAdd(Placeholders::A, Placeholders::B));
Node cosExpanded = KSub(KMult(KCos(Placeholders::A), KCos(Placeholders::B)),
                        KMult(KSin(Placeholders::A), KSin(Placeholders::B)));

Node sinSinContracted = KDiv(KSub(KCos(KSub(Placeholders::A, Placeholders::B)),
                                  KCos(KAdd(Placeholders::A, Placeholders::B))),
                             2_e);
Node sinSinExpanded = KMult(KSin(Placeholders::A), KSin(Placeholders::B));

Node cosCosContracted = KDiv(KAdd(KCos(KSub(Placeholders::A, Placeholders::B)),
                                  KCos(KAdd(Placeholders::A, Placeholders::B))),
                             2_e);
Node cosCosExpanded = KMult(KCos(Placeholders::A), KCos(Placeholders::B));

Node sinCosContracted = KDiv(KAdd(KSin(KSub(Placeholders::A, Placeholders::B)),
                                  KSin(KAdd(Placeholders::A, Placeholders::B))),
                             2_e);
Node sinCosExpanded = KMult(KSin(Placeholders::A), KCos(Placeholders::B));

EditionReference Simplification::ContractReduction(EditionReference reference) {
  reference = reference.matchAndReplace(expExpanded1, expContracted1);
  reference = reference.matchAndReplace(expExpanded2, expContracted2);
  reference = reference.matchAndReplace(sinSinExpanded, sinSinContracted);
  reference = reference.matchAndReplace(cosCosExpanded, cosCosContracted);
  reference = reference.matchAndReplace(sinCosExpanded, sinCosContracted);
  return reference;
}

EditionReference Simplification::ExpandReduction(EditionReference reference) {
  reference = reference.matchAndReplace(expContracted1, expExpanded1);
  reference = reference.matchAndReplace(expContracted2, expExpanded2);
  reference = reference.matchAndReplace(sinContracted, sinExpanded);
  reference = reference.matchAndReplace(cosContracted, cosExpanded);
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

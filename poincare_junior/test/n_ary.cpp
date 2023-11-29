#include <poincare_junior/src/expression/k_tree.h>
#include <poincare_junior/src/memory/edition_reference.h>
#include <poincare_junior/src/n_ary.h>
#include <quiz.h>

#include "helper.h"

using namespace PoincareJ;

QUIZ_CASE(pcj_n_ary_manipulation) {
  EditionReference rackLayout1 =
      SharedEditionPool->push<BlockType::RackLayout>(3);
  SharedEditionPool->push<BlockType::CodePointLayout, CodePoint>('x');
  SharedEditionPool->push(BlockType::VerticalOffsetLayout);
  SharedEditionPool->push(0);
  SharedEditionPool->push<BlockType::CodePointLayout, CodePoint>('2');
  SharedEditionPool->push<BlockType::CodePointLayout, CodePoint>('+');
  // rackLayout1 is x^2+

  EditionReference rackLayout2 =
      SharedEditionPool->push<BlockType::RackLayout>(3);
  SharedEditionPool->push<BlockType::CodePointLayout, CodePoint>('-');
  SharedEditionPool->push<BlockType::CodePointLayout, CodePoint>('4');
  SharedEditionPool->push<BlockType::CodePointLayout, CodePoint>('3');
  // rackLayout2 is -43

  EditionReference four = NAry::DetachChildAtIndex(rackLayout2, 1);
  // rackLayout2 is -3
  NAry::AddChildAtIndex(rackLayout1, four, 3);
  // rackLayout1 is x^2+4
  NAry::AddOrMergeChildAtIndex(rackLayout1, rackLayout2, 2);
  // rackLayout1 is x^2-3+4
  NAry::RemoveChildAtIndex(rackLayout1, 4);
  // rackLayout1 is x^2-34

  EditionReference rackLayout3 =
      SharedEditionPool->push<BlockType::RackLayout>(5);
  SharedEditionPool->push<BlockType::CodePointLayout, CodePoint>('x');
  SharedEditionPool->push(BlockType::VerticalOffsetLayout);
  SharedEditionPool->push(0);
  SharedEditionPool->push<BlockType::CodePointLayout, CodePoint>('2');
  SharedEditionPool->push<BlockType::CodePointLayout, CodePoint>('-');
  SharedEditionPool->push<BlockType::CodePointLayout, CodePoint>('3');
  SharedEditionPool->push<BlockType::CodePointLayout, CodePoint>('4');
  // rackLayout3 is x^2-34

  assert_trees_are_equal(rackLayout1, rackLayout3);

  EditionReference addition1 = SharedEditionPool->push<BlockType::Addition>(3);
  SharedEditionPool->push<BlockType::IntegerShort>(static_cast<int8_t>(1));
  SharedEditionPool->push<BlockType::Addition>(3);
  SharedEditionPool->push<BlockType::Multiplication>(2);
  SharedEditionPool->push<BlockType::IntegerShort>(static_cast<int8_t>(2));
  SharedEditionPool->push<BlockType::IntegerShort>(static_cast<int8_t>(3));
  SharedEditionPool->push<BlockType::IntegerShort>(static_cast<int8_t>(4));
  SharedEditionPool->push<BlockType::IntegerShort>(static_cast<int8_t>(5));
  SharedEditionPool->push<BlockType::Addition>(2);
  SharedEditionPool->push<BlockType::Addition>(1);
  SharedEditionPool->push<BlockType::IntegerShort>(static_cast<int8_t>(6));
  SharedEditionPool->push<BlockType::IntegerShort>(static_cast<int8_t>(7));
  // addition1 is 1+(2*3+4+5)+((+6)+7)
  NAry::Flatten(addition1);
  // addition1 is 1+2*3+4+5+6+7

  EditionReference addition2 = SharedEditionPool->push<BlockType::Addition>(6);
  SharedEditionPool->push<BlockType::IntegerShort>(static_cast<int8_t>(1));
  SharedEditionPool->push<BlockType::Multiplication>(2);
  SharedEditionPool->push<BlockType::IntegerShort>(static_cast<int8_t>(2));
  SharedEditionPool->push<BlockType::IntegerShort>(static_cast<int8_t>(3));
  SharedEditionPool->push<BlockType::IntegerShort>(static_cast<int8_t>(4));
  SharedEditionPool->push<BlockType::IntegerShort>(static_cast<int8_t>(5));
  SharedEditionPool->push<BlockType::IntegerShort>(static_cast<int8_t>(6));
  SharedEditionPool->push<BlockType::IntegerShort>(static_cast<int8_t>(7));
  // addition2 is 1+2+3

  assert_trees_are_equal(addition1, addition2);

  // Sort
  Tree* addition3 = SharedEditionPool->clone(
      KAdd(KLn(5_e), KLn(1_e), KTrig(3_e, 0_e), 1_e, KTrig(2_e, 1_e), 0_e,
           KTrig(1_e, 0_e), 3_e, KMult(1_e, 0_e, KLn(2_e)), 1_e,
           KPow(KTrig(3_e, 0_e), -1_e)));
  NAry::Sort(addition3);
  assert_trees_are_equal(
      addition3, KAdd(0_e, 1_e, 1_e, 3_e, KLn(1_e), KMult(1_e, 0_e, KLn(2_e)),
                      KLn(5_e), KTrig(1_e, 0_e), KTrig(2_e, 1_e),
                      KPow(KTrig(3_e, 0_e), -1_e), KTrig(3_e, 0_e)));

  Tree* sorted = SharedEditionPool->clone(KAdd(1_e, 2_e, 3_e));
  NAry::SortedInsertChild(sorted, EditionReference(0_e));
  NAry::SortedInsertChild(sorted, EditionReference(2_e));
  NAry::SortedInsertChild(sorted, EditionReference(5_e));
  assert_trees_are_equal(sorted, KAdd(0_e, 1_e, 2_e, 2_e, 3_e, 5_e));
}

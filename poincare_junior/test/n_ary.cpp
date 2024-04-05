#include <poincare_junior/src/expression/k_tree.h>
#include <poincare_junior/src/layout/k_tree.h>
#include <poincare_junior/src/memory/tree_ref.h>
#include <poincare_junior/src/n_ary.h>
#include <quiz.h>

#include "helper.h"

using namespace PoincareJ;

QUIZ_CASE(pcj_n_ary_manipulation) {
  TreeRef rackLayout1 = SharedTreeStack->push<Type::RackLayout>(3);
  SharedTreeStack->push<Type::AsciiCodePointLayout, CodePoint>('x');
  SharedTreeStack->push<Type::VerticalOffsetLayout>(false, false);
  SharedTreeStack->push<Type::AsciiCodePointLayout, CodePoint>('2');
  SharedTreeStack->push<Type::AsciiCodePointLayout, CodePoint>('+');
  // rackLayout1 is x^2+

  TreeRef rackLayout2 = SharedTreeStack->push<Type::RackLayout>(3);
  SharedTreeStack->push<Type::AsciiCodePointLayout, CodePoint>('-');
  SharedTreeStack->push<Type::AsciiCodePointLayout, CodePoint>('4');
  SharedTreeStack->push<Type::AsciiCodePointLayout, CodePoint>('3');
  // rackLayout2 is -43

  TreeRef four = NAry::DetachChildAtIndex(rackLayout2, 1);
  // rackLayout2 is -3
  NAry::AddChildAtIndex(rackLayout1, four, 3);
  // rackLayout1 is x^2+4
  NAry::AddOrMergeChildAtIndex(rackLayout1, rackLayout2, 2);
  // rackLayout1 is x^2-3+4
  NAry::RemoveChildAtIndex(rackLayout1, 4);
  // rackLayout1 is x^2-34

  TreeRef rackLayout3 = SharedTreeStack->push<Type::RackLayout>(5);
  SharedTreeStack->push<Type::AsciiCodePointLayout, CodePoint>('x');
  SharedTreeStack->push<Type::VerticalOffsetLayout>(false, false);
  SharedTreeStack->push<Type::AsciiCodePointLayout, CodePoint>('2');
  SharedTreeStack->push<Type::AsciiCodePointLayout, CodePoint>('-');
  SharedTreeStack->push<Type::AsciiCodePointLayout, CodePoint>('3');
  SharedTreeStack->push<Type::AsciiCodePointLayout, CodePoint>('4');
  // rackLayout3 is x^2-34

  assert_trees_are_equal(rackLayout1, rackLayout3);

  TreeRef addition1 = SharedTreeStack->push<Type::Addition>(3);
  SharedTreeStack->push<Type::IntegerShort>(static_cast<int8_t>(1));
  SharedTreeStack->push<Type::Addition>(3);
  SharedTreeStack->push<Type::Multiplication>(2);
  SharedTreeStack->push<Type::IntegerShort>(static_cast<int8_t>(2));
  SharedTreeStack->push<Type::IntegerShort>(static_cast<int8_t>(3));
  SharedTreeStack->push<Type::IntegerShort>(static_cast<int8_t>(4));
  SharedTreeStack->push<Type::IntegerShort>(static_cast<int8_t>(5));
  SharedTreeStack->push<Type::Addition>(2);
  SharedTreeStack->push<Type::Addition>(1);
  SharedTreeStack->push<Type::IntegerShort>(static_cast<int8_t>(6));
  SharedTreeStack->push<Type::IntegerShort>(static_cast<int8_t>(7));
  // addition1 is 1+(2*3+4+5)+((+6)+7)
  NAry::Flatten(addition1);
  // addition1 is 1+2*3+4+5+6+7

  TreeRef addition2 = SharedTreeStack->push<Type::Addition>(6);
  SharedTreeStack->push<Type::IntegerShort>(static_cast<int8_t>(1));
  SharedTreeStack->push<Type::Multiplication>(2);
  SharedTreeStack->push<Type::IntegerShort>(static_cast<int8_t>(2));
  SharedTreeStack->push<Type::IntegerShort>(static_cast<int8_t>(3));
  SharedTreeStack->push<Type::IntegerShort>(static_cast<int8_t>(4));
  SharedTreeStack->push<Type::IntegerShort>(static_cast<int8_t>(5));
  SharedTreeStack->push<Type::IntegerShort>(static_cast<int8_t>(6));
  SharedTreeStack->push<Type::IntegerShort>(static_cast<int8_t>(7));
  // addition2 is 1+2+3

  assert_trees_are_equal(addition1, addition2);

  // Sort
  Tree* addition3 = SharedTreeStack->clone(
      KAdd(KLn(5_e), KLn(1_e), KTrig(3_e, 0_e), 1_e, KTrig(2_e, 1_e), 0_e,
           KTrig(1_e, 0_e), 3_e, KMult(1_e, 0_e, KLn(2_e)), 1_e,
           KPow(KTrig(3_e, 0_e), -1_e)));
  NAry::Sort(addition3);
  assert_trees_are_equal(
      addition3, KAdd(0_e, 1_e, 1_e, 3_e, KLn(1_e), KMult(1_e, 0_e, KLn(2_e)),
                      KLn(5_e), KTrig(1_e, 0_e), KTrig(2_e, 1_e),
                      KPow(KTrig(3_e, 0_e), -1_e), KTrig(3_e, 0_e)));

  Tree* sorted = SharedTreeStack->clone(KAdd(1_e, 2_e, 3_e));
  NAry::SortedInsertChild(sorted, TreeRef(0_e));
  NAry::SortedInsertChild(sorted, TreeRef(2_e));
  NAry::SortedInsertChild(sorted, TreeRef(5_e));
  assert_trees_are_equal(sorted, KAdd(0_e, 1_e, 2_e, 2_e, 3_e, 5_e));

  // CloneSubRange
  Tree* subRange = NAry::CloneSubRange("abcdef"_l, 1, 4);
  assert_trees_are_equal(subRange, "bcd"_l);
}

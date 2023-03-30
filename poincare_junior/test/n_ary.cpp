#include "helper.h"
#include <poincare_junior/src/n_ary.h>
#include <poincare_junior/src/memory/edition_reference.h>
#include <quiz.h>

using namespace PoincareJ;

QUIZ_CASE(pcj_n_ary_manipulation) {
  EditionReference rackLayout1 = EditionReference::Push<BlockType::RackLayout>(3);
  EditionReference::Push<BlockType::CodePointLayout, CodePoint>('x');
  EditionReference::Push<BlockType::VerticalOffsetLayout>();
  EditionReference::Push<BlockType::CodePointLayout, CodePoint>('2');
  EditionReference::Push<BlockType::CodePointLayout, CodePoint>('+');
  // rackLayout1 is x^2+

  EditionReference rackLayout2 = EditionReference::Push<BlockType::RackLayout>(3);
  EditionReference::Push<BlockType::CodePointLayout, CodePoint>('-');
  EditionReference::Push<BlockType::CodePointLayout, CodePoint>('4');
  EditionReference::Push<BlockType::CodePointLayout, CodePoint>('3');
  // rackLayout2 is -43

  EditionReference four = NAry::DetachChildAtIndex(rackLayout2, 1);
  // rackLayout2 is -3
  NAry::AddChildAtIndex(rackLayout1, four, 3);
  // rackLayout1 is x^2+4
  NAry::AddOrMergeChildAtIndex(rackLayout1, rackLayout2, 2);
  // rackLayout1 is x^2-3+4
  NAry::RemoveChildAtIndex(rackLayout1, 4);
  // rackLayout1 is x^2-34

  EditionReference rackLayout3 = EditionReference::Push<BlockType::RackLayout>(5);
  EditionReference::Push<BlockType::CodePointLayout, CodePoint>('x');
  EditionReference::Push<BlockType::VerticalOffsetLayout>();
  EditionReference::Push<BlockType::CodePointLayout, CodePoint>('2');
  EditionReference::Push<BlockType::CodePointLayout, CodePoint>('-');
  EditionReference::Push<BlockType::CodePointLayout, CodePoint>('3');
  EditionReference::Push<BlockType::CodePointLayout, CodePoint>('4');
  // rackLayout3 is x^2-34

  assert_trees_are_equal(rackLayout1, rackLayout3);

  EditionReference addition1 = EditionReference::Push<BlockType::Addition>(3);
  EditionReference::Push<BlockType::IntegerShort>(static_cast<int8_t>(1));
  EditionReference::Push<BlockType::Addition>(2);
  EditionReference::Push<BlockType::IntegerShort>(static_cast<int8_t>(2));
  EditionReference::Push<BlockType::IntegerShort>(static_cast<int8_t>(3));
  // addition1 is 1+(2+3)
  NAry::Flatten(addition1);
  // addition1 is 1+2+3

  EditionReference addition2 = EditionReference::Push<BlockType::Addition>(3);
  EditionReference::Push<BlockType::IntegerShort>(static_cast<int8_t>(1));
  EditionReference::Push<BlockType::IntegerShort>(static_cast<int8_t>(2));
  EditionReference::Push<BlockType::IntegerShort>(static_cast<int8_t>(3));
  // addition2 is 1+2+3

  assert_trees_are_equal(addition1, addition2);
}

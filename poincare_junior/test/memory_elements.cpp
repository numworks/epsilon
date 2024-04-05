#include <poincare_junior/src/expression/k_tree.h>
#include <poincare_junior/src/layout/k_tree.h>
#include <poincare_junior/src/memory/node_iterator.h>
#include <poincare_junior/src/memory/value_block.h>

#include "helper.h"

using namespace PoincareJ;

QUIZ_CASE(pcj_block) {
  SharedEditionPool->flush();

  // Create pool: [ "0" | "1" | "2" | 4 | -4 | "0" ]
  Block* firstBlock = SharedEditionPool->push(Type::Zero)->block();
  SharedEditionPool->push(Type::One);
  SharedEditionPool->push(Type::Two);
  SharedEditionPool->push(ValueBlock(4));
  SharedEditionPool->push(ValueBlock(-4));
  SharedEditionPool->push(Type::Zero);
  assert_pool_block_sizes_is(6);

  // Block navigation
  quiz_assert(*firstBlock->nextNth(5) == *firstBlock);
  quiz_assert(*firstBlock->next() != *firstBlock);
  quiz_assert(*firstBlock->next() == Type::One);
  quiz_assert(static_cast<uint8_t>(*firstBlock->nextNth(3)) == 4);
  quiz_assert(static_cast<int8_t>(*firstBlock->nextNth(4)) == -4);
}

QUIZ_CASE(pcj_type_block) {
  typedef union {
    uint8_t m_value;
    struct {
      bool nAry : 1;
      bool expression : 1;
      bool layout : 1;
      bool integer : 1;
      bool rational : 1;
      bool number : 1;
      bool userNamed : 1;
    };
  } TypeBlockProperties;
  static_assert(sizeof(TypeBlockProperties) == sizeof(uint8_t),
                "TypeBlockProperties  has too many entries for an uint8_t");

  std::pair<Type, TypeBlockProperties> blockTypeTests[] = {
      std::make_pair(Type::Zero, TypeBlockProperties{.nAry = false,
                                                     .expression = true,
                                                     .layout = false,
                                                     .integer = true,
                                                     .rational = true,
                                                     .number = true,
                                                     .userNamed = false}),
      std::make_pair(Type::One, TypeBlockProperties{.nAry = false,
                                                    .expression = true,
                                                    .layout = false,
                                                    .integer = true,
                                                    .rational = true,
                                                    .number = true,
                                                    .userNamed = false}),
      std::make_pair(Type::Two, TypeBlockProperties{.nAry = false,
                                                    .expression = true,
                                                    .layout = false,
                                                    .integer = true,
                                                    .rational = true,
                                                    .number = true,
                                                    .userNamed = false}),
      std::make_pair(Type::Half, TypeBlockProperties{.nAry = false,
                                                     .expression = true,
                                                     .layout = false,
                                                     .integer = false,
                                                     .rational = true,
                                                     .number = true,
                                                     .userNamed = false}),
      std::make_pair(Type::MinusOne, TypeBlockProperties{.nAry = false,
                                                         .expression = true,
                                                         .layout = false,
                                                         .integer = true,
                                                         .rational = true,
                                                         .number = true,
                                                         .userNamed = false}),
      std::make_pair(Type::IntegerShort,
                     TypeBlockProperties{.nAry = false,
                                         .expression = true,
                                         .layout = false,
                                         .integer = true,
                                         .rational = true,
                                         .number = true,
                                         .userNamed = false}),
      std::make_pair(Type::IntegerPosBig,
                     TypeBlockProperties{.nAry = false,
                                         .expression = true,
                                         .layout = false,
                                         .integer = true,
                                         .rational = true,
                                         .number = true,
                                         .userNamed = false}),
      std::make_pair(Type::IntegerNegBig,
                     TypeBlockProperties{.nAry = false,
                                         .expression = true,
                                         .layout = false,
                                         .integer = true,
                                         .rational = true,
                                         .number = true,
                                         .userNamed = false}),
      std::make_pair(Type::RationalShort,
                     TypeBlockProperties{.nAry = false,
                                         .expression = true,
                                         .layout = false,
                                         .integer = false,
                                         .rational = true,
                                         .number = true,
                                         .userNamed = false}),
      std::make_pair(Type::RationalPosBig,
                     TypeBlockProperties{.nAry = false,
                                         .expression = true,
                                         .layout = false,
                                         .integer = false,
                                         .rational = true,
                                         .number = true,
                                         .userNamed = false}),
      std::make_pair(Type::RationalNegBig,
                     TypeBlockProperties{.nAry = false,
                                         .expression = true,
                                         .layout = false,
                                         .integer = false,
                                         .rational = true,
                                         .number = true,
                                         .userNamed = false}),
      std::make_pair(Type::SingleFloat,
                     TypeBlockProperties{.nAry = false,
                                         .expression = true,
                                         .layout = false,
                                         .integer = false,
                                         .rational = false,
                                         .number = true,
                                         .userNamed = false}),
      std::make_pair(Type::DoubleFloat,
                     TypeBlockProperties{.nAry = false,
                                         .expression = true,
                                         .layout = false,
                                         .integer = false,
                                         .rational = false,
                                         .number = true,
                                         .userNamed = false}),
      std::make_pair(Type::Decimal, TypeBlockProperties{.nAry = false,
                                                        .expression = true,
                                                        .layout = false,
                                                        .integer = false,
                                                        .rational = false,
                                                        .number = false,
                                                        .userNamed = false}),
      std::make_pair(Type::UserSymbol, TypeBlockProperties{.nAry = false,
                                                           .expression = true,
                                                           .layout = false,
                                                           .integer = false,
                                                           .rational = false,
                                                           .number = false,
                                                           .userNamed = true}),
      std::make_pair(Type::UserFunction,
                     TypeBlockProperties{.nAry = false,
                                         .expression = true,
                                         .layout = false,
                                         .integer = false,
                                         .rational = false,
                                         .number = false,
                                         .userNamed = true}),
      std::make_pair(Type::UserSequence,
                     TypeBlockProperties{.nAry = false,
                                         .expression = true,
                                         .layout = false,
                                         .integer = false,
                                         .rational = false,
                                         .number = false,
                                         .userNamed = true}),
      std::make_pair(Type::Addition, TypeBlockProperties{.nAry = true,
                                                         .expression = true,
                                                         .layout = false,
                                                         .integer = false,
                                                         .rational = false,
                                                         .number = false,
                                                         .userNamed = false}),
      std::make_pair(Type::Multiplication,
                     TypeBlockProperties{.nAry = true,
                                         .expression = true,
                                         .layout = false,
                                         .integer = false,
                                         .rational = false,
                                         .number = false,
                                         .userNamed = false}),
      std::make_pair(Type::Set, TypeBlockProperties{.nAry = true,
                                                    .expression = true,
                                                    .layout = false,
                                                    .integer = false,
                                                    .rational = false,
                                                    .number = false,
                                                    .userNamed = false}),
      std::make_pair(Type::Matrix, TypeBlockProperties{.nAry = false,
                                                       .expression = true,
                                                       .layout = false,
                                                       .integer = false,
                                                       .rational = false,
                                                       .number = false,
                                                       .userNamed = false}),
      std::make_pair(Type::List, TypeBlockProperties{.nAry = true,
                                                     .expression = true,
                                                     .layout = false,
                                                     .integer = false,
                                                     .rational = false,
                                                     .number = false,
                                                     .userNamed = false}),
      // Deactivated because of isNumber()
      //   std::make_pair(Type::Constant,
      //                  TypeBlockProperties{.nAry = false,
      //                                      .expression = true,
      //                                      .layout = false,
      //                                      .integer = false,
      //                                      .rational = false,
      //                                      .number = true,
      //                                      .userNamed = false}),
      std::make_pair(Type::Power, TypeBlockProperties{.nAry = false,
                                                      .expression = true,
                                                      .layout = false,
                                                      .integer = false,
                                                      .rational = false,
                                                      .number = false,
                                                      .userNamed = false}),
      std::make_pair(Type::PowerReal, TypeBlockProperties{.nAry = false,
                                                          .expression = true,
                                                          .layout = false,
                                                          .integer = false,
                                                          .rational = false,
                                                          .number = false,
                                                          .userNamed = false}),
      std::make_pair(Type::Abs, TypeBlockProperties{.nAry = false,
                                                    .expression = true,
                                                    .layout = false,
                                                    .integer = false,
                                                    .rational = false,
                                                    .number = false,
                                                    .userNamed = false}),
      std::make_pair(Type::Cosine, TypeBlockProperties{.nAry = false,
                                                       .expression = true,
                                                       .layout = false,
                                                       .integer = false,
                                                       .rational = false,
                                                       .number = false,
                                                       .userNamed = false}),
      std::make_pair(Type::Sine, TypeBlockProperties{.nAry = false,
                                                     .expression = true,
                                                     .layout = false,
                                                     .integer = false,
                                                     .rational = false,
                                                     .number = false,
                                                     .userNamed = false}),
      std::make_pair(Type::Tangent, TypeBlockProperties{.nAry = false,
                                                        .expression = true,
                                                        .layout = false,
                                                        .integer = false,
                                                        .rational = false,
                                                        .number = false,
                                                        .userNamed = false}),
      std::make_pair(Type::ArcCosine, TypeBlockProperties{.nAry = false,
                                                          .expression = true,
                                                          .layout = false,
                                                          .integer = false,
                                                          .rational = false,
                                                          .number = false,
                                                          .userNamed = false}),
      std::make_pair(Type::ArcSine, TypeBlockProperties{.nAry = false,
                                                        .expression = true,
                                                        .layout = false,
                                                        .integer = false,
                                                        .rational = false,
                                                        .number = false,
                                                        .userNamed = false}),
      std::make_pair(Type::ArcTangent, TypeBlockProperties{.nAry = false,
                                                           .expression = true,
                                                           .layout = false,
                                                           .integer = false,
                                                           .rational = false,
                                                           .number = false,
                                                           .userNamed = false}),
      std::make_pair(Type::Logarithm, TypeBlockProperties{.nAry = false,
                                                          .expression = true,
                                                          .layout = false,
                                                          .integer = false,
                                                          .rational = false,
                                                          .number = false,
                                                          .userNamed = false}),
      std::make_pair(Type::Log, TypeBlockProperties{.nAry = false,
                                                    .expression = true,
                                                    .layout = false,
                                                    .integer = false,
                                                    .rational = false,
                                                    .number = false,
                                                    .userNamed = false}),
      std::make_pair(Type::Ln, TypeBlockProperties{.nAry = false,
                                                   .expression = true,
                                                   .layout = false,
                                                   .integer = false,
                                                   .rational = false,
                                                   .number = false,
                                                   .userNamed = false}),
      std::make_pair(Type::Derivative, TypeBlockProperties{.nAry = false,
                                                           .expression = true,
                                                           .layout = false,
                                                           .integer = false,
                                                           .rational = false,
                                                           .number = false,
                                                           .userNamed = false}),
      std::make_pair(Type::Exponential,
                     TypeBlockProperties{.nAry = false,
                                         .expression = true,
                                         .layout = false,
                                         .integer = false,
                                         .rational = false,
                                         .number = false,
                                         .userNamed = false}),
      std::make_pair(Type::Trig, TypeBlockProperties{.nAry = false,
                                                     .expression = true,
                                                     .layout = false,
                                                     .integer = false,
                                                     .rational = false,
                                                     .number = false,
                                                     .userNamed = false}),
      std::make_pair(Type::TrigDiff, TypeBlockProperties{.nAry = false,
                                                         .expression = true,
                                                         .layout = false,
                                                         .integer = false,
                                                         .rational = false,
                                                         .number = false,
                                                         .userNamed = false}),
      std::make_pair(Type::Factorial, TypeBlockProperties{.nAry = false,
                                                          .expression = true,
                                                          .layout = false,
                                                          .integer = false,
                                                          .rational = false,
                                                          .number = false,
                                                          .userNamed = false}),
      std::make_pair(Type::SquareRoot, TypeBlockProperties{.nAry = false,
                                                           .expression = true,
                                                           .layout = false,
                                                           .integer = false,
                                                           .rational = false,
                                                           .number = false,
                                                           .userNamed = false}),
      std::make_pair(Type::Subtraction,
                     TypeBlockProperties{.nAry = false,
                                         .expression = true,
                                         .layout = false,
                                         .integer = false,
                                         .rational = false,
                                         .number = false,
                                         .userNamed = false}),
      std::make_pair(Type::Division, TypeBlockProperties{.nAry = false,
                                                         .expression = true,
                                                         .layout = false,
                                                         .integer = false,
                                                         .rational = false,
                                                         .number = false,
                                                         .userNamed = false}),
      std::make_pair(Type::RackLayout, TypeBlockProperties{.nAry = true,
                                                           .expression = false,
                                                           .layout = true,
                                                           .integer = false,
                                                           .rational = false,
                                                           .number = false,
                                                           .userNamed = false}),
      std::make_pair(Type::FractionLayout,
                     TypeBlockProperties{.nAry = false,
                                         .expression = false,
                                         .layout = true,
                                         .integer = false,
                                         .rational = false,
                                         .number = false,
                                         .userNamed = false}),
      std::make_pair(Type::ParenthesisLayout,
                     TypeBlockProperties{.nAry = false,
                                         .expression = false,
                                         .layout = true,
                                         .integer = false,
                                         .rational = false,
                                         .number = false,
                                         .userNamed = false}),
      std::make_pair(Type::VerticalOffsetLayout,
                     TypeBlockProperties{.nAry = false,
                                         .expression = false,
                                         .layout = true,
                                         .integer = false,
                                         .rational = false,
                                         .number = false,
                                         .userNamed = false}),
      std::make_pair(Type::AsciiCodePointLayout,
                     TypeBlockProperties{.nAry = false,
                                         .expression = false,
                                         .layout = true,
                                         .integer = false,
                                         .rational = false,
                                         .number = false,
                                         .userNamed = false})};

  for (std::pair<Type, TypeBlockProperties> test : blockTypeTests) {
    TypeBlock block = TypeBlock(std::get<Type>(test));
    TypeBlockProperties properties = std::get<TypeBlockProperties>(test);
    quiz_assert(block.isNAry() == properties.nAry);
    quiz_assert(block.isExpression() == properties.expression);
    quiz_assert(block.isLayout() == properties.layout);
    quiz_assert(block.isInteger() == properties.integer);
    quiz_assert(block.isRational() == properties.rational);
    quiz_assert(block.isNumber() == properties.number);
    quiz_assert(block.isUserNamed() == properties.userNamed);
  }
}

void assert_tree_equals_blocks(const Tree* node,
                               std::initializer_list<Block> blocks) {
  assert_node_equals_blocks(node, blocks);
  quiz_assert(blocks.size() == node->treeSize());
}

QUIZ_CASE(pcj_constexpr_tree_constructor) {
  assert_tree_equals_blocks(0_e, {TypeBlock(Type::Zero)});
  assert_tree_equals_blocks(1_e, {TypeBlock(Type::One)});
  assert_tree_equals_blocks(2_e, {TypeBlock(Type::Two)});
  assert_tree_equals_blocks(12_e,
                            {TypeBlock(Type::IntegerShort), ValueBlock(12)});
  assert_tree_equals_blocks(127_e,
                            {TypeBlock(Type::IntegerShort), ValueBlock(127)});
  assert_tree_equals_blocks(
      128_e, {TypeBlock(Type::IntegerPosBig), ValueBlock(1), ValueBlock(128)});
  assert_tree_equals_blocks(
      256_e, {TypeBlock(Type::IntegerPosBig), ValueBlock(2), ValueBlock(0),
              ValueBlock(1)});
  assert_tree_equals_blocks(-1_e, {TypeBlock(Type::MinusOne)});
  assert_tree_equals_blocks(-12_e,
                            {TypeBlock(Type::IntegerShort), ValueBlock(-12)});
  assert_tree_equals_blocks(-128_e,
                            {TypeBlock(Type::IntegerShort), ValueBlock(-128)});
  assert_tree_equals_blocks(
      -129_e, {TypeBlock(Type::IntegerNegBig), ValueBlock(1), ValueBlock(129)});

  assert_tree_equals_blocks(π_e, {TypeBlock(Type::Pi)});
  assert_tree_equals_blocks(0.045_e,
                            {TypeBlock(Type::Decimal), ValueBlock(3),
                             TypeBlock(Type::IntegerShort), ValueBlock(45)});
  assert_tree_equals_blocks(1.23_e,
                            {TypeBlock(Type::Decimal), ValueBlock(2),
                             TypeBlock(Type::IntegerShort), ValueBlock(123)});
  assert_tree_equals_blocks(
      2.0_fe, {TypeBlock(Type::SingleFloat), ValueBlock(0), ValueBlock(0),
               ValueBlock(0), ValueBlock(64)});
  assert_tree_equals_blocks(
      2.0_de, {TypeBlock(Type::DoubleFloat), ValueBlock(0), ValueBlock(0),
               ValueBlock(0), ValueBlock(0), ValueBlock(0), ValueBlock(0),
               ValueBlock(0), ValueBlock(64)});
  assert_tree_equals_blocks(-1_e, {Type::MinusOne});
  assert_tree_equals_blocks(1_e, {Type::One});
  assert_tree_equals_blocks(
      KAdd(1_e, 2_e),
      {TypeBlock(Type::Addition), ValueBlock(2), Type::One, Type::Two});
  assert_tree_equals_blocks(KMult(1_e, 2_e, -1_e),
                            {TypeBlock(Type::Multiplication), ValueBlock(3),
                             Type::One, Type::Two, Type::MinusOne});
  assert_tree_equals_blocks(KSet(1_e),
                            {TypeBlock(Type::Set), ValueBlock(1), Type::One});
  assert_tree_equals_blocks(KPow(1_e, 2_e),
                            {TypeBlock(Type::Power), Type::One, Type::Two});
  assert_tree_equals_blocks(KSqrt(2_e),
                            {TypeBlock(Type::SquareRoot), Type::Two});
  assert_tree_equals_blocks(
      KSub(1_e, 2_e), {TypeBlock(Type::Subtraction), Type::One, Type::Two});
  assert_tree_equals_blocks(
      KSub(1_e, 2_e), {TypeBlock(Type::Subtraction), Type::One, Type::Two});
  assert_tree_equals_blocks(
      "var"_e, {TypeBlock(Type::UserSymbol), ValueBlock(4), ValueBlock('v'),
                ValueBlock('a'), ValueBlock('r'), ValueBlock(0)});
}

QUIZ_CASE(pcj_edition_node_constructor) {
  assert_node_equals_blocks(
      SharedEditionPool->push<Type::IntegerPosBig>(
          static_cast<uint64_t>(1232424242)),
      {TypeBlock(Type::IntegerPosBig), ValueBlock(4), ValueBlock(0x32),
       ValueBlock(0x4d), ValueBlock(0x75), ValueBlock(0x49)});
  assert_node_equals_blocks(
      SharedEditionPool->push<Type::IntegerNegBig>(
          static_cast<uint64_t>(1232424242)),
      {TypeBlock(Type::IntegerNegBig), ValueBlock(4), ValueBlock(0x32),
       ValueBlock(0x4d), ValueBlock(0x75), ValueBlock(0x49)});
  assert_node_equals_blocks(
      SharedEditionPool->push<Type::IntegerNegBig>(
          static_cast<uint64_t>(1232424242)),
      {TypeBlock(Type::IntegerNegBig), ValueBlock(4), ValueBlock(0x32),
       ValueBlock(0x4d), ValueBlock(0x75), ValueBlock(0x49)});
}

QUIZ_CASE(pcj_node_iterator) {
  constexpr KTree k_simpleExpression = KMult(KAdd(1_e, 2_e), 3_e, 4_e);
  EditionReference mult(k_simpleExpression);
  KTree a = KAdd(1_e, 2_e);
  KTree b = 3_e;
  KTree c = 4_e;
  const Tree* children[] = {a, b, c};

  // Scan children forward
  for (const std::pair<const Tree*, int> indexedNode :
       NodeIterator::Children<NoEditable>(mult)) {
    assert_trees_are_equal(std::get<const Tree*>(indexedNode),
                           children[std::get<int>(indexedNode)]);
  }

  // Edit children forward
  KTree e = 6_e;
  KTree f = 7_e;
  KTree g = 8_e;
  const Tree* newChildren[] = {e, f, g};
  for (std::pair<EditionReference, int> indexedRef :
       NodeIterator::Children<Editable>(mult)) {
    std::get<EditionReference>(indexedRef)
        ->cloneTreeOverTree(newChildren[std::get<int>(indexedRef)]);
  }
  // Check edition
  for (const std::pair<const Tree*, int> indexedNode :
       NodeIterator::Children<NoEditable>(mult)) {
    assert_trees_are_equal(std::get<const Tree*>(indexedNode),
                           newChildren[std::get<int>(indexedNode)]);
  }

  constexpr KTree k_secondSimpleExpression = KMult(KAdd(1_e, 2_e), 3_e);
  EditionReference mult2(k_secondSimpleExpression);
  const Tree* children2[] = {a, b};
  // Scan two nodes children forward
  for (std::pair<std::array<const Tree*, 2>, int> indexedArray :
       MultipleNodesIterator::Children<NoEditable, 2>(
           std::array<const Tree*, 2>({mult, mult2}))) {
    std::array<const Tree*, 2> childrenPair =
        std::get<std::array<const Tree*, 2>>(indexedArray);
    int pairIndex = std::get<int>(indexedArray);

    assert_trees_are_equal(childrenPair[0], newChildren[pairIndex]);
    assert_trees_are_equal(childrenPair[1], children2[pairIndex]);
  }
  KTree n6 = 8_e;
  KTree n10 = 10_e;
  KTree n11 = 11_e;
  KTree n13 = 13_e;
  KTree n14 = 14_e;
  const Tree* newChildren1[] = {n10, n11};
  const Tree* newChildren2[] = {n13, n14};
  // Edit two nodes children forward
  for (std::pair<std::array<EditionReference, 2>, int> indexedRefs :
       MultipleNodesIterator::Children<Editable, 2>(
           std::array<EditionReference, 2>({mult, mult2}))) {
    std::array<EditionReference, 2> childrenPair =
        std::get<std::array<EditionReference, 2>>(indexedRefs);
    int pairIndex = std::get<int>(indexedRefs);
    childrenPair[0]->cloneTreeOverTree(newChildren1[pairIndex]);
    childrenPair[1]->cloneTreeOverTree(newChildren2[pairIndex]);
  }
  // Check edition
  const Tree* children1[] = {n10, n11, n6};
  for (const std::pair<const Tree*, int> indexedNode :
       NodeIterator::Children<NoEditable>(mult)) {
    assert_trees_are_equal(std::get<const Tree*>(indexedNode),
                           children1[std::get<int>(indexedNode)]);
  }
  for (const std::pair<const Tree*, int> indexedNode :
       NodeIterator::Children<NoEditable>(mult2)) {
    assert_trees_are_equal(std::get<const Tree*>(indexedNode),
                           newChildren2[std::get<int>(indexedNode)]);
  }
}

QUIZ_CASE(pcj_node) {
  SharedEditionPool->flush();

  // operator==
  const Tree* node0 = 42_e;
  Tree* node1 =
      SharedEditionPool->push<Type::IntegerShort>(static_cast<int8_t>(42));
  quiz_assert(node0 != node1 && *node0->block() == *node1->block());
  Tree* node2 = Tree::FromBlocks(SharedEditionPool->firstBlock());
  quiz_assert(node2 == node1);

  // Tree navigation
  constexpr KTree e1 = KMult(KAdd(1_e, 2_e), 3_e, 4_e, KMult(5_e, 6_e));
  constexpr KTree e2 = KPow(5_e, 6_e);
  Tree* n1 = EditionReference(e1);
  Tree* n2 = EditionReference(e2);
  quiz_assert(n1->treeSize() == 16);  // TODO: Magic Number
  assert_trees_are_equal(n1->nextNode(), KAdd(1_e, 2_e));
  assert_trees_are_equal(n1->nextTree(), e2);
  quiz_assert(n1->numberOfDescendants(false) == 8);
  quiz_assert(n1->numberOfDescendants(true) == 9);
  assert_trees_are_equal(n1->child(0), n1->nextNode());
  assert_trees_are_equal(n1->child(1),
                         n1->nextNode()->nextNode()->nextNode()->nextNode());
  quiz_assert(n1->indexOfChild(n1->child(1)) == 1);
  quiz_assert(!n1->hasChild(e2));
  quiz_assert(n1->hasChild(n1->child(2)));

  quiz_assert(n1->child(2)->hasAncestor(n1, true));
  quiz_assert(n1->child(2)->hasAncestor(n1, false));
  quiz_assert(n1->hasAncestor(n1, true));
  quiz_assert(!n1->hasAncestor(n1, false));
  quiz_assert(!n1->hasAncestor(n1->child(2), true));
  quiz_assert(!n1->hasAncestor(n1->child(2), false));
  quiz_assert(!n2->hasAncestor(n1, true));
  quiz_assert(!n2->hasAncestor(n1, false));
  quiz_assert(!n1->hasAncestor(n2, true));
  quiz_assert(!n1->hasAncestor(n2, false));

  quiz_assert(n1->commonAncestor(n1, n1) == n1);
  quiz_assert(n1->commonAncestor(n1, n1->child(0)->child(1)) == n1);
  quiz_assert(n1->commonAncestor(n1->child(0)->child(1), n1) == n1);
  quiz_assert(n1->commonAncestor(n1->child(0)->child(1), n1->child(2)) == n1);
  quiz_assert(n1->commonAncestor(n1->child(0)->child(0),
                                 n1->child(0)->child(1)) == n1->child(0));
  quiz_assert(n1->commonAncestor(n1->child(3)->child(0),
                                 n1->child(3)->child(1)) == n1->child(3));
  quiz_assert(!n1->commonAncestor(n1->child(0)->child(0), n2));
  quiz_assert(!n1->commonAncestor(n2, n2->child(0)));
  quiz_assert(!n2->commonAncestor(n1->child(0)->child(0), n2->child(0)));

  int position;
  quiz_assert(!n1->parentOfDescendant(n1, &position));
  quiz_assert(!n1->parentOfDescendant(n2, &position));
  quiz_assert(!n2->parentOfDescendant(n1, &position));
  quiz_assert(n1->parentOfDescendant(n1->child(0), &position) == n1);
  quiz_assert(position == 0);
  quiz_assert(n1->parentOfDescendant(n1->child(1), &position) == n1);
  quiz_assert(position == 1);
  quiz_assert(n1->parentOfDescendant(n1->child(3)->child(0), &position) ==
              n1->child(3));
  quiz_assert(position == 0);
  quiz_assert(n1->parentOfDescendant(n1->child(0)->child(1), &position) ==
              n1->child(0));
  quiz_assert(position == 1);
}

QUIZ_CASE(pcj_node_size) {
  Tree* node = SharedEditionPool->push<Type::IntegerPosBig>(
      static_cast<uint64_t>(0x00FF0000));
  quiz_assert(node->nodeSize() == 5);
  node = SharedEditionPool->push<Type::IntegerNegBig>(
      static_cast<uint64_t>(0x0000FF00));
  quiz_assert(node->nodeSize() == 4);
}

QUIZ_CASE(pcj_tree_ancestors) {
  const Tree* root = KMult(1_e, KAdd(2_e, KCos(KAdd(3_e, 4_e))));
  const Tree* add1 = root->child(1);
  const Tree* cos = add1->child(1);
  const Tree* add2 = cos->child(0);
  const Tree* tree = add2->child(0);
  quiz_assert(tree->treeIsIdenticalTo(3_e));
  for (int i = 0; const Tree* ancestor : tree->ancestors(root)) {
    quiz_assert(i != 0 || ancestor == root);
    quiz_assert(i != 1 || ancestor == add1);
    quiz_assert(i != 2 || ancestor == cos);
    quiz_assert(i != 3 || ancestor == add2);
    quiz_assert(i != 4);
    i++;
  }
}

QUIZ_CASE(pcj_constructor) {
  assert_tree_equals_blocks(
      KRackL("1+"_l,
             KParenthesisL(KRackL(
                 "2*"_l, KParenthesisL(KRackL("1+"_l, KFracL("1"_l, "2"_l))))),
             KSuperscriptL("2"_l), "-2"_l),
      {
          TypeBlock(Type::RackLayout),
          ValueBlock(4),
          ValueBlock(0),
          TypeBlock(Type::RackLayout),
          ValueBlock(2),
          ValueBlock(0),
          TypeBlock(Type::AsciiCodePointLayout),
          ValueBlock('1'),
          TypeBlock(Type::AsciiCodePointLayout),
          ValueBlock('+'),
          TypeBlock(Type::ParenthesisLayout),
          ValueBlock(0),
          TypeBlock(Type::RackLayout),
          ValueBlock(2),
          ValueBlock(0),
          TypeBlock(Type::RackLayout),
          ValueBlock(2),
          ValueBlock(0),
          TypeBlock(Type::AsciiCodePointLayout),
          ValueBlock('2'),
          TypeBlock(Type::AsciiCodePointLayout),
          ValueBlock('*'),
          TypeBlock(Type::ParenthesisLayout),
          ValueBlock(0),
          TypeBlock(Type::RackLayout),
          ValueBlock(2),
          ValueBlock(0),
          TypeBlock(Type::RackLayout),
          ValueBlock(2),
          ValueBlock(0),
          TypeBlock(Type::AsciiCodePointLayout),
          ValueBlock('1'),
          TypeBlock(Type::AsciiCodePointLayout),
          ValueBlock('+'),
          TypeBlock(Type::FractionLayout),
          TypeBlock(Type::RackLayout),
          ValueBlock(1),
          ValueBlock(0),
          TypeBlock(Type::AsciiCodePointLayout),
          ValueBlock('1'),
          TypeBlock(Type::RackLayout),
          ValueBlock(1),
          ValueBlock(0),
          TypeBlock(Type::AsciiCodePointLayout),
          ValueBlock('2'),
          TypeBlock(Type::VerticalOffsetLayout),
          ValueBlock(0),
          TypeBlock(Type::RackLayout),
          ValueBlock(1),
          ValueBlock(0),
          TypeBlock(Type::AsciiCodePointLayout),
          ValueBlock('2'),
          TypeBlock(Type::RackLayout),
          ValueBlock(2),
          ValueBlock(0),
          TypeBlock(Type::AsciiCodePointLayout),
          ValueBlock('-'),
          TypeBlock(Type::AsciiCodePointLayout),
          ValueBlock('2'),
      });
}

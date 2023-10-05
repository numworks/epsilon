#include <poincare_junior/src/expression/k_tree.h>
#include <poincare_junior/src/layout/k_tree.h>
#include <poincare_junior/src/memory/node_iterator.h>
#include <poincare_junior/src/memory/value_block.h>

#include "helper.h"

using namespace PoincareJ;

QUIZ_CASE(pcj_block) {
  CachePool::SharedCachePool->reset();

  // Create pool: [ "0" | "1" | "2" | 4 | -4 | "0" ]
  Block* firstBlock = SharedEditionPool->pushBlock(ZeroBlock);
  SharedEditionPool->pushBlock(OneBlock);
  SharedEditionPool->pushBlock(TwoBlock);
  SharedEditionPool->pushBlock(ValueBlock(4));
  SharedEditionPool->pushBlock(ValueBlock(-4));
  SharedEditionPool->pushBlock(ZeroBlock);
  assert_pools_block_sizes_are(0, 6);

  // Block navigation
  quiz_assert(*firstBlock->nextNth(5) == *firstBlock);
  quiz_assert(*firstBlock->next() != *firstBlock);
  quiz_assert(*firstBlock->next() == OneBlock);
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

  std::pair<BlockType, TypeBlockProperties> blockTypeTests[] = {
      std::make_pair(BlockType::Zero, TypeBlockProperties{.nAry = false,
                                                          .expression = true,
                                                          .layout = false,
                                                          .integer = true,
                                                          .rational = true,
                                                          .number = true,
                                                          .userNamed = false}),
      std::make_pair(BlockType::One, TypeBlockProperties{.nAry = false,
                                                         .expression = true,
                                                         .layout = false,
                                                         .integer = true,
                                                         .rational = true,
                                                         .number = true,
                                                         .userNamed = false}),
      std::make_pair(BlockType::Two, TypeBlockProperties{.nAry = false,
                                                         .expression = true,
                                                         .layout = false,
                                                         .integer = true,
                                                         .rational = true,
                                                         .number = true,
                                                         .userNamed = false}),
      std::make_pair(BlockType::Half, TypeBlockProperties{.nAry = false,
                                                          .expression = true,
                                                          .layout = false,
                                                          .integer = false,
                                                          .rational = true,
                                                          .number = true,
                                                          .userNamed = false}),
      std::make_pair(BlockType::MinusOne,
                     TypeBlockProperties{.nAry = false,
                                         .expression = true,
                                         .layout = false,
                                         .integer = true,
                                         .rational = true,
                                         .number = true,
                                         .userNamed = false}),
      std::make_pair(BlockType::IntegerShort,
                     TypeBlockProperties{.nAry = false,
                                         .expression = true,
                                         .layout = false,
                                         .integer = true,
                                         .rational = true,
                                         .number = true,
                                         .userNamed = false}),
      std::make_pair(BlockType::IntegerPosBig,
                     TypeBlockProperties{.nAry = false,
                                         .expression = true,
                                         .layout = false,
                                         .integer = true,
                                         .rational = true,
                                         .number = true,
                                         .userNamed = false}),
      std::make_pair(BlockType::IntegerNegBig,
                     TypeBlockProperties{.nAry = false,
                                         .expression = true,
                                         .layout = false,
                                         .integer = true,
                                         .rational = true,
                                         .number = true,
                                         .userNamed = false}),
      std::make_pair(BlockType::RationalShort,
                     TypeBlockProperties{.nAry = false,
                                         .expression = true,
                                         .layout = false,
                                         .integer = false,
                                         .rational = true,
                                         .number = true,
                                         .userNamed = false}),
      std::make_pair(BlockType::RationalPosBig,
                     TypeBlockProperties{.nAry = false,
                                         .expression = true,
                                         .layout = false,
                                         .integer = false,
                                         .rational = true,
                                         .number = true,
                                         .userNamed = false}),
      std::make_pair(BlockType::RationalNegBig,
                     TypeBlockProperties{.nAry = false,
                                         .expression = true,
                                         .layout = false,
                                         .integer = false,
                                         .rational = true,
                                         .number = true,
                                         .userNamed = false}),
      std::make_pair(BlockType::Float, TypeBlockProperties{.nAry = false,
                                                           .expression = true,
                                                           .layout = false,
                                                           .integer = false,
                                                           .rational = false,
                                                           .number = true,
                                                           .userNamed = false}),
      std::make_pair(BlockType::Decimal,
                     TypeBlockProperties{.nAry = false,
                                         .expression = true,
                                         .layout = false,
                                         .integer = false,
                                         .rational = false,
                                         .number = false,
                                         .userNamed = false}),
      std::make_pair(BlockType::UserSymbol,
                     TypeBlockProperties{.nAry = false,
                                         .expression = true,
                                         .layout = false,
                                         .integer = false,
                                         .rational = false,
                                         .number = false,
                                         .userNamed = true}),
      std::make_pair(BlockType::UserFunction,
                     TypeBlockProperties{.nAry = false,
                                         .expression = true,
                                         .layout = false,
                                         .integer = false,
                                         .rational = false,
                                         .number = false,
                                         .userNamed = true}),
      std::make_pair(BlockType::UserSequence,
                     TypeBlockProperties{.nAry = false,
                                         .expression = true,
                                         .layout = false,
                                         .integer = false,
                                         .rational = false,
                                         .number = false,
                                         .userNamed = true}),
      std::make_pair(BlockType::Addition,
                     TypeBlockProperties{.nAry = true,
                                         .expression = true,
                                         .layout = false,
                                         .integer = false,
                                         .rational = false,
                                         .number = false,
                                         .userNamed = false}),
      std::make_pair(BlockType::Multiplication,
                     TypeBlockProperties{.nAry = true,
                                         .expression = true,
                                         .layout = false,
                                         .integer = false,
                                         .rational = false,
                                         .number = false,
                                         .userNamed = false}),
      std::make_pair(BlockType::Set, TypeBlockProperties{.nAry = true,
                                                         .expression = true,
                                                         .layout = false,
                                                         .integer = false,
                                                         .rational = false,
                                                         .number = false,
                                                         .userNamed = false}),
      std::make_pair(BlockType::Matrix,
                     TypeBlockProperties{.nAry = false,
                                         .expression = true,
                                         .layout = false,
                                         .integer = false,
                                         .rational = false,
                                         .number = false,
                                         .userNamed = false}),
      std::make_pair(BlockType::List, TypeBlockProperties{.nAry = true,
                                                          .expression = true,
                                                          .layout = false,
                                                          .integer = false,
                                                          .rational = false,
                                                          .number = false,
                                                          .userNamed = false}),
      // Deactivated because of isNumber()
      //   std::make_pair(BlockType::Constant,
      //                  TypeBlockProperties{.nAry = false,
      //                                      .expression = true,
      //                                      .layout = false,
      //                                      .integer = false,
      //                                      .rational = false,
      //                                      .number = true,
      //                                      .userNamed = false}),
      std::make_pair(BlockType::Power, TypeBlockProperties{.nAry = false,
                                                           .expression = true,
                                                           .layout = false,
                                                           .integer = false,
                                                           .rational = false,
                                                           .number = false,
                                                           .userNamed = false}),
      std::make_pair(BlockType::PowerReal,
                     TypeBlockProperties{.nAry = false,
                                         .expression = true,
                                         .layout = false,
                                         .integer = false,
                                         .rational = false,
                                         .number = false,
                                         .userNamed = false}),
      std::make_pair(BlockType::Abs, TypeBlockProperties{.nAry = false,
                                                         .expression = true,
                                                         .layout = false,
                                                         .integer = false,
                                                         .rational = false,
                                                         .number = false,
                                                         .userNamed = false}),
      std::make_pair(BlockType::Cosine,
                     TypeBlockProperties{.nAry = false,
                                         .expression = true,
                                         .layout = false,
                                         .integer = false,
                                         .rational = false,
                                         .number = false,
                                         .userNamed = false}),
      std::make_pair(BlockType::Sine, TypeBlockProperties{.nAry = false,
                                                          .expression = true,
                                                          .layout = false,
                                                          .integer = false,
                                                          .rational = false,
                                                          .number = false,
                                                          .userNamed = false}),
      std::make_pair(BlockType::Tangent,
                     TypeBlockProperties{.nAry = false,
                                         .expression = true,
                                         .layout = false,
                                         .integer = false,
                                         .rational = false,
                                         .number = false,
                                         .userNamed = false}),
      std::make_pair(BlockType::ArcCosine,
                     TypeBlockProperties{.nAry = false,
                                         .expression = true,
                                         .layout = false,
                                         .integer = false,
                                         .rational = false,
                                         .number = false,
                                         .userNamed = false}),
      std::make_pair(BlockType::ArcSine,
                     TypeBlockProperties{.nAry = false,
                                         .expression = true,
                                         .layout = false,
                                         .integer = false,
                                         .rational = false,
                                         .number = false,
                                         .userNamed = false}),
      std::make_pair(BlockType::ArcTangent,
                     TypeBlockProperties{.nAry = false,
                                         .expression = true,
                                         .layout = false,
                                         .integer = false,
                                         .rational = false,
                                         .number = false,
                                         .userNamed = false}),
      std::make_pair(BlockType::Complex,
                     TypeBlockProperties{.nAry = false,
                                         .expression = true,
                                         .layout = false,
                                         .integer = false,
                                         .rational = false,
                                         .number = false,
                                         .userNamed = false}),
      std::make_pair(BlockType::Logarithm,
                     TypeBlockProperties{.nAry = false,
                                         .expression = true,
                                         .layout = false,
                                         .integer = false,
                                         .rational = false,
                                         .number = false,
                                         .userNamed = false}),
      std::make_pair(BlockType::Log, TypeBlockProperties{.nAry = false,
                                                         .expression = true,
                                                         .layout = false,
                                                         .integer = false,
                                                         .rational = false,
                                                         .number = false,
                                                         .userNamed = false}),
      std::make_pair(BlockType::Ln, TypeBlockProperties{.nAry = false,
                                                        .expression = true,
                                                        .layout = false,
                                                        .integer = false,
                                                        .rational = false,
                                                        .number = false,
                                                        .userNamed = false}),
      std::make_pair(BlockType::Derivative,
                     TypeBlockProperties{.nAry = false,
                                         .expression = true,
                                         .layout = false,
                                         .integer = false,
                                         .rational = false,
                                         .number = false,
                                         .userNamed = false}),
      std::make_pair(BlockType::Exponential,
                     TypeBlockProperties{.nAry = false,
                                         .expression = true,
                                         .layout = false,
                                         .integer = false,
                                         .rational = false,
                                         .number = false,
                                         .userNamed = false}),
      std::make_pair(BlockType::Trig, TypeBlockProperties{.nAry = false,
                                                          .expression = true,
                                                          .layout = false,
                                                          .integer = false,
                                                          .rational = false,
                                                          .number = false,
                                                          .userNamed = false}),
      std::make_pair(BlockType::TrigDiff,
                     TypeBlockProperties{.nAry = false,
                                         .expression = true,
                                         .layout = false,
                                         .integer = false,
                                         .rational = false,
                                         .number = false,
                                         .userNamed = false}),
      std::make_pair(BlockType::Factorial,
                     TypeBlockProperties{.nAry = false,
                                         .expression = true,
                                         .layout = false,
                                         .integer = false,
                                         .rational = false,
                                         .number = false,
                                         .userNamed = false}),
      std::make_pair(BlockType::SquareRoot,
                     TypeBlockProperties{.nAry = false,
                                         .expression = true,
                                         .layout = false,
                                         .integer = false,
                                         .rational = false,
                                         .number = false,
                                         .userNamed = false}),
      std::make_pair(BlockType::Subtraction,
                     TypeBlockProperties{.nAry = false,
                                         .expression = true,
                                         .layout = false,
                                         .integer = false,
                                         .rational = false,
                                         .number = false,
                                         .userNamed = false}),
      std::make_pair(BlockType::Division,
                     TypeBlockProperties{.nAry = false,
                                         .expression = true,
                                         .layout = false,
                                         .integer = false,
                                         .rational = false,
                                         .number = false,
                                         .userNamed = false}),
      std::make_pair(BlockType::RackLayout,
                     TypeBlockProperties{.nAry = true,
                                         .expression = false,
                                         .layout = true,
                                         .integer = false,
                                         .rational = false,
                                         .number = false,
                                         .userNamed = false}),
      std::make_pair(BlockType::FractionLayout,
                     TypeBlockProperties{.nAry = false,
                                         .expression = false,
                                         .layout = true,
                                         .integer = false,
                                         .rational = false,
                                         .number = false,
                                         .userNamed = false}),
      std::make_pair(BlockType::ParenthesisLayout,
                     TypeBlockProperties{.nAry = false,
                                         .expression = false,
                                         .layout = true,
                                         .integer = false,
                                         .rational = false,
                                         .number = false,
                                         .userNamed = false}),
      std::make_pair(BlockType::VerticalOffsetLayout,
                     TypeBlockProperties{.nAry = false,
                                         .expression = false,
                                         .layout = true,
                                         .integer = false,
                                         .rational = false,
                                         .number = false,
                                         .userNamed = false}),
      std::make_pair(BlockType::CodePointLayout,
                     TypeBlockProperties{.nAry = false,
                                         .expression = false,
                                         .layout = true,
                                         .integer = false,
                                         .rational = false,
                                         .number = false,
                                         .userNamed = false})};

  for (std::pair<BlockType, TypeBlockProperties> test : blockTypeTests) {
    TypeBlock block = TypeBlock(std::get<BlockType>(test));
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
  assert_tree_equals_blocks(0_e, {TypeBlock(BlockType::Zero)});
  assert_tree_equals_blocks(1_e, {TypeBlock(BlockType::One)});
  assert_tree_equals_blocks(2_e, {TypeBlock(BlockType::Two)});
  assert_tree_equals_blocks(
      12_e, {TypeBlock(BlockType::IntegerShort), ValueBlock(12)});
  assert_tree_equals_blocks(
      127_e, {TypeBlock(BlockType::IntegerShort), ValueBlock(127)});
  assert_tree_equals_blocks(128_e, {TypeBlock(BlockType::IntegerPosBig),
                                    ValueBlock(1), ValueBlock(128)});
  assert_tree_equals_blocks(
      256_e, {TypeBlock(BlockType::IntegerPosBig), ValueBlock(2), ValueBlock(0),
              ValueBlock(1)});
  assert_tree_equals_blocks(-1_e, {TypeBlock(BlockType::MinusOne)});
  assert_tree_equals_blocks(
      -12_e, {TypeBlock(BlockType::IntegerShort), ValueBlock(-12)});
  assert_tree_equals_blocks(
      -128_e, {TypeBlock(BlockType::IntegerShort), ValueBlock(-128)});
  assert_tree_equals_blocks(-129_e, {TypeBlock(BlockType::IntegerNegBig),
                                     ValueBlock(1), ValueBlock(129)});

  assert_tree_equals_blocks(
      π_e, {TypeBlock(BlockType::Constant),
            ValueBlock(static_cast<uint8_t>(Constant::Type::Pi))});
  assert_tree_equals_blocks(
      2.0_e, {TypeBlock(BlockType::Float), ValueBlock(0), ValueBlock(0),
              ValueBlock(0), ValueBlock(64)});
  assert_tree_equals_blocks(-1_e, {MinusOneBlock});
  assert_tree_equals_blocks(1_e, {OneBlock});
  assert_tree_equals_blocks(
      KAdd(1_e, 2_e),
      {TypeBlock(BlockType::Addition), ValueBlock(2), OneBlock, TwoBlock});
  assert_tree_equals_blocks(KMult(1_e, 2_e, -1_e),
                            {TypeBlock(BlockType::Multiplication),
                             ValueBlock(3), OneBlock, TwoBlock, MinusOneBlock});
  assert_tree_equals_blocks(
      KSet(1_e), {TypeBlock(BlockType::Set), ValueBlock(1), OneBlock});
  assert_tree_equals_blocks(KPow(1_e, 2_e),
                            {TypeBlock(BlockType::Power), OneBlock, TwoBlock});
  assert_tree_equals_blocks(KSqrt(2_e),
                            {TypeBlock(BlockType::SquareRoot), TwoBlock});
  assert_tree_equals_blocks(
      KSub(1_e, 2_e), {TypeBlock(BlockType::Subtraction), OneBlock, TwoBlock});
  assert_tree_equals_blocks(
      KSub(1_e, 2_e), {TypeBlock(BlockType::Subtraction), OneBlock, TwoBlock});
  assert_tree_equals_blocks(
      "var"_e, {TypeBlock(BlockType::UserSymbol), ValueBlock(3),
                ValueBlock('v'), ValueBlock('a'), ValueBlock('r')});
}

QUIZ_CASE(pcj_edition_node_constructor) {
  assert_node_equals_blocks(
      SharedEditionPool->push<BlockType::IntegerPosBig>(
          static_cast<uint64_t>(1232424242)),
      {TypeBlock(BlockType::IntegerPosBig), ValueBlock(4), ValueBlock(0x32),
       ValueBlock(0x4d), ValueBlock(0x75), ValueBlock(0x49)});
  assert_node_equals_blocks(
      SharedEditionPool->push<BlockType::IntegerNegBig>(
          static_cast<uint64_t>(1232424242)),
      {TypeBlock(BlockType::IntegerNegBig), ValueBlock(4), ValueBlock(0x32),
       ValueBlock(0x4d), ValueBlock(0x75), ValueBlock(0x49)});
  assert_node_equals_blocks(
      SharedEditionPool->push<BlockType::IntegerNegBig>(
          static_cast<uint64_t>(1232424242)),
      {TypeBlock(BlockType::IntegerNegBig), ValueBlock(4), ValueBlock(0x32),
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
  CachePool::SharedCachePool->reset();

  // operator==
  const Tree* node0 = 42_e;
  Tree* node1 =
      SharedEditionPool->push<BlockType::IntegerShort>(static_cast<int8_t>(42));
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
  Tree* node = SharedEditionPool->push<BlockType::IntegerPosBig>(
      static_cast<uint64_t>(0x00FF0000));
  quiz_assert(node->nodeSize() == 5);
  node = SharedEditionPool->push<BlockType::IntegerNegBig>(
      static_cast<uint64_t>(0x0000FF00));
  quiz_assert(node->nodeSize() == 4);
}

QUIZ_CASE(pcj_constructor) {
  assert_tree_equals_blocks(
      KRackL("1+"_l,
             KParenthesisL(KRackL(
                 "2*"_l, KParenthesisL(KRackL("1+"_l, KFracL("1"_l, "2"_l))))),
             KVertOffL("2"_l), "-2"_l),
      {
          TypeBlock(BlockType::RackLayout),
          ValueBlock(4),
          TypeBlock(BlockType::RackLayout),
          ValueBlock(2),
          TypeBlock(BlockType::CodePointLayout),
          ValueBlock('1'),
          ValueBlock(0),
          ValueBlock(0),
          ValueBlock(0),
          TypeBlock(BlockType::CodePointLayout),
          ValueBlock('+'),
          ValueBlock(0),
          ValueBlock(0),
          ValueBlock(0),
          TypeBlock(BlockType::ParenthesisLayout),
          TypeBlock(BlockType::RackLayout),
          ValueBlock(2),
          TypeBlock(BlockType::RackLayout),
          ValueBlock(2),
          TypeBlock(BlockType::CodePointLayout),
          ValueBlock('2'),
          ValueBlock(0),
          ValueBlock(0),
          ValueBlock(0),
          TypeBlock(BlockType::CodePointLayout),
          ValueBlock('*'),
          ValueBlock(0),
          ValueBlock(0),
          ValueBlock(0),
          TypeBlock(BlockType::ParenthesisLayout),
          TypeBlock(BlockType::RackLayout),
          ValueBlock(2),
          TypeBlock(BlockType::RackLayout),
          ValueBlock(2),
          TypeBlock(BlockType::CodePointLayout),
          ValueBlock('1'),
          ValueBlock(0),
          ValueBlock(0),
          ValueBlock(0),
          TypeBlock(BlockType::CodePointLayout),
          ValueBlock('+'),
          ValueBlock(0),
          ValueBlock(0),
          ValueBlock(0),
          TypeBlock(BlockType::FractionLayout),
          TypeBlock(BlockType::RackLayout),
          ValueBlock(1),
          TypeBlock(BlockType::CodePointLayout),
          ValueBlock('1'),
          ValueBlock(0),
          ValueBlock(0),
          ValueBlock(0),
          TypeBlock(BlockType::RackLayout),
          ValueBlock(1),
          TypeBlock(BlockType::CodePointLayout),
          ValueBlock('2'),
          ValueBlock(0),
          ValueBlock(0),
          ValueBlock(0),
          TypeBlock(BlockType::VerticalOffsetLayout),
          TypeBlock(BlockType::RackLayout),
          ValueBlock(1),
          TypeBlock(BlockType::CodePointLayout),
          ValueBlock('2'),
          ValueBlock(0),
          ValueBlock(0),
          ValueBlock(0),
          TypeBlock(BlockType::RackLayout),
          ValueBlock(2),
          TypeBlock(BlockType::CodePointLayout),
          ValueBlock('-'),
          ValueBlock(0),
          ValueBlock(0),
          ValueBlock(0),
          TypeBlock(BlockType::CodePointLayout),
          ValueBlock('2'),
          ValueBlock(0),
          ValueBlock(0),
          ValueBlock(0),
      });
}

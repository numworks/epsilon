#include "helper.h"
#include <poincare_junior/src/memory/node_iterator.h>
#include <poincare_junior/src/memory/value_block.h>
#include <poincare_junior/src/expression/k_creator.h>
#include <poincare_junior/src/layout/k_creator.h>

using namespace PoincareJ;

QUIZ_CASE(pcj_block) {
  CachePool * cachePool = CachePool::sharedCachePool();
  cachePool->reset();
  EditionPool * editionPool = cachePool->editionPool();

  // Create pool: [ "0" | "1" | "2" | 4 | -4 | "0" ]
  Block * firstBlock = editionPool->pushBlock(ZeroBlock);
  editionPool->pushBlock(OneBlock);
  editionPool->pushBlock(TwoBlock);
  editionPool->pushBlock(ValueBlock(4));
  editionPool->pushBlock(ValueBlock(-4));
  Block * lastBlock = editionPool->pushBlock(ZeroBlock);
  assert_pools_block_sizes_are(0, 6);

  // Block navigation
  quiz_assert(*firstBlock->nextNth(5) == *firstBlock);
  quiz_assert(*firstBlock->next() != *firstBlock);
  quiz_assert(*firstBlock->next() == OneBlock);
  quiz_assert(static_cast<uint8_t>(*firstBlock->nextNth(3)) == 4);
  quiz_assert(static_cast<int8_t>(*firstBlock->nextNth(4)) == -4);
  quiz_assert(*lastBlock->previous() == ValueBlock(-4));
  quiz_assert(*lastBlock->previousNth(2) == ValueBlock(4));
}

QUIZ_CASE(pcj_type_block) {
  typedef union {
    uint8_t m_value;
    struct {
      bool nAry: 1;
      bool expression: 1;
      bool layout: 1;
      bool integer: 1;
      bool rational: 1;
      bool number: 1;
      bool userNamed: 1;
    };
  } TypeBlockProperties;
  static_assert(sizeof(TypeBlockProperties) == sizeof(uint8_t), "TypeBlockProperties  has too many entries for an uint8_t");

  std::pair<BlockType, TypeBlockProperties> blockTypeTests[] = {
    std::make_pair(BlockType::Zero, TypeBlockProperties{.nAry = false, .expression = true, .layout = false, .integer = true, .rational = true, .number = true, .userNamed = false }),
    std::make_pair(BlockType::One, TypeBlockProperties{.nAry = false, .expression = true, .layout = false, .integer = true, .rational = true, .number = true, .userNamed = false }),
    std::make_pair(BlockType::Two, TypeBlockProperties{.nAry = false, .expression = true, .layout = false, .integer = true, .rational = true, .number = true, .userNamed = false }),
    std::make_pair(BlockType::Half, TypeBlockProperties{.nAry = false, .expression = true, .layout = false, .integer = false, .rational = true, .number = true, .userNamed = false }),
    std::make_pair(BlockType::MinusOne, TypeBlockProperties{.nAry = false, .expression = true, .layout = false, .integer = true, .rational = true, .number = true, .userNamed = false }),
    std::make_pair(BlockType::IntegerShort, TypeBlockProperties{.nAry = false, .expression = true, .layout = false, .integer = true, .rational = true, .number = true, .userNamed = false }),
    std::make_pair(BlockType::IntegerPosBig, TypeBlockProperties{.nAry = false, .expression = true, .layout = false, .integer = true, .rational = true, .number = true, .userNamed = false }),
    std::make_pair(BlockType::IntegerNegBig, TypeBlockProperties{.nAry = false, .expression = true, .layout = false, .integer = true, .rational = true, .number = true, .userNamed = false }),
    std::make_pair(BlockType::RationalShort, TypeBlockProperties{.nAry = false, .expression = true, .layout = false, .integer = false, .rational = true, .number = true, .userNamed = false }),
    std::make_pair(BlockType::RationalPosBig, TypeBlockProperties{.nAry = false, .expression = true, .layout = false, .integer = false, .rational = true, .number = true, .userNamed = false }),
    std::make_pair(BlockType::RationalNegBig, TypeBlockProperties{.nAry = false, .expression = true, .layout = false, .integer = false, .rational = true, .number = true, .userNamed = false }),
    std::make_pair(BlockType::Float, TypeBlockProperties{.nAry = false, .expression = true, .layout = false, .integer = false, .rational = false, .number = true, .userNamed = false }),
    std::make_pair(BlockType::UserSymbol, TypeBlockProperties{.nAry = false, .expression = true, .layout = false, .integer = false, .rational = false, .number = false, .userNamed = true }),
    std::make_pair(BlockType::UserFunction, TypeBlockProperties{.nAry = false, .expression = true, .layout = false, .integer = false, .rational = false, .number = false, .userNamed = true }),
    std::make_pair(BlockType::UserSequence, TypeBlockProperties{.nAry = false, .expression = true, .layout = false, .integer = false, .rational = false, .number = false, .userNamed = true }),
    std::make_pair(BlockType::Addition, TypeBlockProperties{.nAry = true, .expression = true, .layout = false, .integer = false, .rational = false, .number = false, .userNamed = false }),
    std::make_pair(BlockType::Multiplication, TypeBlockProperties{.nAry = true, .expression = true, .layout = false, .integer = false, .rational = false, .number = false, .userNamed = false }),
    std::make_pair(BlockType::Set, TypeBlockProperties{.nAry = true, .expression = true, .layout = false, .integer = false, .rational = false, .number = false, .userNamed = false }),
    std::make_pair(BlockType::List, TypeBlockProperties{.nAry = true, .expression = true, .layout = false, .integer = false, .rational = false, .number = false, .userNamed = false }),
    std::make_pair(BlockType::Constant, TypeBlockProperties{.nAry = false, .expression = true, .layout = false, .integer = false, .rational = false, .number = false, .userNamed = false }),
    std::make_pair(BlockType::Power, TypeBlockProperties{.nAry = false, .expression = true, .layout = false, .integer = false, .rational = false, .number = false, .userNamed = false }),
    std::make_pair(BlockType::Factorial, TypeBlockProperties{.nAry = false, .expression = true, .layout = false, .integer = false, .rational = false, .number = false, .userNamed = false }),
    std::make_pair(BlockType::Subtraction, TypeBlockProperties{.nAry = false, .expression = true, .layout = false, .integer = false, .rational = false, .number = false, .userNamed = false }),
    std::make_pair(BlockType::Division, TypeBlockProperties{.nAry = false, .expression = true, .layout = false, .integer = false, .rational = false, .number = false, .userNamed = false }),
    std::make_pair(BlockType::RackLayout, TypeBlockProperties{.nAry = true, .expression = false, .layout = true, .integer = false, .rational = false, .number = false, .userNamed = false }),
    std::make_pair(BlockType::FractionLayout, TypeBlockProperties{.nAry = false, .expression = false, .layout = true, .integer = false, .rational = false, .number = false, .userNamed = false }),
    std::make_pair(BlockType::ParenthesisLayout, TypeBlockProperties{.nAry = false, .expression = false, .layout = true, .integer = false, .rational = false, .number = false, .userNamed = false }),
    std::make_pair(BlockType::VerticalOffsetLayout, TypeBlockProperties{.nAry = false, .expression = false, .layout = true, .integer = false, .rational = false, .number = false, .userNamed = false }),
    std::make_pair(BlockType::CodePointLayout, TypeBlockProperties{.nAry = false, .expression = false, .layout = true, .integer = false, .rational = false, .number = false, .userNamed = false })
  };

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

void assert_tree_equals_blocks(Node node, std::initializer_list<Block> blocks) {
  assert_node_equals_blocks(node, blocks);
  quiz_assert(blocks.size() == node.treeSize());
}

QUIZ_CASE(pcj_constexpr_tree_constructor) {
  Node n = 12_e;
  assert_tree_equals_blocks(0_e, {TypeBlock(BlockType::Zero)});
  assert_tree_equals_blocks(1_e, {TypeBlock(BlockType::One)});
  assert_tree_equals_blocks(2_e, {TypeBlock(BlockType::Two)});
  assert_tree_equals_blocks(12_e, {TypeBlock(BlockType::IntegerShort), ValueBlock(12), TypeBlock(BlockType::IntegerShort)});
  assert_tree_equals_blocks(127_e, {TypeBlock(BlockType::IntegerShort), ValueBlock(127), TypeBlock(BlockType::IntegerShort)});
  assert_tree_equals_blocks(128_e, {TypeBlock(BlockType::IntegerPosBig), ValueBlock(1), ValueBlock(128), ValueBlock(1), TypeBlock(BlockType::IntegerPosBig)});
  assert_tree_equals_blocks(256_e, {TypeBlock(BlockType::IntegerPosBig), ValueBlock(2), ValueBlock(0), ValueBlock(1), ValueBlock(2), TypeBlock(BlockType::IntegerPosBig)});
  assert_tree_equals_blocks(-1_e, {TypeBlock(BlockType::MinusOne)});
  assert_tree_equals_blocks(-12_e, {TypeBlock(BlockType::IntegerShort), ValueBlock(-12), TypeBlock(BlockType::IntegerShort)});
  assert_tree_equals_blocks(-128_e, {TypeBlock(BlockType::IntegerShort), ValueBlock(-128), TypeBlock(BlockType::IntegerShort)});
  assert_tree_equals_blocks(-129_e, {TypeBlock(BlockType::IntegerNegBig), ValueBlock(1), ValueBlock(129), ValueBlock(1), TypeBlock(BlockType::IntegerNegBig)});

  assert_tree_equals_blocks(π_e, {TypeBlock(BlockType::Constant), ValueBlock(static_cast<uint8_t>(Constant::Type::Pi)), TypeBlock(BlockType::Constant)});
  assert_tree_equals_blocks(2.0_e, {TypeBlock(BlockType::Float), ValueBlock(0), ValueBlock(0), ValueBlock(0), ValueBlock(64), TypeBlock(BlockType::Float)});
  assert_tree_equals_blocks(-1_e, {MinusOneBlock});
  assert_tree_equals_blocks(1_e, {OneBlock});
  assert_tree_equals_blocks(KAdd(1_e, 2_e), {TypeBlock(BlockType::Addition), ValueBlock(2), TypeBlock(BlockType::Addition), OneBlock, TwoBlock});
  assert_tree_equals_blocks(KMult(1_e, 2_e, -1_e), {TypeBlock(BlockType::Multiplication), ValueBlock(3), TypeBlock(BlockType::Multiplication), OneBlock, TwoBlock, MinusOneBlock});
  assert_tree_equals_blocks(KSet(1_e), {TypeBlock(BlockType::Set), ValueBlock(1), TypeBlock(BlockType::Set), OneBlock});
  assert_tree_equals_blocks(KPow(1_e, 2_e), {TypeBlock(BlockType::Power), OneBlock, TwoBlock});
  assert_tree_equals_blocks(KSub(1_e, 2_e), {TypeBlock(BlockType::Subtraction), OneBlock, TwoBlock});
  assert_tree_equals_blocks(KSub(1_e, 2_e), {TypeBlock(BlockType::Subtraction), OneBlock, TwoBlock});
  assert_tree_equals_blocks("var"_e, {TypeBlock(BlockType::UserSymbol), ValueBlock(3), ValueBlock('v'), ValueBlock('a'), ValueBlock('r'), ValueBlock(3), TypeBlock(BlockType::UserSymbol)});
}

QUIZ_CASE(pcj_edition_node_constructor) {
  assert_node_equals_blocks(EditionReference::Push<BlockType::IntegerPosBig>(static_cast<uint64_t>(1232424242)), {TypeBlock(BlockType::IntegerPosBig), ValueBlock(4), ValueBlock(0x32), ValueBlock(0x4d), ValueBlock(0x75), ValueBlock(0x49), ValueBlock(4), TypeBlock(BlockType::IntegerPosBig)});
  assert_node_equals_blocks(EditionReference::Push<BlockType::IntegerNegBig>(static_cast<uint64_t>(1232424242)), {TypeBlock(BlockType::IntegerNegBig), ValueBlock(4), ValueBlock(0x32), ValueBlock(0x4d), ValueBlock(0x75), ValueBlock(0x49), ValueBlock(4), TypeBlock(BlockType::IntegerNegBig)});
  assert_node_equals_blocks(EditionReference::Push<BlockType::IntegerNegBig>(static_cast<uint64_t>(1232424242)), {TypeBlock(BlockType::IntegerNegBig), ValueBlock(4), ValueBlock(0x32), ValueBlock(0x4d), ValueBlock(0x75), ValueBlock(0x49), ValueBlock(4), TypeBlock(BlockType::IntegerNegBig)});
}

QUIZ_CASE(pcj_node_iterator) {
  constexpr Tree k_simpleExpression = KMult(KAdd(1_e, 2_e), 3_e, 4_e);
  EditionReference mult(k_simpleExpression);
  size_t numberOfChildren = mult.numberOfChildren();
  Tree a = KAdd(1_e, 2_e);
  Tree b = 3_e;
  Tree c = 4_e;
  Node children[] = {a, b, c};

  // Scan children forward
  for (const std::pair<Node, int> indexedNode : NodeIterator::Children<Forward, NoEditable>(mult)) {
    assert_trees_are_equal(std::get<Node>(indexedNode), children[std::get<int>(indexedNode)]);
  }

  // Scan children backward
  for (const std::pair<Node, int>indexedNode : NodeIterator::Children<Backward, NoEditable>(mult)) {
    assert_trees_are_equal(std::get<Node>(indexedNode), children[numberOfChildren - 1 - std::get<int>(indexedNode)]);
  }

  // Edit children forward
  Tree e = 6_e;
  Tree f = 7_e;
  Tree g = 8_e;
  Node newChildren[] = {e, f, g};
  for (std::pair<EditionReference, int> indexedRef : NodeIterator::Children<Forward, Editable>(mult)) {
    std::get<EditionReference>(indexedRef).replaceTreeByTree(newChildren[std::get<int>(indexedRef)]);
  }
  // Check edition
  for (const std::pair<Node, int> indexedNode : NodeIterator::Children<Forward, NoEditable>(mult)) {
    assert_trees_are_equal(std::get<Node>(indexedNode), newChildren[std::get<int>(indexedNode)]);
  }

  // Edit children backward
  for (std::pair<EditionReference, int> indexedRef : NodeIterator::Children<Backward, Editable>(mult)) {
    std::get<EditionReference>(indexedRef).replaceTreeByTree(newChildren[std::get<int>(indexedRef)]);
  }
  // Check edition
  for (const std::pair<Node, int> indexedNode : NodeIterator::Children<Forward, NoEditable>(mult)) {
    assert_trees_are_equal(std::get<Node>(indexedNode), newChildren[numberOfChildren - 1 - std::get<int>(indexedNode)]);
  }

  constexpr Tree k_secondSimpleExpression = KMult(KAdd(1_e, 2_e), 3_e);
  EditionReference mult2(k_secondSimpleExpression);
  size_t numberOfChildren2 = mult2.numberOfChildren();
  Node children2[] = {a, b};
  // Scan two nodes children forward
  for (std::pair<std::array<Node, 2>, int> indexedArray : MultipleNodesIterator::Children<Forward, NoEditable, 2>(std::array<Node, 2>({mult, mult2}))) {
    std::array<Node, 2> childrenPair = std::get<std::array<Node, 2>>(indexedArray);
    int pairIndex = std::get<int>(indexedArray);
    assert_trees_are_equal(childrenPair[0], newChildren[numberOfChildren - 1 - pairIndex]);
    assert_trees_are_equal(childrenPair[1], children2[pairIndex]);
  }

  // Scan two nodes children backward
  for (std::pair<std::array<Node, 2>, int> indexedArray : MultipleNodesIterator::Children<Backward, NoEditable, 2>(std::array<Node, 2>({mult, mult2}))) {
    std::array<Node, 2> childrenPair = std::get<std::array<Node, 2>>(indexedArray);
    int pairIndex = std::get<int>(indexedArray);
    assert_trees_are_equal(childrenPair[0], newChildren[pairIndex]);
    assert_trees_are_equal(childrenPair[1], children2[numberOfChildren2 - 1 - pairIndex]);
  }

  Tree n6 = 6_e;
  Tree n10 = 10_e;
  Tree n11 = 11_e;
  Tree n13 = 13_e;
  Tree n14 = 14_e;
  Node newChildren1[] = {n10, n11};
  Node newChildren2[] = {n13, n14};
  // Edit two nodes children forward
  for (std::pair<std::array<EditionReference, 2>, int> indexedRefs : MultipleNodesIterator::Children<Forward, Editable, 2>(std::array<EditionReference, 2>({mult, mult2}))) {
    std::array<EditionReference, 2> childrenPair = std::get<std::array<EditionReference, 2>>(indexedRefs);
    int pairIndex = std::get<int>(indexedRefs);
    childrenPair[0].replaceTreeByTree(newChildren1[pairIndex]);
    childrenPair[1].replaceTreeByTree(newChildren2[pairIndex]);
  }
  // Check edition
  Node children1[] = {n10, n11, n6};
  for (const std::pair<Node, int> indexedNode : NodeIterator::Children<Forward, NoEditable>(mult)) {
    assert_trees_are_equal(std::get<Node>(indexedNode), children1[std::get<int>(indexedNode)]);
  }
  for (const std::pair<Node, int> indexedNode : NodeIterator::Children<Forward, NoEditable>(mult2)) {
    assert_trees_are_equal(std::get<Node>(indexedNode), newChildren2[std::get<int>(indexedNode)]);
  }

  // Edit two nodes children backward
  for (std::pair<std::array<EditionReference, 2>, int> indexedRefs : MultipleNodesIterator::Children<Backward, Editable, 2>(std::array<EditionReference, 2>({mult, mult2}))) {
    std::array<EditionReference, 2> childrenPair = std::get<std::array<EditionReference, 2>>(indexedRefs);
    int pairIndex = std::get<int>(indexedRefs);
    childrenPair[0].replaceTreeByTree(newChildren1[pairIndex]);
    childrenPair[1].replaceTreeByTree(newChildren2[pairIndex]);
  }
  // Check edition
  Node editedChildren1[] = {n10, n11, n10};
  for (const std::pair<Node, int> indexedNode : NodeIterator::Children<Forward, NoEditable>(mult)) {
    assert_trees_are_equal(std::get<Node>(indexedNode), editedChildren1[std::get<int>(indexedNode)]);
  }
  for (const std::pair<Node, int> indexedNode : NodeIterator::Children<Forward, NoEditable>(mult2)) {
    assert_trees_are_equal(std::get<Node>(indexedNode), newChildren2[numberOfChildren2 - 1 - std::get<int>(indexedNode)]);
  }
}

QUIZ_CASE(pcj_node) {
  CachePool * cachePool = CachePool::sharedCachePool();
  cachePool->reset();
  EditionPool * editionPool = cachePool->editionPool();

  // operator==
  Node node0 = 42_e;
  Node node1 = EditionReference::Push<BlockType::IntegerShort>(static_cast<int8_t>(42));
  quiz_assert(node0 != node1 && *node0.block() == *node1.block());
  Node node2(editionPool->firstBlock());
  quiz_assert(node2 == node1);

  // Node navigation
  constexpr Tree e1 = KMult(KAdd(1_e, 2_e), 3_e, 4_e);
  constexpr Tree e2 = KPow(5_e, 6_e);
  Node n1 = EditionReference(e1);
  Node n2 = EditionReference(e2);
  quiz_assert(n1.treeSize() == 14); // TODO: Magic Number
  assert_trees_are_equal(n1.nextNode(), KAdd(1_e, 2_e));
  assert_trees_are_equal(n1.nextTree(), e2);
  assert_trees_are_equal(n2.previousNode(), 4_e);
  assert_trees_are_equal(n2.previousTree(), e1);
  assert_trees_are_equal(n1.nextNode().nextNode().parent(), n1.nextNode());
  assert_trees_are_equal(n1.nextNode().nextNode().root(), n1);
  quiz_assert(n1.numberOfDescendants(false) == 5);
  quiz_assert(n1.numberOfDescendants(true) == 6);
  assert_trees_are_equal(n1.childAtIndex(0), n1.nextNode());
  assert_trees_are_equal(n1.childAtIndex(1), n1.nextNode().nextNode().nextNode().nextNode());
  quiz_assert(n1.indexOfChild(n1.childAtIndex(1)) == 1);
  quiz_assert(n1.childAtIndex(0).indexInParent() == 0);
  quiz_assert(!n1.hasChild(e2));
  quiz_assert(n1.hasChild(n1.childAtIndex(2)));
  quiz_assert(!n1.hasSibling(n1.childAtIndex(2)));
  quiz_assert(n1.nextNode().hasSibling(n1.childAtIndex(2)));
  quiz_assert(n1.commonAncestorWith(n1) == n1);
  quiz_assert(n1.commonAncestorWith(n1.childAtIndex(0).childAtIndex(1)) == n1);
  quiz_assert(n1.childAtIndex(0).childAtIndex(1).commonAncestorWith(n1) == n1);
  quiz_assert(n1.childAtIndex(0).childAtIndex(1).commonAncestorWith(n1.childAtIndex(2)) == n1);
}

QUIZ_CASE(pcj_node_size) {
  Node node = EditionReference::Push<BlockType::IntegerPosBig>(static_cast<uint64_t>(0x00FF0000));
  quiz_assert(node.nodeSize() == 7);
  node = static_cast<Node>(EditionReference::Push<BlockType::IntegerNegBig>(static_cast<uint64_t>(0x0000FF00)));
  quiz_assert(node.nodeSize() == 6);
}

QUIZ_CASE(pcj_constructor) {
  assert_tree_equals_blocks(
    KRackL(
      "1+"_l,
      KParenthesisL(
        KRackL(
          "2*"_l,
          KParenthesisL(
            KRackL(
              "1+"_l,
              KFracL("1"_l, "2"_l)
            )
          )
        )
      ),
      KVertOffL("2"_l),
      "-2"_l
    ),
    {
      TypeBlock(BlockType::RackLayout), ValueBlock(4), TypeBlock(BlockType::RackLayout),
      TypeBlock(BlockType::RackLayout), ValueBlock(2), TypeBlock(BlockType::RackLayout),
      TypeBlock(BlockType::CodePointLayout), ValueBlock('1'), ValueBlock(0), ValueBlock(0), ValueBlock(0), TypeBlock(BlockType::CodePointLayout),
      TypeBlock(BlockType::CodePointLayout), ValueBlock('+'), ValueBlock(0), ValueBlock(0), ValueBlock(0), TypeBlock(BlockType::CodePointLayout),
      TypeBlock(BlockType::ParenthesisLayout),
      TypeBlock(BlockType::RackLayout), ValueBlock(2), TypeBlock(BlockType::RackLayout),
      TypeBlock(BlockType::RackLayout), ValueBlock(2), TypeBlock(BlockType::RackLayout),
      TypeBlock(BlockType::CodePointLayout), ValueBlock('2'), ValueBlock(0), ValueBlock(0), ValueBlock(0), TypeBlock(BlockType::CodePointLayout),
      TypeBlock(BlockType::CodePointLayout), ValueBlock('*'), ValueBlock(0), ValueBlock(0), ValueBlock(0), TypeBlock(BlockType::CodePointLayout),
      TypeBlock(BlockType::ParenthesisLayout),
      TypeBlock(BlockType::RackLayout), ValueBlock(2), TypeBlock(BlockType::RackLayout),
      TypeBlock(BlockType::RackLayout), ValueBlock(2), TypeBlock(BlockType::RackLayout),
      TypeBlock(BlockType::CodePointLayout), ValueBlock('1'), ValueBlock(0), ValueBlock(0), ValueBlock(0), TypeBlock(BlockType::CodePointLayout),
      TypeBlock(BlockType::CodePointLayout), ValueBlock('+'), ValueBlock(0), ValueBlock(0), ValueBlock(0), TypeBlock(BlockType::CodePointLayout),
      TypeBlock(BlockType::FractionLayout),
      TypeBlock(BlockType::RackLayout), ValueBlock(1), TypeBlock(BlockType::RackLayout),
      TypeBlock(BlockType::CodePointLayout), ValueBlock('1'), ValueBlock(0), ValueBlock(0), ValueBlock(0), TypeBlock(BlockType::CodePointLayout),
      TypeBlock(BlockType::RackLayout), ValueBlock(1), TypeBlock(BlockType::RackLayout),
      TypeBlock(BlockType::CodePointLayout), ValueBlock('2'), ValueBlock(0), ValueBlock(0), ValueBlock(0), TypeBlock(BlockType::CodePointLayout),
      TypeBlock(BlockType::VerticalOffsetLayout),
      TypeBlock(BlockType::RackLayout), ValueBlock(1), TypeBlock(BlockType::RackLayout),
      TypeBlock(BlockType::CodePointLayout), ValueBlock('2'), ValueBlock(0), ValueBlock(0), ValueBlock(0), TypeBlock(BlockType::CodePointLayout),
      TypeBlock(BlockType::RackLayout), ValueBlock(2), TypeBlock(BlockType::RackLayout),
      TypeBlock(BlockType::CodePointLayout), ValueBlock('-'), ValueBlock(0), ValueBlock(0), ValueBlock(0), TypeBlock(BlockType::CodePointLayout),
      TypeBlock(BlockType::CodePointLayout), ValueBlock('2'), ValueBlock(0), ValueBlock(0), ValueBlock(0), TypeBlock(BlockType::CodePointLayout),
    }
  );
}

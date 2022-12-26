#include "print.h"
#include <poincare_junior/src/memory/node_iterator.h>
#include <poincare_junior/src/memory/tree_constructor.h>

using namespace Poincare;

void testBlock() {
  CachePool * cachePool = CachePool::sharedCachePool();
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
  assert(*firstBlock->nextNth(5) == *firstBlock);
  assert(*firstBlock->next() != *firstBlock);
  assert(*firstBlock->next() == OneBlock);
  assert(static_cast<uint8_t>(*firstBlock->nextNth(3)) == 4);
  assert(static_cast<int8_t>(*firstBlock->nextNth(4)) == -4);
  assert(*lastBlock->previous() == ValueBlock(-4));
  assert(*lastBlock->previousNth(2) == ValueBlock(4));
}

void testTypeBlock() {
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
    std::make_pair(BlockType::Division, TypeBlockProperties{.nAry = false, .expression = true, .layout = false, .integer = false, .rational = false, .number = false, .userNamed = false })
  };

  for (std::pair<BlockType, TypeBlockProperties> test : blockTypeTests) {
    TypeBlock block = TypeBlock(std::get<BlockType>(test));
    TypeBlockProperties properties = std::get<TypeBlockProperties>(test);
    assert(block.isNAry() == properties.nAry);
    assert(block.isExpression() == properties.expression);
    assert(block.isLayout() == properties.layout);
    assert(block.isInteger() == properties.integer);
    assert(block.isRational() == properties.rational);
    assert(block.isNumber() == properties.number);
    assert(block.isUserNamed() == properties.userNamed);
  }
}

template <unsigned N>
void assert_tree_equals_blocks(Tree<N> tree, std::initializer_list<Block> blocks) {
  assert_node_equals_blocks(static_cast<Node>(tree), blocks);
  assert(blocks.size() == N);
}

void testConstexprTreeConstructor() {
  assert_tree_equals_blocks(u'π'_n, {TypeBlock(BlockType::Constant), ValueBlock(static_cast<uint8_t>(Constant::Type::Pi)), TypeBlock(BlockType::Constant)});
  assert_tree_equals_blocks(2.0_fn, {TypeBlock(BlockType::Float), ValueBlock(0), ValueBlock(0), ValueBlock(0), ValueBlock(64), TypeBlock(BlockType::Float)});
  assert_tree_equals_blocks(1_nsn, {MinusOneBlock});
  assert_tree_equals_blocks(1_sn, {OneBlock});
  assert_tree_equals_blocks(Add(1_sn, 2_sn), {TypeBlock(BlockType::Addition), ValueBlock(2), TypeBlock(BlockType::Addition), OneBlock, TwoBlock});
  assert_tree_equals_blocks(Mult(1_sn, 2_sn, 1_nsn), {TypeBlock(BlockType::Multiplication), ValueBlock(3), TypeBlock(BlockType::Multiplication), OneBlock, TwoBlock, MinusOneBlock});
  assert_tree_equals_blocks(Set(1_sn), {TypeBlock(BlockType::Set), ValueBlock(1), TypeBlock(BlockType::Set), OneBlock});
  assert_tree_equals_blocks(Pow(1_sn, 2_sn), {TypeBlock(BlockType::Power), OneBlock, TwoBlock});
  assert_tree_equals_blocks(Sub(1_sn, 2_sn), {TypeBlock(BlockType::Subtraction), OneBlock, TwoBlock});
}

void testEditionNodeConstructor() {
  assert_node_equals_blocks(EditionReference::Push<BlockType::IntegerPosBig>(1232424242).node(), {TypeBlock(BlockType::IntegerPosBig), ValueBlock(4), ValueBlock(0x32), ValueBlock(0x4d), ValueBlock(0x75), ValueBlock(0x49), ValueBlock(4), TypeBlock(BlockType::IntegerPosBig)});
  assert_node_equals_blocks(EditionReference::Push<BlockType::IntegerNegBig>(1232424242).node(), {TypeBlock(BlockType::IntegerNegBig), ValueBlock(4), ValueBlock(0x32), ValueBlock(0x4d), ValueBlock(0x75), ValueBlock(0x49), ValueBlock(4), TypeBlock(BlockType::IntegerNegBig)});
  assert_node_equals_blocks(EditionReference::Push<BlockType::IntegerNegBig>(-1232424242).node(), {TypeBlock(BlockType::IntegerNegBig), ValueBlock(4), ValueBlock(0x32), ValueBlock(0x4d), ValueBlock(0x75), ValueBlock(0x49), ValueBlock(4), TypeBlock(BlockType::IntegerNegBig)});
}

void testNodeIterator() {
  constexpr Tree k_simpleExpression = Mult(Add(1_sn, 2_sn), 3_n, 4_n);
  EditionReference mult(k_simpleExpression);
  size_t numberOfChildren = mult.node().numberOfChildren();
  Node children[] = {Add(1_sn, 2_sn), 3_n, 4_n};

  // Scan children forward
  for (const std::pair<Node, int> indexedNode : NodeIterator::Children<Forward, NoEditable>(mult.node())) {
    assert_trees_are_equal(std::get<Node>(indexedNode), children[std::get<int>(indexedNode)]);
  }

  // Scan children backward
  for (const std::pair<Node, int>indexedNode : NodeIterator::Children<Backward, NoEditable>(mult.node())) {
    assert_trees_are_equal(std::get<Node>(indexedNode), children[numberOfChildren - 1 - std::get<int>(indexedNode)]);
  }

  // Edit children forward
  Node newChildren[] = {6_n, 7_n, 8_n};
  for (std::pair<EditionReference, int> indexedRef : NodeIterator::Children<Forward, Editable>(mult)) {
    std::get<EditionReference>(indexedRef).replaceTreeByTree(newChildren[std::get<int>(indexedRef)]);
  }
  // Check edition
  for (const std::pair<Node, int> indexedNode : NodeIterator::Children<Forward, NoEditable>(mult.node())) {
    assert_trees_are_equal(std::get<Node>(indexedNode), newChildren[std::get<int>(indexedNode)]);
  }

  // Edit children backward
  for (std::pair<EditionReference, int> indexedRef : NodeIterator::Children<Backward, Editable>(mult)) {
    std::get<EditionReference>(indexedRef).replaceTreeByTree(newChildren[std::get<int>(indexedRef)]);
  }
  // Check edition
  for (const std::pair<Node, int> indexedNode : NodeIterator::Children<Forward, NoEditable>(mult.node())) {
    assert_trees_are_equal(std::get<Node>(indexedNode), newChildren[numberOfChildren - 1 - std::get<int>(indexedNode)]);
  }

  constexpr Tree k_secondSimpleExpression = Mult(Add(1_sn, 2_sn), 3_n);
  EditionReference mult2(k_secondSimpleExpression);
  size_t numberOfChildren2 = mult2.node().numberOfChildren();
  Node children2[] = {Add(1_sn, 2_sn), 3_n};
  // Scan two nodes children forward
  for (std::pair<std::array<Node, 2>, int> indexedArray : MultipleNodesIterator::Children<Forward, NoEditable, 2>(std::array<Node, 2>({mult.node(), mult2.node()}))) {
    std::array<Node, 2> childrenPair = std::get<0>(indexedArray);
    int pairIndex = std::get<int>(indexedArray);
    assert_trees_are_equal(childrenPair[0], newChildren[numberOfChildren - 1 - pairIndex]);
    assert_trees_are_equal(childrenPair[1], children2[pairIndex]);
  }

  // Scan two nodes children backward
  for (std::pair<std::array<Node, 2>, int> indexedArray : MultipleNodesIterator::Children<Backward, NoEditable, 2>(std::array<Node, 2>({mult.node(), mult2.node()}))) {
    std::array<Node, 2> childrenPair = std::get<0>(indexedArray);
    int pairIndex = std::get<int>(indexedArray);
    assert_trees_are_equal(childrenPair[0], newChildren[pairIndex]);
    assert_trees_are_equal(childrenPair[1], children2[numberOfChildren2 - 1 - pairIndex]);
  }

  Node newChildren1[] = {10_n, 11_n};
  Node newChildren2[] = {13_n, 14_n};
  // Edit two nodes children forward
  for (std::pair<std::array<EditionReference, 2>, int> indexedRefs : MultipleNodesIterator::Children<Forward, Editable, 2>(std::array<EditionReference, 2>({mult, mult2}))) {
    std::array<EditionReference, 2> childrenPair = std::get<0>(indexedRefs);
    int pairIndex = std::get<int>(indexedRefs);
    childrenPair[0].replaceTreeByTree(newChildren1[pairIndex]);
    childrenPair[1].replaceTreeByTree(newChildren2[pairIndex]);
  }
  // Check edition
  Node children1[] = {10_n, 11_n, 6_n};
  for (const std::pair<Node, int> indexedNode : NodeIterator::Children<Forward, NoEditable>(mult.node())) {
    assert_trees_are_equal(std::get<Node>(indexedNode), children1[std::get<int>(indexedNode)]);
  }
  for (const std::pair<Node, int> indexedNode : NodeIterator::Children<Forward, NoEditable>(mult2.node())) {
    assert_trees_are_equal(std::get<Node>(indexedNode), newChildren2[std::get<int>(indexedNode)]);
  }

  // Edit two nodes children backward
  for (std::pair<std::array<EditionReference, 2>, int> indexedRefs : MultipleNodesIterator::Children<Backward, Editable, 2>(std::array<EditionReference, 2>({mult, mult2}))) {
    std::array<EditionReference, 2> childrenPair = std::get<0>(indexedRefs);
    int pairIndex = std::get<int>(indexedRefs);
    childrenPair[0].replaceTreeByTree(newChildren1[pairIndex]);
    childrenPair[1].replaceTreeByTree(newChildren2[pairIndex]);
  }
  // Check edition
  Node editedChildren1[] = {10_n, 11_n, 10_n};
  for (const std::pair<Node, int> indexedNode : NodeIterator::Children<Forward, NoEditable>(mult.node())) {
    assert_trees_are_equal(std::get<Node>(indexedNode), editedChildren1[std::get<int>(indexedNode)]);
  }
  for (const std::pair<Node, int> indexedNode : NodeIterator::Children<Forward, NoEditable>(mult2.node())) {
    assert_trees_are_equal(std::get<Node>(indexedNode), newChildren2[numberOfChildren2 - 1 - std::get<int>(indexedNode)]);
  }
}

void testNode() {
  CachePool * cachePool = CachePool::sharedCachePool();
  EditionPool * editionPool = cachePool->editionPool();

  // operator==
  Node node0 = 42_n;
  Node node1 = EditionReference::Push<BlockType::IntegerShort>(42).node();
  assert(node0 != node1 && *node0.block() == *node1.block());
  Node node2(editionPool->firstBlock());
  assert(node2 == node1);

  // Node navigation
  constexpr Tree e1 = Mult(Add(1_sn, 2_sn), 3_n, 4_n);
  constexpr Tree e2 = Pow(5_n, 6_n);
  Node n1 = EditionReference(e1).node();
  Node n2 = EditionReference(e2).node();
  assert(n1.treeSize() == 14); // TODO: Magic Number
  assert_trees_are_equal(n1.nextNode(), Add(1_sn, 2_sn));
  assert_trees_are_equal(n1.nextTree(), e2);
  assert_trees_are_equal(n2.previousNode(), 4_n);
  assert_trees_are_equal(n2.previousTree(), e1);
  assert_trees_are_equal(n1.nextNode().nextNode().parent(), n1.nextNode());
  assert_trees_are_equal(n1.nextNode().nextNode().root(), n1);
  assert(n1.numberOfDescendants(false) == 5);
  assert(n1.numberOfDescendants(true) == 6);
  assert_trees_are_equal(n1.childAtIndex(0), n1.nextNode());
  assert_trees_are_equal(n1.childAtIndex(1), n1.nextNode().nextNode().nextNode().nextNode());
  assert(n1.indexOfChild(n1.childAtIndex(1)) == 1);
  assert(n1.childAtIndex(0).indexInParent() == 0);
  assert(!n1.hasChild(e2));
  assert(n1.hasChild(n1.childAtIndex(2)));
  assert(!n1.hasSibling(n1.childAtIndex(2)));
  assert(n1.nextNode().hasSibling(n1.childAtIndex(2)));
}

void testNodeSize() {
  Node node = EditionReference::Push<BlockType::IntegerPosBig>(0x00FF0000).node();
  assert(node.nodeSize() == 7);
  node = EditionReference::Push<BlockType::IntegerNegBig>(0x0000FF00).node();
  assert(node.nodeSize() == 6);
}


#include "print.h"
#include <poincare_junior/src/memory/node_iterator.h>
#include <poincare_junior/src/memory/tree_constructor.h>

using namespace PoincareJ;

void testBlock() {
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
  assert(*firstBlock->nextNth(5) == *firstBlock);
  assert(*firstBlock->next() != *firstBlock);
  assert(*firstBlock->next() == OneBlock);
  assert(static_cast<uint8_t>(*firstBlock->nextNth(3)) == 4);
  assert(static_cast<int8_t>(*firstBlock->nextNth(4)) == -4);
  assert(*lastBlock->previous() == ValueBlock(-4));
  assert(*lastBlock->previousNth(2) == ValueBlock(4));
}
QUIZ_CASE(pcj_block) { testBlock(); }

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
QUIZ_CASE(pcj_type_block) { testTypeBlock(); }

template <unsigned N>
void assert_tree_equals_blocks(Tree<N> tree, std::initializer_list<Block> blocks) {
  assert_node_equals_blocks(static_cast<Node>(tree), blocks);
  assert(blocks.size() == N);
}

void testConstexprTreeConstructor() {
  Node n = "12"_n;
  assert_tree_equals_blocks("0"_n, {TypeBlock(BlockType::Zero)});
  assert_tree_equals_blocks("1"_n, {TypeBlock(BlockType::One)});
  assert_tree_equals_blocks("2"_n, {TypeBlock(BlockType::Two)});
  assert_tree_equals_blocks("12"_n, {TypeBlock(BlockType::IntegerShort), ValueBlock(12), TypeBlock(BlockType::IntegerShort)});
  assert_tree_equals_blocks("127"_n, {TypeBlock(BlockType::IntegerShort), ValueBlock(127), TypeBlock(BlockType::IntegerShort)});
  assert_tree_equals_blocks("128"_n, {TypeBlock(BlockType::IntegerPosBig), ValueBlock(1), ValueBlock(128), ValueBlock(1), TypeBlock(BlockType::IntegerPosBig)});
  assert_tree_equals_blocks("256"_n, {TypeBlock(BlockType::IntegerPosBig), ValueBlock(2), ValueBlock(0), ValueBlock(1), ValueBlock(2), TypeBlock(BlockType::IntegerPosBig)});
  assert_tree_equals_blocks("-1"_n, {TypeBlock(BlockType::MinusOne)});
  assert_tree_equals_blocks("-12"_n, {TypeBlock(BlockType::IntegerShort), ValueBlock(-12), TypeBlock(BlockType::IntegerShort)});
  assert_tree_equals_blocks("-128"_n, {TypeBlock(BlockType::IntegerShort), ValueBlock(-128), TypeBlock(BlockType::IntegerShort)});
  assert_tree_equals_blocks("-129"_n, {TypeBlock(BlockType::IntegerNegBig), ValueBlock(1), ValueBlock(129), ValueBlock(1), TypeBlock(BlockType::IntegerNegBig)});

  assert_tree_equals_blocks(u'π'_n, {TypeBlock(BlockType::Constant), ValueBlock(static_cast<uint8_t>(Constant::Type::Pi)), TypeBlock(BlockType::Constant)});
  assert_tree_equals_blocks(2.0_n, {TypeBlock(BlockType::Float), ValueBlock(0), ValueBlock(0), ValueBlock(0), ValueBlock(64), TypeBlock(BlockType::Float)});
  assert_tree_equals_blocks("-1"_n, {MinusOneBlock});
  assert_tree_equals_blocks("1"_n, {OneBlock});
  assert_tree_equals_blocks(Add("1"_n, "2"_n), {TypeBlock(BlockType::Addition), ValueBlock(2), TypeBlock(BlockType::Addition), OneBlock, TwoBlock});
  assert_tree_equals_blocks(Mult("1"_n, "2"_n, "-1"_n), {TypeBlock(BlockType::Multiplication), ValueBlock(3), TypeBlock(BlockType::Multiplication), OneBlock, TwoBlock, MinusOneBlock});
  assert_tree_equals_blocks(Set("1"_n), {TypeBlock(BlockType::Set), ValueBlock(1), TypeBlock(BlockType::Set), OneBlock});
  assert_tree_equals_blocks(Pow("1"_n, "2"_n), {TypeBlock(BlockType::Power), OneBlock, TwoBlock});
  assert_tree_equals_blocks(Sub("1"_n, "2"_n), {TypeBlock(BlockType::Subtraction), OneBlock, TwoBlock});
  assert_tree_equals_blocks(Sub("1"_n, "2"_n), {TypeBlock(BlockType::Subtraction), OneBlock, TwoBlock});
  assert_tree_equals_blocks("var"_n, {TypeBlock(BlockType::UserSymbol), ValueBlock(3), ValueBlock('v'), ValueBlock('a'), ValueBlock('r'), ValueBlock(3), TypeBlock(BlockType::UserSymbol)});
}
QUIZ_CASE(pcj_constexpr_tree_constructor) { testConstexprTreeConstructor(); }

void testEditionNodeConstructor() {
  assert_node_equals_blocks(EditionReference::Push<BlockType::IntegerPosBig>(static_cast<uint64_t>(1232424242)), {TypeBlock(BlockType::IntegerPosBig), ValueBlock(4), ValueBlock(0x32), ValueBlock(0x4d), ValueBlock(0x75), ValueBlock(0x49), ValueBlock(4), TypeBlock(BlockType::IntegerPosBig)});
  assert_node_equals_blocks(EditionReference::Push<BlockType::IntegerNegBig>(static_cast<uint64_t>(1232424242)), {TypeBlock(BlockType::IntegerNegBig), ValueBlock(4), ValueBlock(0x32), ValueBlock(0x4d), ValueBlock(0x75), ValueBlock(0x49), ValueBlock(4), TypeBlock(BlockType::IntegerNegBig)});
  assert_node_equals_blocks(EditionReference::Push<BlockType::IntegerNegBig>(static_cast<uint64_t>(1232424242)), {TypeBlock(BlockType::IntegerNegBig), ValueBlock(4), ValueBlock(0x32), ValueBlock(0x4d), ValueBlock(0x75), ValueBlock(0x49), ValueBlock(4), TypeBlock(BlockType::IntegerNegBig)});
}
QUIZ_CASE(pcj_edition_node_constructor) { testEditionNodeConstructor(); }

void testNodeIterator() {
  constexpr Tree k_simpleExpression = Mult(Add("1"_n, "2"_n), "3"_n, "4"_n);
  EditionReference mult(k_simpleExpression);
  size_t numberOfChildren = mult.numberOfChildren();
  Node children[] = {Add("1"_n, "2"_n), "3"_n, "4"_n};

  // Scan children forward
  for (const std::pair<Node, int> indexedNode : NodeIterator::Children<Forward, NoEditable>(mult)) {
    assert_trees_are_equal(std::get<Node>(indexedNode), children[std::get<int>(indexedNode)]);
  }

  // Scan children backward
  for (const std::pair<Node, int>indexedNode : NodeIterator::Children<Backward, NoEditable>(mult)) {
    assert_trees_are_equal(std::get<Node>(indexedNode), children[numberOfChildren - 1 - std::get<int>(indexedNode)]);
  }

  // Edit children forward
  Node newChildren[] = {"6"_n, "7"_n, "8"_n};
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

  constexpr Tree k_secondSimpleExpression = Mult(Add("1"_n, "2"_n), "3"_n);
  EditionReference mult2(k_secondSimpleExpression);
  size_t numberOfChildren2 = mult2.numberOfChildren();
  Node children2[] = {Add("1"_n, "2"_n), "3"_n};
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

  Node newChildren1[] = {"10"_n, "11"_n};
  Node newChildren2[] = {"13"_n, "14"_n};
  // Edit two nodes children forward
  for (std::pair<std::array<EditionReference, 2>, int> indexedRefs : MultipleNodesIterator::Children<Forward, Editable, 2>(std::array<EditionReference, 2>({mult, mult2}))) {
    std::array<EditionReference, 2> childrenPair = std::get<std::array<EditionReference, 2>>(indexedRefs);
    int pairIndex = std::get<int>(indexedRefs);
    childrenPair[0].replaceTreeByTree(newChildren1[pairIndex]);
    childrenPair[1].replaceTreeByTree(newChildren2[pairIndex]);
  }
  // Check edition
  Node children1[] = {"10"_n, "11"_n, "6"_n};
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
  Node editedChildren1[] = {"10"_n, "11"_n, "10"_n};
  for (const std::pair<Node, int> indexedNode : NodeIterator::Children<Forward, NoEditable>(mult)) {
    assert_trees_are_equal(std::get<Node>(indexedNode), editedChildren1[std::get<int>(indexedNode)]);
  }
  for (const std::pair<Node, int> indexedNode : NodeIterator::Children<Forward, NoEditable>(mult2)) {
    assert_trees_are_equal(std::get<Node>(indexedNode), newChildren2[numberOfChildren2 - 1 - std::get<int>(indexedNode)]);
  }
}
QUIZ_CASE(pcj_node_iterator) { testNodeIterator(); }

void testNode() {
  CachePool * cachePool = CachePool::sharedCachePool();
  cachePool->reset();
  EditionPool * editionPool = cachePool->editionPool();

  // operator==
  Node node0 = "42"_n;
  Node node1 = EditionReference::Push<BlockType::IntegerShort>(static_cast<int8_t>(42));
  assert(node0 != node1 && *node0.block() == *node1.block());
  Node node2(editionPool->firstBlock());
  assert(node2 == node1);

  // Node navigation
  constexpr Tree e1 = Mult(Add("1"_n, "2"_n), "3"_n, "4"_n);
  constexpr Tree e2 = Pow("5"_n, "6"_n);
  Node n1 = EditionReference(e1);
  Node n2 = EditionReference(e2);
  assert(n1.treeSize() == 14); // TODO: Magic Number
  assert_trees_are_equal(n1.nextNode(), Add("1"_n, "2"_n));
  assert_trees_are_equal(n1.nextTree(), e2);
  assert_trees_are_equal(n2.previousNode(), "4"_n);
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
QUIZ_CASE(pcj_node) { testNode(); }

void testNodeSize() {
  Node node = EditionReference::Push<BlockType::IntegerPosBig>(static_cast<uint64_t>(0x00FF0000));
  assert(node.nodeSize() == 7);
  node = static_cast<Node>(EditionReference::Push<BlockType::IntegerNegBig>(static_cast<uint64_t>(0x0000FF00)));
  assert(node.nodeSize() == 6);
}
QUIZ_CASE(pcj_node_size) { testNodeSize(); }


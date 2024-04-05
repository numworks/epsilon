#include <poincare_junior/src/expression/k_tree.h>
#include <poincare_junior/src/memory/node_iterator.h>

#include "helper.h"

using namespace PoincareJ;

QUIZ_CASE(pcj_edition_pool) {
  EditionPool* pool = SharedEditionPool;
  pool->flush();

  constexpr KTree k_expression = KMult(KAdd(1_e, 2_e), 3_e, 4_e);
  const Tree* handingNode = k_expression;
  Tree* editedNode = pool->clone(handingNode);
  quiz_assert(pool->size() == handingNode->treeSize());
  quiz_assert(pool->numberOfTrees() == 1);
  quiz_assert(pool->isRootBlock(editedNode->block()));
  quiz_assert(!pool->isRootBlock(editedNode->child(1)->block()));

  // References
  quiz_assert(pool->nodeForIdentifier(pool->referenceNode(editedNode)) ==
              editedNode);

  editedNode = pool->clone(handingNode);
  quiz_assert(pool->isRootBlock(editedNode->block()));
  pool->flush();
  quiz_assert(pool->size() == 0);

  pool->push(Type::Zero);
  pool->push(Type::One);
  quiz_assert(*pool->firstBlock() == Type::Zero &&
              *(pool->lastBlock() - 1) == Type::One && pool->size() == 2);
  pool->popBlock();
  quiz_assert(*(pool->lastBlock() - 1) == Type::Zero && pool->size() == 1);
  pool->replaceBlock(pool->firstBlock(), Type::Two);
  quiz_assert(*pool->blockAtIndex(0) == Type::Two);
  pool->insertBlock(pool->firstBlock(), Type::One);
  quiz_assert(*pool->firstBlock() == Type::One && pool->size() == 2);
  pool->insertBlocks(pool->blockAtIndex(2), pool->blockAtIndex(0), 2);
  quiz_assert(*(pool->blockAtIndex(2)) == Type::One &&
              *(pool->blockAtIndex(3)) == Type::Two && pool->size() == 4);
  pool->removeBlocks(pool->firstBlock(), 3);
  quiz_assert(*(pool->firstBlock()) == Type::Two && pool->size() == 1);
  pool->push(Type::Zero);
  pool->push(Type::One);
  pool->push(Type::Half);
  //[ 2 0 1 1/2 ]--> [ 2 1 1/2 0 ]
  pool->moveBlocks(pool->firstBlock() + 1, pool->blockAtIndex(2), 2);
  quiz_assert(*(pool->blockAtIndex(0)) == Type::Two &&
              *(pool->blockAtIndex(1)) == Type::One &&
              *(pool->blockAtIndex(2)) == Type::Half &&
              *(pool->blockAtIndex(3)) == Type::Zero && pool->size() == 4);
  quiz_assert(pool->contains(pool->blockAtIndex(2)));
  quiz_assert(!pool->contains(pool->blockAtIndex(5)));
}

QUIZ_CASE(pcj_edition_reference) {
  SharedEditionPool->flush();

  constexpr KTree k_expr0 = KMult(KAdd(1_e, 2_e), 3_e, 4_e);
  constexpr KTree k_subExpr1 = 6_e;
  constexpr KTree k_expr1 = KPow(KSub(5_e, k_subExpr1), 7_e);

  // Operator ==
  EditionReference ref0;
  EditionReference ref1;
  quiz_assert(ref0 == ref1);
  quiz_assert(ref0.isUninitialized());
  ref0 = EditionReference(k_expr0);
  quiz_assert(!ref0.isUninitialized());
  quiz_assert(ref0 != ref1);
  ref1 = EditionReference(ref0);
  quiz_assert(ref0 == ref1);
  ref1 = EditionReference(k_expr1);
  quiz_assert(ref0 != ref1);

  // Constructors
  ref0->clone();
  EditionReference ref2 = EditionReference(
      SharedEditionPool->push<Type::IntegerShort>(static_cast<int8_t>(8)));
  assert_edition_pool_contains({k_expr0, k_expr1, k_expr0, 8_e});

  // Insertions
  ref2->cloneNodeAfterNode(9_e);
  assert_edition_pool_contains({k_expr0, k_expr1, k_expr0, 8_e, 9_e});
  ref2->cloneNodeAfterNode(10_e);
  assert_edition_pool_contains({k_expr0, k_expr1, k_expr0, 8_e, 10_e, 9_e});
  ref2->moveTreeAfterNode(ref0);
  assert_edition_pool_contains({k_expr1, k_expr0, 8_e, k_expr0, 10_e, 9_e});
  ref2->cloneNodeBeforeNode(10_e);
  assert_edition_pool_contains(
      {k_expr1, k_expr0, 10_e, 8_e, k_expr0, 10_e, 9_e});
  ref2->moveTreeBeforeNode(ref1);
  assert_edition_pool_contains(
      {k_expr0, 10_e, k_expr1, 8_e, k_expr0, 10_e, 9_e});

  // Replacements
  ref0 = ref2;  // 8_e
  assert_trees_are_equal(ref0, 8_e);
  ref1 = ref0->nextTree();  // k_expr0
  ref2 = ref1->nextTree();  // 10_e

  // Replacements by same
  ref2 = ref2->moveTreeOverNode(ref2);
  assert_edition_pool_contains(
      {k_expr0, 10_e, k_expr1, 8_e, k_expr0, 10_e, 9_e});

  // Replacements from nodes outside of the EditionPool
  ref0 = ref0->cloneNodeOverNode(9_e);  // Same size
  assert_edition_pool_contains(
      {k_expr0, 10_e, k_expr1, 9_e, k_expr0, 10_e, 9_e});
  ref1 = ref1->cloneNodeOverTree(10_e);  // Smaller size
  assert_edition_pool_contains({k_expr0, 10_e, k_expr1, 9_e, 10_e, 10_e, 9_e});
  ref2 = ref2->cloneTreeOverNode(k_expr1);  // Bigger size
  assert_edition_pool_contains(
      {k_expr0, 10_e, k_expr1, 9_e, 10_e, k_expr1, 9_e});

  // Replacements from nodes living in the EditionPool
  EditionReference subRef0(ref2->child(0)->child(1));
  EditionReference subRef1(ref2->child(0));
  ref2 = ref2->moveTreeOverTree(subRef0);  // Child
  quiz_assert(subRef1.isUninitialized());
  ref1->moveNodeOverNode(ref0);  // Before
  assert_edition_pool_contains({k_expr0, 10_e, k_expr1, 9_e, k_subExpr1, 9_e});
  ref0->moveTreeOverTree(ref2);  // After
  assert_edition_pool_contains({k_expr0, 10_e, k_expr1, k_subExpr1, 9_e});

  // Removals
  ref0 = EditionReference(SharedEditionPool->firstBlock());  // k_expr0
  ref1 = ref0->nextTree();                                   // 10_e
  ref2 = ref1->nextTree();                                   // k_expr1

  ref2->removeTree();
  ref1->removeNode();
  assert_edition_pool_contains({k_expr0, k_subExpr1, 9_e});

  // Detach
  subRef0 = EditionReference(ref0->child(0));
  subRef0->cloneTreeBeforeNode(13_e);
  subRef0->detachTree();
  assert_edition_pool_contains(
      {KMult(13_e, 3_e, 4_e), k_subExpr1, 9_e, KAdd(1_e, 2_e)});
}

QUIZ_CASE(pcj_edition_reference_reallocation) {
  SharedEditionPool->flush();
  EditionReference reference0(KAdd(1_e, 1_e));
  EditionReference referenceSub0(reference0->child(0));
  EditionReference referenceSub1(reference0->child(1));
  for (size_t i = 0; i < EditionPool::k_maxNumberOfReferences - 5; i++) {
    EditionReference reference1(1_e);
  }
  /* The reference table is now full but we can reference a new node of another
   * one is out-dated. */
  reference0->removeTree();
  quiz_assert(referenceSub0.isUninitialized());
  EditionReference reference2(2_e);
}

QUIZ_CASE(pcj_edition_reference_destructor) {
  SharedEditionPool->flush();
  {
    EditionReference ref0(0_e);
    QUIZ_ASSERT(ref0.identifier() == 0);
    EditionReference ref1(1_e);
    QUIZ_ASSERT(ref1.identifier() == 1);
  }
  {
    // The first ones have been deleted
    EditionReference ref0(2_e);
    QUIZ_ASSERT(ref0.identifier() == 0);
    // Copy
    EditionReference ref1 = ref0;
    QUIZ_ASSERT(ref1.identifier() == 1);
    // Move
    EditionReference stolenRef0 = std::move(ref0);
    QUIZ_ASSERT(stolenRef0.identifier() == 0);
    // ref0 has been invalidated by the move
    QUIZ_ASSERT(static_cast<Tree*>(ref0) == nullptr);
  }
  // Destruction of the references does not destruct trees
  QUIZ_ASSERT(SharedEditionPool->numberOfTrees() == 3);
}

QUIZ_CASE(pcj_tree_comments) {
  EditionReference u, v;
  auto setup = [&]() {
    u = "aaaa"_e->clone();
    "bb"_e->clone();
    v = "ccc"_e->clone();
    "dd"_e->clone();
  };
  setup();
  u->moveNodeBeforeNode(v);
  QUIZ_ASSERT(v->nextNode() == u && u->treeIsIdenticalTo("aaaa"_e) &&
              v->treeIsIdenticalTo("ccc"_e));
  setup();
  u->moveNodeAtNode(v);
  QUIZ_ASSERT(u == v && u->treeIsIdenticalTo("ccc"_e));
  setup();
  u->moveNodeAfterNode(v);
  QUIZ_ASSERT(u->nextNode() == v && u->treeIsIdenticalTo("aaaa"_e) &&
              v->treeIsIdenticalTo("ccc"_e));
  setup();
  u->moveTreeOverNode(v);
  QUIZ_ASSERT(v->nextNode()->treeIsIdenticalTo("bb"_e) && u.isUninitialized() &&
              v->treeIsIdenticalTo("ccc"_e));
  setup();
  u->swapWithTree(v);
  QUIZ_ASSERT(u->treeIsIdenticalTo("aaaa"_e) && v->treeIsIdenticalTo("ccc"_e) &&
              v->nextNode()->treeIsIdenticalTo("bb"_e) &&
              v->nextNode()->nextNode() == u);
}

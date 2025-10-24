#include "advanced_reduction.h"

#include <poincare/src/memory/n_ary.h>
#include <poincare/src/memory/pattern_matching.h>
#include <poincare/src/memory/tree_helpers.h>
#include <poincare/src/memory/tree_stack_checkpoint.h>

#include "k_tree.h"
#include "metric.h"
#include "systematic_reduction.h"

namespace Poincare::Internal {

/* This Debug level can be changed from 0 to 4:
 * - 0 : No assertions
 * - 1 : Free assertions
 * - 2 : Costly assertions run only once per reduction
 * - 3 : Costly assertions
 * - 4 : Very costly assertions */
#define DEBUG_LEVEL 2

#define ASSERT_IF_LEVEL(lvl, cond)      \
  if constexpr (DEBUG_LEVEL >= (lvl)) { \
    assert(cond);                       \
  }

#define VERBOSE_REDUCTION 0

#if !DEBUG
#undef VERBOSE_REDUCTION
#endif

#if VERBOSE_REDUCTION > 0
#define VERBOSE_INDENT(lvl)                   \
  if constexpr (VERBOSE_REDUCTION >= (lvl)) { \
    ++s_indent;                               \
  }
#define VERBOSE_OUTDENT(lvl)                  \
  if constexpr (VERBOSE_REDUCTION >= (lvl)) { \
    --s_indent;                               \
  }

/* LOG_INDENT should not be used directly, it is only there as a way to create
 * the LOG macro with the default paramets indent=true
 * LOG(1, "Test", ()) => print Test with indent
 * LOG(1, "Test", (), false) => print Test without indent */
#define LOG_INDENT(lvl, str, object, indent, default, ...) \
  if constexpr (VERBOSE_REDUCTION >= (lvl)) {              \
    if (indent) LogIndent();                               \
    std::cout << str;                                      \
    object;                                                \
  }

#define LOG(lvl, str, object, ...) \
  LOG_INDENT(lvl, str, object, ##__VA_ARGS__, true, )

size_t s_indent = 0;

void LogIndent() {
  for (size_t i = 0; i < s_indent; i++) {
    std::cout << "  ";
  }
}

void LogExpression(const Tree* e, bool displayDependencies = false) {
  (!displayDependencies && e->isDep() ? e->child(0) : e)->logSerialize();
}
#else
#define VERBOSE_INDENT(_)
#define VERBOSE_OUTDENT(_)
#define LOG(...)

#endif

AdvancedReduction::Path AdvancedReduction::FindBestReduction(
    const Tree* e, ReductionTarget reductionTarget) {
  /* The advanced reduction is capped in depth by Path::k_size and in breadth by
   * CrcCollection::k_size. If this limit is reached, no further possibilities
   * will be explored.
   * This means calling Reduce on an equivalent but different
   * expression could yield different results if limits have been reached. */

  /* TODO: We could do without cloning [e] here, but [applyNextNode] is
   * optimized to be applied on trees at the end of the TreeStack only. */
  Tree* editedExpression = e->cloneTree();
  Context ctx(editedExpression, Metric::GetMetric(e, reductionTarget),
              CrcCollection::AdvancedHash(e), reductionTarget);
  // Add initial root
  ctx.m_crcCollection.add(CrcCollection::AdvancedHash(e), 0);
#if VERBOSE_REDUCTION >= 1
  std::cout << "\nReduce\nInitial tree (" << ctx.m_bestMetric << ") is : ";
  LogExpression(e, true);
  s_indent = 0;
#endif
  PrivateReduce(editedExpression, &ctx);
  editedExpression->removeTree();

#if VERBOSE_REDUCTION >= 1
  ASSERT_IF_LEVEL(1, s_indent == 0);
  std::cout << "Best path metric is: " << ctx.m_bestMetric << "\n";
#endif

  return std::move(ctx.m_bestPath);
}

bool AdvancedReduction::Reduce(Tree* e, ReductionTarget reductionTarget) {
#if POINCARE_NO_ADVANCED_REDUCTION
  uint32_t h = e->hash();
  PrivateDeepExpand(e, ShallowExpandPseudoReduce);
  PrivateDeepContract(e, ShallowContractPseudoReduce);
  return e->hash() != h;
#endif
  /* In the case of a list, advanced reduction works more efficiently when
   * called independently on the list elements. Thus we ensure that there are no
   * "hidden" lists. The List::BubbleUp function fails when the tree has
   * randomized descendants, so this case is excluded. This exception is not
   * critical for the advanced reduction performance. */
  ASSERT_IF_LEVEL(2, e->isList() || !Dimension::IsList(e) ||
                         e->hasDescendantSatisfying(
                             [](const Tree* e) { return e->isRandomized(); }));
  ASSERT_IF_LEVEL(2, !SystematicReduction::DeepReduce(e));

  bool changed = false;
  if (!(e->isList())) {
    changed = ReduceIndependantElement(e, reductionTarget);
  } else {
    for (Tree* child : e->children()) {
      changed = ReduceIndependantElement(child, reductionTarget) || changed;
    }
  }
  ASSERT_IF_LEVEL(2, !changed || !SystematicReduction::DeepReduce(e));
  return changed;
}

bool AdvancedReduction::ReduceIndependantElement(
    Tree* e, ReductionTarget reductionTarget) {
  Path best_path{};
  ExceptionTry { best_path = FindBestReduction(e, reductionTarget); }
  ExceptionCatch(type) {
    if (!(type == ExceptionType::TreeStackOverflow ||
          type == ExceptionType::IntegerOverflow)) {
      TreeStackCheckpoint::Raise(type);
    }
#if VERBOSE_REDUCTION >= 1
    s_indent = 0;
    std::cout << "\nTree stack overflow,  advanced reduction failed.\n";
#endif
    best_path = {};
  }

#if VERBOSE_REDUCTION >= 1
  bool changed;
  Tree* logTree = e->cloneTree();
  Tree* logRoot = logTree;
  std::cout << "Best path is";
  best_path.log();
  std::cout << "Start\t\t";
  LogExpression(e);
  for (int i = 0; i < best_path.length(); ++i) {
    Direction d = best_path.getStack()[i];
    d.apply(&logTree, logRoot, &changed);
    if (!d.isNextNode()) {
      d.log(false);
      std::cout << "  \t";
      LogExpression(logTree);
    }
  }
  LOG(1, "Final tree is : ", LogExpression(logTree, true));
  logTree->removeTree();
  ASSERT_IF_LEVEL(1, s_indent == 0);
#endif
  return best_path.apply(e);
}

bool AdvancedReduction::CrcCollection::add(uint32_t crc, uint8_t depth) {
  if (depth > m_maxDepth) {
    /* Escape if depth is too high, behave as if tree had already been tested
     * (by returning false) to prevent going further. */
    return false;
  }
  // TODO Here we could check if crc is not already in db before decreasing size
  // But impact is probably minimal either way
  if (isFull()) {
    decreaseMaxDepth();
    return !isFull() && add(crc, depth);
  }
  int8_t availableDepth = m_maxDepth - depth;
  // TODO: Maybe use a dichotomic search.
  for (size_t i = 0; i < m_length; i++) {
    uint32_t crc_i = m_collection[i];
    if (crc_i < crc) {
      continue;
    }
    if (crc_i == crc) {
      if (m_depth[i] > depth) {
        // Keep smallest depth
        m_depth[i] = depth;
      }
      if (m_availableDepth[i] >= availableDepth) {
        // Already explored with a bigger available depth.
        return false;
      }
      // There are new nodes to explore with more resources
      m_availableDepth[i] = availableDepth;
      return true;
    }
    // Insert CRC32, availableDepth and depth
    memmove(m_collection + i + 1, m_collection + i,
            sizeof(*m_collection) * (m_length - i));
    memmove(m_availableDepth + i + 1, m_availableDepth + i,
            sizeof(*m_availableDepth) * (m_length - i));
    memmove(m_depth + i + 1, m_depth + i, sizeof(*m_depth) * (m_length - i));
    m_length++;
    m_collection[i] = crc;
    m_availableDepth[i] = availableDepth;
    m_depth[i] = depth;
    return true;
  }
  m_availableDepth[m_length] = availableDepth;
  m_depth[m_length] = depth;
  m_collection[m_length++] = crc;
  return true;
}

void AdvancedReduction::CrcCollection::decreaseMaxDepth() {
#if VERBOSE_REDUCTION >= 4
  LogIndent();
  ASSERT_IF_LEVEL(1, isFull());
  std::cout << "CrcCollection had a " << (int)m_maxDepth
            << " max depth and is full. \n";
#endif
  // Find the smallest available depth in CRC collection
  m_maxDepth = 0;
  size_t firstMaxDepthIndex = 0;
  for (size_t i = 0; i < m_length; i++) {
    uint8_t depth = m_depth[i];
    if (m_maxDepth < depth) {
      m_maxDepth = depth;
      firstMaxDepthIndex = i;
    }
  }
  // Decrement maxDepth
  m_maxDepth--;
  // Remove all CRC explored at maxDepth
  size_t i = firstMaxDepthIndex;
  while (i < m_length) {
    if (m_depth[i] > m_maxDepth) {
      memmove(m_depth + i, m_depth + i + 1,
              (m_length - i - 1) * sizeof(*m_depth));
      memmove(m_availableDepth + i, m_availableDepth + i + 1,
              (m_length - i - 1) * sizeof(*m_availableDepth));
      memmove(m_collection + i, m_collection + i + 1,
              (m_length - i - 1) * sizeof(*m_collection));
      m_length--;
    } else {
      i++;
    }
  }
#if VERBOSE_REDUCTION >= 4
  std::cout << "Remove " << (int)(k_size - m_length)
            << " elements of depth bigger than " << (int)m_maxDepth << ".\n";
#endif
}

const Tree* NextNodeSkippingIgnoredTrees(const Tree* e) {
  ASSERT_IF_LEVEL(1, !AdvancedOperation::CanSkipTree(e));
  const Tree* next = e->nextNode();
  while (next->block() < SharedTreeStack->lastBlock() &&
         AdvancedOperation::CanSkipTree(next)) {
    next = next->nextTree();
  }
  return next;
}

bool AdvancedReduction::Direction::applyNextNode(const Tree** u,
                                                 const Tree* root) const {
  // Optimization: No trees are expected after root, so we can use lastBlock()
  ASSERT_IF_LEVEL(1, isNextNode());
  const Tree* next = NextNodeSkippingIgnoredTrees(*u);

  ASSERT_IF_LEVEL(1, (next->block() < SharedTreeStack->lastBlock()) ==
                         next->hasAncestor(root, false));
  /* TODO We would like this second assert instead of the one above. But we
   * cannot because we apply a path in [ReduceIndependantElement], and there the
   * tree is not guaranteed to be last on TreeStack */
#if 0
  ASSERT_IF_LEVEL(
      1, root->nextTree() == SharedTreeStack->lastBlock() && *u >= root);
#endif

  if (!(next->block() < SharedTreeStack->lastBlock())) {
    return false;
  }
  static_assert(k_baseNextNodeType > 0);
  for (uint8_t i = m_type - 1; i >= k_baseNextNodeType; i--) {
    next = NextNodeSkippingIgnoredTrees(next);
    ASSERT_IF_LEVEL(1, next->block() < SharedTreeStack->lastBlock());
  }
  *u = next;
  return true;
}

bool AdvancedReduction::Direction::applyContractOrExpand(Tree** u,
                                                         Tree* root) const {
  ASSERT_IF_LEVEL(1, isContract() || isExpand());
  ASSERT_IF_LEVEL(1, !AdvancedOperation::CanSkipTree(*u));

  if (!(isContract() ? ShallowContract : ShallowExpand)(*u, false)) {
    return false;
  }
  // Apply a deep systematic reduction starting from (*u)
  UpwardSystematicReduce(root, *u);
  // Move back to root so we only move down trees.
  *u = root;
  return true;
}

bool AdvancedReduction::Direction::apply(Tree** u, Tree* root,
                                         bool* rootChanged) const {
  if (isNextNode()) {
    return applyNextNode(const_cast<const Tree**>(u), root);
  }
  *rootChanged = applyContractOrExpand(u, root);
  return *rootChanged;
}

#if POINCARE_TREE_LOG
void AdvancedReduction::Direction::log(bool addLineReturn) const {
  if (isNextNode()) {
    std::cout << "NextNode";
    if (m_type > 1) {
      std::cout << " * " << std::to_string(m_type);
    }
  } else if (isContract()) {
    std::cout << "Contract";
  } else {
    ASSERT_IF_LEVEL(1, isExpand());
    std::cout << "Expand";
  }
  if (addLineReturn) {
    std::cout << std::endl;
  }
}
#endif

bool AdvancedReduction::Direction::combine(Direction other) {
  if (!isNextNode() || !other.isNextNode() ||
      m_type >= k_expandType - other.m_type) {
    return false;
  }
  m_type += other.m_type;
  return true;
}

bool AdvancedReduction::Direction::decrement() {
  if (!isNextNode() || m_type == k_baseNextNodeType) {
    return false;
  }
  m_type--;
  return true;
}

bool AdvancedReduction::Path::apply(Tree* root) const {
  Tree* e = root;
  bool rootChanged = false;
  for (uint8_t i = 0; i < length(); i++) {
    [[maybe_unused]] bool didApply = m_stack[i].apply(&e, root, &rootChanged);
    ASSERT_IF_LEVEL(1, didApply);
  }
  return rootChanged;
}

void AdvancedReduction::Path::popBaseDirection() {
  ASSERT_IF_LEVEL(1, m_length > 0);
  if (!m_stack[m_length - 1].decrement()) {
    m_length--;
  }
}

void AdvancedReduction::Path::popWholeDirection() {
  ASSERT_IF_LEVEL(1, m_length > 0);
  --m_length;
}

bool AdvancedReduction::Path::append(Direction direction) {
  if (m_length == 0 || !m_stack[m_length - 1].combine(direction)) {
    if (m_length >= k_size) {
      return false;
    }
    m_stack[m_length] = direction;
    m_length += 1;
  }
  return true;
}

#if POINCARE_TREE_LOG
void AdvancedReduction::Path::log() const {
  std::cout << " | ";
  for (uint8_t i = 0; i < m_length; i++) {
    m_stack[i].log(false);
    std::cout << " | ";
  }
  std::cout << std::endl;
}
#endif

void AdvancedReduction::UpdateBestMetric(Context* ctx) {
  float metric = Metric::GetMetric(ctx->m_root, ctx->m_reductionTarget);
  if (metric == Metric::k_perfectMetric) {
    ctx->m_bestMetric = Metric::k_perfectMetric;
    ctx->m_bestPath = ctx->m_path;
    // Skip unnecessary ctx->m_bestHash update
    LOG(2, "Early exit with: ", LogExpression(ctx->m_root));
    return;
  }
#if VERBOSE_REDUCTION >= 1
  const float oldMetric = ctx->m_bestMetric;
  const char* label = "Metric (";
#endif

  /* If metric is the same, compare hash to ensure a same result when starting
   * from two different equivalent expressions. */
  if (metric < ctx->m_bestMetric ||
      (metric == ctx->m_bestMetric &&
       CrcCollection::AdvancedHash(ctx->m_root) > ctx->m_bestHash)) {
    ctx->m_bestMetric = metric;
    ctx->m_bestPath = ctx->m_path;
    ctx->m_bestHash = CrcCollection::AdvancedHash(ctx->m_root);
#if VERBOSE_REDUCTION >= 1
    label = "Improved metric (";
#endif
  }

  VERBOSE_INDENT(1);
  LOG(1, label << metric << " VS " << oldMetric << ")", );
  VERBOSE_OUTDENT(1);
#if VERBOSE_REDUCTION == 1
  std::cout << ": ";
  LogExpression(ctx->m_root);
#endif
#if VERBOSE_REDUCTION == 2
  std::cout << std::endl;
#endif
#if VERBOSE_REDUCTION >= 3
  ctx->m_path.log();
#endif
}

bool AdvancedReduction::PrivateReduce(const Tree* e, Context* ctx,
                                      bool zeroNextNodeAllowed) {
  if (AdvancedOperation::CanSkipTree(e)) {
    return true;
  }
  VERBOSE_INDENT(3);
  bool fullExploration = true;
  if (ctx->m_path.length() + 1 >= ctx->m_crcCollection.maxDepth()) {
    // Skip nextNode direction if we can't add a contract or expand after
    fullExploration = false;
  } else {
    Direction nextNode = Direction::NextNode(1);
    int i = 0;
    const Tree* target = e;
    /* [targets] caches all intermediate node obtained when doing the maximum
     * number of nextNode direction. This is useful because we want to iterate
     * over this list in reverse order. Caching the intermediate trees avoids
     * recomputing the same nextNode direction twice. */
    const Tree* targets[Direction::k_maxNextNodeAmount] = {};
    while (i < Direction::k_maxNextNodeAmount &&
           nextNode.applyNextNode(&target, ctx->m_root)) {
      targets[i++] = target;
    }
    if (i > 0) {
      [[maybe_unused]] bool hasAppendPath =
          ctx->m_path.append(Direction::NextNode(i));
      ASSERT_IF_LEVEL(1, hasAppendPath);
    }
    if (i == Direction::k_maxNextNodeAmount) {
      /* NextNode direction has to be split in two whole directions in the
       * path to handle more than 254 consecutive NextNode */
      ASSERT_IF_LEVEL(1, fullExploration);
      fullExploration = PrivateReduce(target, ctx, false);
      if (ctx->shouldEarlyExit()) {
        VERBOSE_OUTDENT(3);
        return false;
      }
    }
    /* 254 to 1 NextNode handled here */
    ASSERT_IF_LEVEL(1, i <= Direction::k_maxNextNodeAmount);
    for (; i >= 1; --i) {
      // It will be impossible to add C||E after our NextNodes: stop here
      if (!ctx->canAppendDirection()) {
        LOG(1, "CRC ", ctx->m_crcCollection.log());
        ctx->m_path.popWholeDirection();
        fullExploration = false;
        break;
      }
      LOG(3, "Apply ", ctx->m_path.logLastDirection(false));
      LOG(3, ": ", LogExpression(targets[i - 1]), false);
      fullExploration =
          ReduceContractThenExpand(targets[i - 1], ctx) && fullExploration;
      if (ctx->shouldEarlyExit()) {
        VERBOSE_OUTDENT(3);
        return false;
      }
      ctx->m_path.popBaseDirection();
    }
  }
  VERBOSE_OUTDENT(3);

  /* 0 NextNode handle here */
  if (zeroNextNodeAllowed && ctx->canAppendDirection()) {
    fullExploration = ReduceContractThenExpand(e, ctx) && fullExploration;
  }
  return fullExploration;
}

bool inline AdvancedReduction::DuplicateRootAndReduceDirection(
    const Tree* target, Context* ctx, Direction dir) {
  const Tree* oldRoot = ctx->m_root;
  Tree* newRoot = oldRoot->cloneTree();
  ASSERT_IF_LEVEL(1, oldRoot <= target && target < newRoot);
  Tree* newTarget = target - oldRoot + newRoot;
  ctx->m_root = newRoot;
  bool result = ReduceDirection(newTarget, newRoot, ctx, dir);
  ctx->m_root = oldRoot;
  SharedTreeStack->dropBlocksFrom(newRoot);
  return result;
}

bool inline AdvancedReduction::ReduceDirection(Tree* e, Tree* root,
                                               Context* ctx, Direction dir) {
  ASSERT_IF_LEVEL(1, !dir.isNextNode());
  ASSERT_IF_LEVEL(1, ctx->canAppendDirection());
  ASSERT_IF_LEVEL(1, ctx->m_root == root);
  Tree* target = e;
  if (!dir.applyContractOrExpand(&target, root)) {
    LOG(3, "Nothing to ", dir.log());
    return true;
  }
  uint32_t hash = CrcCollection::AdvancedHash(ctx->m_root);
  /* If explored, do not go further. */
  if (!ctx->m_crcCollection.add(hash, ctx->m_path.length() + 1)) {
    if (!ctx->canAppendDirection()) {
      // Not able to add due to decreased maxDepth
      return false;
    }
    if (ctx->m_bestHash == hash &&
        ctx->m_bestPath.length() > ctx->m_path.length()) {
      // If this is the best reduction, with a shorter path, use it
      ASSERT_IF_LEVEL(
          2, ctx->m_bestMetric ==
                 Metric::GetMetric(ctx->m_root, ctx->m_reductionTarget));
      ASSERT_IF_LEVEL(1, ctx->canAppendDirection());
      ctx->m_path.append(dir);
      ctx->m_bestPath = ctx->m_path;
      ctx->m_path.popWholeDirection();
    }
    LOG(3, "Seen before ", dir.log(false));
    LOG(3, ": ", LogExpression(ctx->m_root), false);
    return true;
  }
  /* Otherwise, recursively advance reduce */
  LOG(2, "", dir.log(false));
  LOG(2, ": ", LogExpression(ctx->m_root), false);

  ASSERT_IF_LEVEL(1, ctx->canAppendDirection());
  [[maybe_unused]] bool canAddDir = ctx->m_path.append(dir);
  ASSERT_IF_LEVEL(1, canAddDir);

  // Successfully applied C||E dir and result is unexplored: compute metric
  UpdateBestMetric(ctx);
  if (ctx->shouldEarlyExit()) {
    return false;
  }

  bool fullExploration = PrivateReduce(target, ctx);
  if (ctx->shouldEarlyExit()) {
    return false;
  }
  if (fullExploration) {
    // No need to explore this again, even at smaller lengths.
    ctx->m_crcCollection.add(hash, 0);
  }
  ctx->m_path.popWholeDirection();
  return fullExploration;
}

bool AdvancedReduction::ReduceContractThenExpand(const Tree* e, Context* ctx) {
  VERBOSE_INDENT(2);
  bool fullExploration =
      DuplicateRootAndReduceDirection(e, ctx, Direction::Contract());
  if (ctx->shouldEarlyExit()) {
    VERBOSE_OUTDENT(2);
    return false;
  }
  if (!ctx->canAppendDirection()) {
    LOG(1, "CRC ", ctx->m_crcCollection.log());
    VERBOSE_OUTDENT(2);
    return false;
  }
  fullExploration =
      DuplicateRootAndReduceDirection(e, ctx, Direction::Expand()) &&
      fullExploration;
  VERBOSE_OUTDENT(2);
  return fullExploration;
}

bool AdvancedReduction::UpwardSystematicReduce(Tree* root, const Tree* tree) {
  if (root == tree) {
    ASSERT_IF_LEVEL(4, !SystematicReduction::DeepReduce(root));
    return true;
  }
  ASSERT_IF_LEVEL(1, root < tree);
  for (Tree* child : root->children()) {
    if (UpwardSystematicReduce(child, tree)) {
      SystematicReduction::ShallowReduce(root);
      return true;
    }
  }
  return false;
}

/* Expand/Contract operations */

bool AdvancedReduction::DeepContract(Tree* e) {
  return PrivateDeepContract(e, ShallowContract);
}

bool AdvancedReduction::PrivateDeepContract(
    Tree* e, ShallowApplication shallowContract) {
  if (e->isDepList()) {
    // Never contract anything in dependency's dependencies set.
    return false;
  }

  bool changed = false;
  for (Tree* child : e->children()) {
    changed = PrivateDeepContract(child, shallowContract) || changed;
  }

  /* Contracting the children might break the sorting, e.g.:
   *   exp(3/8*ln(2)*exp(1/8*ln(3))) -> exp(1/8*ln(8))*exp(1/8*ln(3))
   *   sorted: exp(1/8*ln(3))*exp(1/8*ln(8))
   * TODO: This is enough for now, but if need be feel free to upgrade this Sort
   * to a full DeepReduce. */
  if (changed && e->isNAry()) {
    NAry::Sort(e);
  }

  // TODO: Assert !DeepContract(e)
  return shallowContract(e, true) || changed;
}

bool AdvancedReduction::DeepExpand(Tree* e) {
  return PrivateDeepExpand(e, ShallowExpand);
}

bool AdvancedReduction::DeepExpandAlgebraic(Tree* e) {
  return PrivateDeepExpand(e, ShallowExpandAlgebraic);
}

bool AdvancedReduction::PrivateDeepExpand(Tree* e,
                                          ShallowApplication shallowExpand) {
  // Tree::ApplyShallowTopDown could be used but we need to skip dependencies
  bool changed = false;
  /* ShallowExpand may push and remove trees at the end of TreeStack.
   * We use an end marker to keep track of the initial end position of e.  */
  TreeRef endMarker = pushEndMarker(e);
  Tree* target = e;
  while (target->block() < endMarker->block()) {
    while (target->isDepList()) {
      // Never expand anything in dependency's dependencies set.
      target = target->nextTree();
    }
    changed = shallowExpand(target, true) || changed;
    target = target->nextNode();
  }
  removeMarker(endMarker);
  if (changed) {
    // Bottom-up systematic reduce is necessary.
    SystematicReduction::DeepReduce(e);
    // TODO_PCJ: Find a solution so we don't have to run this twice.
    bool temp = PrivateDeepExpand(e, shallowExpand);
    ASSERT_IF_LEVEL(3, !temp || !PrivateDeepExpand(e, shallowExpand));
    (void)temp;
  }
  return changed;
}

bool AdvancedReduction::TryAllOperations(Tree* e,
                                         const Tree::Operation* operations,
                                         int numberOfOperations) {
  /* For example :
   * Most contraction operations are very shallow.
   * exp(A)*exp(B)*exp(C)*|D|*|E| = exp(A+B)*exp(C)*|D|*|E|
   *                              = exp(A+B)*exp(C)*|D*E|
   *                              = exp(A+B+C)*|D*E|
   * Most expansion operations have to handle themselves smartly.
   * exp(A+B+C) = exp(A)*exp(B)*exp(C) */
  int consecutiveFailures = 0;
  int i = 0;
  ASSERT_IF_LEVEL(3, !SystematicReduction::DeepReduce(e));
  while (consecutiveFailures < numberOfOperations) {
    uint32_t hash = e->hash();
    [[maybe_unused]] bool hasAppliedAdvancedOperation =
        operations[i % numberOfOperations](e);
    bool hasChangedExpression = hash != e->hash();
    /* This assertion should help catch useless advanced operations, i.e.
     * operations which are negated by the systematic reduction and thus return
     * an identical expression.
     * It could be relaxed to (!hasChangedExpression ||
     * hasAppliedAdvancedOperation) in case some of those operations cannot be
     * refactored. */
    ASSERT_IF_LEVEL(1, hasAppliedAdvancedOperation == hasChangedExpression);
    consecutiveFailures = hasChangedExpression ? 0 : consecutiveFailures + 1;
    // All operations should preserve e's reduced status
    ASSERT_IF_LEVEL(4, !SystematicReduction::DeepReduce(e));
    i++;
  }
  return i > numberOfOperations;
}

bool AdvancedReduction::TryOneOperation(Tree* e,
                                        const Tree::Operation* operations,
                                        int numberOfOperations) {
  ASSERT_IF_LEVEL(3, !SystematicReduction::DeepReduce(e));
  for (int i = 0; i < numberOfOperations; i++) {
    if (operations[i](e)) {
      ASSERT_IF_LEVEL(4, !SystematicReduction::DeepReduce(e));
      return true;
    }
  }
  return false;
}

/* TODO: sign(A*B) <-> sign(A) * sign(B)
 * It may not work at all with complexes. */

}  // namespace Poincare::Internal

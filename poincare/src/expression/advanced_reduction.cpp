#include "advanced_reduction.h"

#include <poincare/src/memory/pattern_matching.h>
#include <poincare/src/memory/tree_helpers.h>
#include <poincare/src/memory/tree_stack_checkpoint.h>

#include "k_tree.h"
#include "metric.h"
#include "systematic_reduction.h"

namespace Poincare::Internal {

#define LOG_NEW_ADVANCED_REDUCTION_VERBOSE 0

#if LOG_NEW_ADVANCED_REDUCTION_VERBOSE > 0
bool s_logIndividualPathStep = false;
size_t s_indent = 0;

void LogIndent() {
  for (size_t i = 0; i < s_indent; i++) {
    std::cout << "  ";
  }
}

void LogExpression(const Tree* e, bool displayDependencies = false) {
  (!displayDependencies && e->isDep() ? e->child(0) : e)->logSerialize();
}

#endif

AdvancedReduction::Path AdvancedReduction::FindBestReduction(const Tree* e) {
  /* The advanced reduction is capped in depth by Path::k_size and in breadth by
   * CrcCollection::k_size. If this limit is reached, no further possibilities
   * will be explored.
   * This means calling Reduce on an equivalent but different
   * expression could yield different results if limits have been reached. */

  Tree* editedExpression = e->cloneTree();
  Context ctx(editedExpression, e, Metric::GetMetric(e),
              CrcCollection::AdvancedHash(e));
  // Add initial root
  ctx.m_crcCollection.add(CrcCollection::AdvancedHash(e), 0);
#if LOG_NEW_ADVANCED_REDUCTION_VERBOSE >= 1
  std::cout << "\nReduce\nInitial tree (" << ctx.m_bestMetric << ") is : ";
  LogExpression(e, true);
  s_indent = 1;
#endif
  ReduceRec(editedExpression, &ctx);
  editedExpression->removeTree();

#if LOG_NEW_ADVANCED_REDUCTION_VERBOSE >= 1
  s_indent = 0;
  std::cout << "Best path metric is: " << ctx.m_bestMetric << "\n";
#endif

  return std::move(ctx.m_bestPath);
}

bool AdvancedReduction::Reduce(Tree* e) {
#if POINCARE_NO_ADVANCED_REDUCTION
  return false;
#endif
  /* In the case of a list, advanced reduction works more efficiently when
   * called independently on the list elements. Thus we ensure that there are no
   * "hidden" lists. The List::BubbleUp function fails when the tree has
   * randomized descendants, so this case is excluded. This exception is not
   * critical for the advanced reduction performance. */
  assert(e->isList() || !Dimension::IsList(e) ||
         e->hasDescendantSatisfying(
             [](const Tree* e) { return e->isRandomized(); }));

  if (!(e->isList())) {
    return ReduceIndependantElement(e);
  }
  bool changed = false;
  for (Tree* child : e->children()) {
    changed = ReduceIndependantElement(child) || changed;
  }
  return changed;
}

bool AdvancedReduction::ReduceIndependantElement(Tree* e) {
  Path best_path{};
  ExceptionTry { best_path = FindBestReduction(e); }
  ExceptionCatch(type) {
    if (!(type == ExceptionType::TreeStackOverflow ||
          type == ExceptionType::IntegerOverflow)) {
      TreeStackCheckpoint::Raise(type);
    }
#if LOG_NEW_ADVANCED_REDUCTION_VERBOSE >= 1
    s_indent = 0;
    std::cout << "\nTree stack overflow,  advanced reduction failed.\n";
#endif
    best_path = {};
  }

#if LOG_NEW_ADVANCED_REDUCTION_VERBOSE >= 1
  s_logIndividualPathStep = true;
#endif
  bool result = best_path.apply(e);
#if LOG_NEW_ADVANCED_REDUCTION_VERBOSE >= 1
  s_logIndividualPathStep = false;
  s_indent = 0;
  std::cout << "Final tree is : ";
  LogExpression(e, true);
#endif
  return result;
}

bool AdvancedReduction::CrcCollection::add(uint32_t crc, uint8_t depth) {
  if (depth > m_maxDepth) {
    /* Escape if depth is too high, behave as if tree had already been tested
     * (by returning false) to prevent going further. */
    return false;
  }
  // Hugo: Here we should check if the crc is not already in db before
  // decreasing size ?
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
#if LOG_NEW_ADVANCED_REDUCTION_VERBOSE >= 1
  LogIndent();
  assert(isFull());
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
#if LOG_NEW_ADVANCED_REDUCTION_VERBOSE >= 4
  std::cout << "Remove " << (int)(k_size - m_length)
            << " elements of depth bigger than " << (int)m_maxDepth << ".\n";
#endif
}

bool SkipTree(const Tree* e) {
  return e->block() < SharedTreeStack->lastBlock() && e->isDepList();
}

Tree* NextNode(Tree* e) {
  assert(!SkipTree(e));
  Tree* next = e->nextNode();
  while (SkipTree(next)) {
    next = next->nextTree();
  }
  return next;
}

const Tree* NextNode(const Tree* e) {
  assert(!SkipTree(e));
  const Tree* next = e->nextNode();
  while (SkipTree(next)) {
    next = next->nextTree();
  }
  return next;
}

bool AdvancedReduction::Direction::applyNextNode(Tree** u,
                                                 const Tree* root) const {
  // Optimization: No trees are expected after root, so we can use lastBlock()
  assert(isNextNode());
  assert(m_type >= k_baseNextNodeType);
  // TODO this assert only test application for m_type == 1
  // not for any m_type > 1
  assert((NextNode(*u)->block() < SharedTreeStack->lastBlock()) ==
         NextNode(*u)->hasAncestor(root, false));
  if (!(NextNode(*u)->block() < SharedTreeStack->lastBlock())) {
    return false;
  }
  for (uint8_t i = m_type; i >= k_baseNextNodeType; i--) {
    *u = NextNode(*u);
  }
  return true;
}

bool AdvancedReduction::Direction::applyContractOrExpand(Tree** u,
                                                         Tree* root) const {
  assert(isContract() || isExpand());
  if ((*u)->numberOfChildren() == 0) {
    // TODO move this test in canApply ?
    // Trees without children cannot be contracted or expanded.
    return false;
  }
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
    return applyNextNode(u, root);
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
    assert(isExpand());
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
#if LOG_NEW_ADVANCED_REDUCTION_VERBOSE > 0
  if (s_logIndividualPathStep) {
    std::cout << "Best path is: ";
    LogExpression(e);
  }
#endif
  for (uint8_t i = 0; i < length(); i++) {
    [[maybe_unused]] bool didApply = m_stack[i].apply(&e, root, &rootChanged);
    assert(didApply);
#if LOG_NEW_ADVANCED_REDUCTION_VERBOSE > 0
    if (s_logIndividualPathStep && !m_stack[i].isNextNode()) {
      m_stack[i].log(false);
      std::cout << ": ";
      LogExpression(e);
    }
#endif
  }
  return rootChanged;
}

void AdvancedReduction::Path::popBaseDirection() {
  assert(m_length > 0);
  if (!m_stack[m_length - 1].decrement()) {
    m_length--;
  }
}
void AdvancedReduction::Path::popWholeDirection() {
  assert(m_length > 0);
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

void inline AdvancedReduction::ResetRootIfNeeded(Context* ctx) {
  if (ctx->m_mustResetRoot) {
    // Reset root to current path
    ctx->m_root->cloneTreeOverTree(ctx->m_original);
    ctx->m_path.apply(ctx->m_root);
    ctx->m_mustResetRoot = false;
  }
}

void AdvancedReduction::UpdateBestMetric(Context* ctx) {
  // Otherwise, root should be reset to current path.
  assert(!ctx->m_mustResetRoot);
  int metric = Metric::GetMetric(ctx->m_root);
#if LOG_NEW_ADVANCED_REDUCTION_VERBOSE >= 1
  const int oldMetric = ctx->m_bestMetric;
  const char* label = "Metric (";
#endif

  // If metric is the same, compare hash to ensure a deterministic result.
  if (metric < ctx->m_bestMetric ||
      (metric == ctx->m_bestMetric &&
       CrcCollection::AdvancedHash(ctx->m_root) > ctx->m_bestHash)) {
    ctx->m_bestMetric = metric;
    ctx->m_bestPath = ctx->m_path;
    ctx->m_bestHash = CrcCollection::AdvancedHash(ctx->m_root);
#if LOG_NEW_ADVANCED_REDUCTION_VERBOSE >= 1
    label = "Improved metric (";
#endif
  }

#if LOG_NEW_ADVANCED_REDUCTION_VERBOSE >= 1
  LogIndent();
  std::cout << label << metric << " VS " << oldMetric << ")";
#if LOG_NEW_ADVANCED_REDUCTION_VERBOSE == 1
  std::cout << ": ";
  LogExpression(ctx->m_root);
#endif
#if LOG_NEW_ADVANCED_REDUCTION_VERBOSE == 2
  std::cout << std::endl;
#endif
#if LOG_NEW_ADVANCED_REDUCTION_VERBOSE >= 3
  ctx->m_path.log();
#endif
#endif
}

bool AdvancedReduction::ReduceRec(Tree* e, Context* ctx,
                                  bool zeroNextNodeAllowed) {
  bool fullExploration = true;
  // Hugo: maybe we could make the Direction constructor public ?
  Direction nextNode = Direction::SingleDirectionForIndex(0);
  uint8_t i = 0;
  Tree* target = e;
  Tree* targets[UINT8_MAX - 1] = {};
  // Checking if we can add 2 direction to path (NN + C||E)
  if (ctx->m_path.length() + 1 >= ctx->m_crcCollection.maxDepth()) {
    fullExploration = false;
    goto noNextNode;
  }
  /* 254 to 1 NextNode handled here */
  while (i < UINT8_MAX - 1 && nextNode.applyNextNode(&target, ctx->m_root)) {
    targets[i++] = target;
    // TODO: could be a single append outside the while
    // requires public Direction constructor
    [[maybe_unused]] bool hasAppendPath = ctx->m_path.append(nextNode);
    assert(hasAppendPath);
  }
  if (i == UINT8_MAX - 1) {
    /* More than 254 NextNode handle here */
    fullExploration = ReduceRec(target, ctx, false) && fullExploration;
  } else if (i == 0) {
    goto noNextNode;
  }
  assert(i > 0 && i < UINT8_MAX);
  --i;
  for (; i != UINT8_MAX; --i) {
    ResetRootIfNeeded(ctx);
    assert(ctx->m_path.length() < ctx->m_crcCollection.maxDepth());
#if LOG_NEW_ADVANCED_REDUCTION_VERBOSE >= 2
#if LOG_NEW_ADVANCED_REDUCTION_VERBOSE >= 3
    LogIndent();
    std::cout << "Apply ";
    ctx->m_path.logBaseDir(true);
#endif
    ++s_indent;
#endif
    // fullExploration = NewReduceCE(*nextNodeTarget, ctx) && fullExploration;
    fullExploration = ReduceCE(targets[i], ctx) && fullExploration;
#if LOG_NEW_ADVANCED_REDUCTION_VERBOSE >= 2
    --s_indent;
#endif
    // It will be impossible to add C||E after our NextNodes: stop here
    if (!ctx->canAddDirToPath()) {
#if LOG_NEW_ADVANCED_REDUCTION_VERBOSE >= 1
      LogIndent();
      std::cout << "CRC maxDepth reduced. ";
      ctx->m_crcCollection.log();
#endif
      ctx->m_path.popWholeDirection();
      // not sure this is required to be set at false
      fullExploration = false;
      break;
    }
    ctx->m_path.popBaseDirection();
  }

noNextNode:
  /* 0 NextNode handle here */
  if (zeroNextNodeAllowed && ctx->canAddDirToPath()) {
    fullExploration = ReduceCE(e, ctx) && fullExploration;
  }
  return fullExploration;
}

bool AdvancedReduction::ReduceCE(Tree* e, Context* ctx) {
#if LOG_NEW_ADVANCED_REDUCTION_VERBOSE == 2
  bool printPreviousNN = true;
#endif
  bool fullExploration = true;
  for (uint8_t i = 1; i < Direction::k_numberOfBaseDirections; i++) {
    assert(ctx->m_path.length() < ctx->m_crcCollection.maxDepth());
    ResetRootIfNeeded(ctx);
    // Hugo: maybe we should make the Direction constructer public ?
    Direction dir = Direction::SingleDirectionForIndex(i);
    Tree* target = e;
    // Hugo: We could avoid doing this apply if we somehow know that the
    // resulting path is outisde scope (length > maxDepth). But do we want to ?
    if (!dir.applyContractOrExpand(&target, ctx->m_root)) {
#if LOG_NEW_ADVANCED_REDUCTION_VERBOSE >= 3
      LogIndent();
      std::cout << "Nothing to ";
      dir.log();
#endif
      continue;
    }
    // Hugo: If m_root is last on treestack we could avoid using treeSize in
    // hash
    uint32_t hash = CrcCollection::AdvancedHash(ctx->m_root);
    /* If explored, do not go further. */
    // Hugo: it's slower(why?) with this +1 but it's more correct right ?
    if (!ctx->m_crcCollection.add(hash, ctx->m_path.length() + 1)) {
      ctx->m_mustResetRoot = true;
      if (!ctx->canAddDirToPath()) {
        // Not able to add due to decreased maxDepth (probably?)
        fullExploration = false;
        break;
      }
#if LOG_NEW_ADVANCED_REDUCTION_VERBOSE >= 3
      LogIndent();
      std::cout << "Already applied ";
      dir.log(false);
      std::cout << ": ";
      LogExpression(ctx->m_root);
#endif
      continue;
    }
    /* Otherwise, recursively advanced reduce */
#if LOG_NEW_ADVANCED_REDUCTION_VERBOSE >= 2
#if LOG_NEW_ADVANCED_REDUCTION_VERBOSE == 2
    if (printPreviousNN && ctx->m_path.length() > 0) {
      --s_indent;
      LogIndent();
      ++s_indent;
      std::cout << "Apply ";
      ctx->m_path.logBaseDir(true);
      printPreviousNN = false;
    }
#endif
    LogIndent();
    std::cout << "Apply ";
    dir.log(false);
    std::cout << ": ";
    LogExpression(ctx->m_root);
    s_indent++;
#endif

    assert(ctx->m_path.length() < ctx->m_crcCollection.maxDepth());
    [[maybe_unused]] bool canAddDir = ctx->m_path.append(dir);
    assert(canAddDir);

    // Successfully applied C||E dir and result is unexplored: compute metric
    UpdateBestMetric(ctx);

    if (ReduceRec(target, ctx)) {
      // No need to explore this again, even at smaller lengths.
      ctx->m_crcCollection.add(hash, 0);
    } else {
      fullExploration = false;
    }
    ctx->m_path.popWholeDirection();
    ctx->m_mustResetRoot = true;
#if LOG_NEW_ADVANCED_REDUCTION_VERBOSE >= 2
    assert(s_indent > 0);
    s_indent--;
#endif
    if (i == 1 && !ctx->canAddDirToPath()) {
#if LOG_NEW_ADVANCED_REDUCTION_VERBOSE >= 1
      LogIndent();
      std::cout << "CRC maxDepth reduced. ";
      ctx->m_crcCollection.log();
      std::cout << "\n";
#endif
      break;
    }
  }
  return fullExploration;
}

bool AdvancedReduction::UpwardSystematicReduce(Tree* root, const Tree* tree) {
  if (root == tree) {
    assert(!SystematicReduction::DeepReduce(root));
    return true;
  }
  assert(root < tree);
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
  if (e->isDepList()) {
    // Never contract anything in dependency's dependencies set.
    return false;
  }
  bool changed = false;
  for (Tree* child : e->children()) {
    changed = DeepContract(child) || changed;
  }
  // TODO: Assert !DeepContract(e)
  return ShallowContract(e, true) || changed;
}

bool AdvancedReduction::PrivateDeepExpand(Tree* e,
                                          bool onlyAlgebraicOperations) {
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
    changed = (onlyAlgebraicOperations ? ShallowExpandAlgebraic
                                       : ShallowExpand)(target, true) ||
              changed;
    target = target->nextNode();
  }
  removeMarker(endMarker);
  if (changed) {
    // Bottom-up systematic reduce is necessary.
    SystematicReduction::DeepReduce(e);
    // TODO_PCJ: Find a solution so we don't have to run this twice.
    bool temp = PrivateDeepExpand(e, onlyAlgebraicOperations);
    assert(!temp || !PrivateDeepExpand(e, onlyAlgebraicOperations));
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
  assert(!SystematicReduction::DeepReduce(e));
  while (consecutiveFailures < numberOfOperations) {
#if ASSERTIONS
    uint32_t hash = e->hash();
#endif
    bool changed = operations[i % numberOfOperations](e);
    assert(changed || hash == e->hash());
    consecutiveFailures = changed ? 0 : consecutiveFailures + 1;
    // EveryOperation should preserve e's reduced status
    assert(!SystematicReduction::DeepReduce(e));
    i++;
  }
  return i > numberOfOperations;
}

bool AdvancedReduction::TryOneOperation(Tree* e,
                                        const Tree::Operation* operations,
                                        int numberOfOperations) {
  assert(!SystematicReduction::DeepReduce(e));
  for (int i = 0; i < numberOfOperations; i++) {
    if (operations[i](e)) {
      assert(!SystematicReduction::DeepReduce(e));
      return true;
    }
  }
  return false;
}

/* TODO: sign(A*B) <-> sign(A) * sign(B)
 * It may not work at all with complexes. */

}  // namespace Poincare::Internal

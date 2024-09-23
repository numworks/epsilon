#include "advanced_reduction.h"

#include <poincare/src/memory/pattern_matching.h>
#include <poincare/src/memory/tree_stack_checkpoint.h>

#include "k_tree.h"
#include "metric.h"
#include "systematic_reduction.h"

namespace Poincare::Internal {

#define LOG_NEW_ADVANCED_REDUCTION_VERBOSE 0

#if LOG_NEW_ADVANCED_REDUCTION_VERBOSE > 0
size_t s_indent = 0;

void LogIndent() {
  for (size_t i = 0; i < s_indent; i++) {
    std::cout << "  ";
  }
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
  e->logSerialize();
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
  if (!(e->isList())) {
    return ReduceIndependantElement(e);
  }
  bool changed = false;
  for (std::size_t i = 0; i < e->numberOfChildren(); i++) {
    changed = ReduceIndependantElement(e->child(i)) || changed;
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

  bool result = best_path.apply(e);
#if LOG_NEW_ADVANCED_REDUCTION_VERBOSE >= 1
  s_indent = 0;
  std::cout << "Final tree is : ";
  e->logSerialize();
#endif
  return result;
}

bool AdvancedReduction::CrcCollection::add(uint32_t crc, uint8_t depth) {
  if (depth > m_maxDepth) {
    /* Escape if depth is too high, behave as if tree had already been tested
     * (by returning false) to prevent going further. */
    return false;
  }
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
#if LOG_NEW_ADVANCED_REDUCTION_VERBOSE >= 4
  LogIndent();
  assert(isFull());
  std::cout << "CrcCollection had a " << (int)m_maxDepth
            << " max depth and is full. ";
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

bool AdvancedReduction::Direction::canApply(const Tree* e,
                                            const Tree* root) const {
  // Optimization: No trees are expected after root, so we can use lastBlock()
  assert(!isNextNode() ||
         (NextNode(e)->block() < SharedTreeStack->lastBlock()) ==
             NextNode(e)->hasAncestor(root, false));
  return !isNextNode() || NextNode(e)->block() < SharedTreeStack->lastBlock();
}

bool AdvancedReduction::Direction::apply(Tree** u, Tree* root,
                                         bool* rootChanged) const {
  if (isNextNode()) {
    assert(m_type >= k_baseNextNodeType);
    for (uint8_t i = m_type; i >= k_baseNextNodeType; i--) {
      *u = NextNode(*u);
    }
    return true;
  }
  assert(isContract() || isExpand());
  if ((*u)->numberOfChildren() == 0) {
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
  *rootChanged = true;
  return true;
}

#if POINCARE_TREE_LOG
void AdvancedReduction::Direction::log() const {
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
    bool didApply = m_stack[i].apply(&e, root, &rootChanged);
    (void)didApply;
    assert(didApply);
  }
  return rootChanged;
}

void AdvancedReduction::Path::popBaseDirection() {
  assert(m_length > 0);
  if (!m_stack[m_length - 1].decrement()) {
    m_length--;
  }
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
    m_stack[i].log();
    std::cout << " | ";
  }
}
#endif

bool AdvancedReduction::ReduceRec(Tree* e, Context* ctx) {
  bool fullExploration = true;
#if LOG_NEW_ADVANCED_REDUCTION_VERBOSE >= 4
  LogIndent();
  std::cout << "ReduceRec on subtree: ";
  e->logSerialize();
#endif
  if (!ctx->m_path.canAddNewDirection()) {
    fullExploration = false;
#if LOG_NEW_ADVANCED_REDUCTION_VERBOSE >= 1
    LogIndent();
    std::cout << "Full path.\n";
#endif
  } else {
    bool isLeaf = true;
    for (uint8_t i = 0; i < Direction::k_numberOfBaseDirections; i++) {
      if (ctx->m_crcCollection.maxDepth() < ctx->m_path.length()) {
        fullExploration = false;
#if LOG_NEW_ADVANCED_REDUCTION_VERBOSE >= 1
        LogIndent();
        std::cout << "CRC maxDepth has been reduced.\n";
#endif
        break;
      }
      Direction dir = Direction::SingleDirectionForIndex(i);
      if (ctx->m_mustResetRoot) {
        // Reset root to current path
        ctx->m_root->cloneTreeOverTree(ctx->m_original);
        ctx->m_path.apply(ctx->m_root);
        ctx->m_mustResetRoot = false;
      }
      Tree* target = e;
      bool rootChanged = false;
      if (!dir.canApply(target, ctx->m_root)) {
#if LOG_NEW_ADVANCED_REDUCTION_VERBOSE >= 3
        LogIndent();
        std::cout << "Can't apply ";
        dir.log();
        std::cout << ".\n";
#endif
        continue;
      }
      if (!dir.apply(&target, ctx->m_root, &rootChanged)) {
#if LOG_NEW_ADVANCED_REDUCTION_VERBOSE >= 3
        LogIndent();
        std::cout << "Nothing to ";
        dir.log();
        std::cout << ".\n";
#endif
        continue;
      }
      uint32_t hash;
      if (rootChanged) {
        // No need to recompute hash if root did not change.
        hash = CrcCollection::AdvancedHash(ctx->m_root);
      }
      /* If unchanged or unexplored, recursively advanced reduce. Otherwise,
       * do not go further. */
      if (!rootChanged ||
          ctx->m_crcCollection.add(hash, ctx->m_path.length())) {
#if LOG_NEW_ADVANCED_REDUCTION_VERBOSE >= 2
#if LOG_NEW_ADVANCED_REDUCTION_VERBOSE >= 3
        bool shouldLog = true;
#else
        bool shouldLog = !dir.isNextNode();
#endif
        if (shouldLog) {
          LogIndent();
          std::cout << "Apply ";
          dir.log();
          std::cout << ": ";
          if (rootChanged) {
            ctx->m_root->logSerialize();
          } else {
            std::cout << "\n";
          }
          s_indent++;
        }
#endif
        isLeaf = false;
        assert(ctx->m_crcCollection.maxDepth() >= ctx->m_path.length());
        bool canAddDir = ctx->m_path.append(dir);

        assert(canAddDir);
        (void)canAddDir;
        if (!ReduceRec(target, ctx)) {
          fullExploration = false;
        } else if (rootChanged) {
          // No need to explore this again, even at smaller lengths.
          ctx->m_crcCollection.add(hash, 0);
        }
        ctx->m_path.popBaseDirection();
#if LOG_NEW_ADVANCED_REDUCTION_VERBOSE >= 2
        if (shouldLog) {
          assert(s_indent > 0);
          s_indent--;
        }
#endif
      }
#if LOG_NEW_ADVANCED_REDUCTION_VERBOSE >= 1
      else if (ctx->m_crcCollection.isFull()) {
        LogIndent();
        std::cout << "Full CRC collection.\n";
      }
#endif
#if LOG_NEW_ADVANCED_REDUCTION_VERBOSE >= 3
      else {
        LogIndent();
        std::cout << "Already applied ";
        dir.log();
        std::cout << ": ";
        ctx->m_root->logSerialize();
      }
#endif
      if (rootChanged) {
        // root will be reset to current path if needed later.
        ctx->m_mustResetRoot = true;
      }
    }
    if (!isLeaf) {
      return fullExploration;
    }
  }
  // Otherwise, root should be reset to current path.
  assert(!ctx->m_mustResetRoot);
  // All directions are impossible, we are at a leaf. Compare metrics.
  int metric = Metric::GetMetric(ctx->m_root);

#if LOG_NEW_ADVANCED_REDUCTION_VERBOSE >= 1
  LogIndent();
  std::cout << "Leaf reached (" << metric << " VS " << ctx->m_bestMetric << ")";
#if LOG_NEW_ADVANCED_REDUCTION_VERBOSE <= 1
  std::cout << ": ";
  ctx->m_root->logSerialize();
#endif
#if LOG_NEW_ADVANCED_REDUCTION_VERBOSE >= 3
  LogIndent();
  std::cout << "Path:";
  ctx->m_path.log();
  std::cout << "\n";
#endif
#endif
  // If metric is the same, compare hash to ensure a deterministic result.
  if (metric < ctx->m_bestMetric ||
      (metric == ctx->m_bestMetric &&
       CrcCollection::AdvancedHash(ctx->m_root) > ctx->m_bestHash)) {
    ctx->m_bestMetric = metric;
    ctx->m_bestPath = ctx->m_path;
    ctx->m_bestHash = CrcCollection::AdvancedHash(ctx->m_root);
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

bool AdvancedReduction::DeepExpand(Tree* e) {
  // Tree::ApplyShallowTopDown could be used but we need to skip sets
  bool changed = false;
  /* ShallowExpand may push and remove trees at the end of TreeStack.
   * We push a temporary tree to preserve TreeRef.
   * TODO: Maybe find a solution for this unintuitive workaround, the same
   * hack is used in Projection::DeepReplaceUserNamed. */
  TreeRef nextTree = e->nextTree()->cloneTreeBeforeNode(0_e);
  Tree* target = e;
  while (target->block() < nextTree->block()) {
    while (target->isDepList()) {
      // Never expand anything in dependency's dependencies set.
      target = target->nextTree();
    }
    changed = ShallowExpand(target, true) || changed;
    target = target->nextNode();
  }
  nextTree->removeTree();
  if (changed) {
    // Bottom-up systematic reduce is necessary.
    SystematicReduction::DeepReduce(e);
    // TODO_PCJ: Find a solution so we don't have to run this twice.
    bool temp = DeepExpand(e);
    assert(!temp || !DeepExpand(e));
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
  int failures = 0;
  int i = 0;
  assert(!SystematicReduction::DeepReduce(e));
  while (failures < numberOfOperations) {
    failures = operations[i % numberOfOperations](e) ? 0 : failures + 1;
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

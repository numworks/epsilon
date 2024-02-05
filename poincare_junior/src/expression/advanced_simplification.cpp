#include "advanced_simplification.h"

#include <ion.h>
#include <poincare_junior/src/expression/dependency.h>
#include <poincare_junior/src/expression/metric.h>
#include <poincare_junior/src/expression/simplification.h>

namespace PoincareJ {

#define LOG_NEW_ADVANCED_REDUCTION_VERBOSE 0

#if LOG_NEW_ADVANCED_REDUCTION_VERBOSE > 0
size_t s_indent = 0;

void LogIndent() {
  for (size_t i = 0; i < s_indent; i++) {
    std::cout << "  ";
  }
}

#endif

bool AdvancedSimplification::AdvancedReduce(Tree* origin) {
  /* The advanced reduction is capped in depth by Path::k_size and in breadth by
   * CrcCollection::k_size. If this limit is reached, no further possibilities
   * will be explored.
   * This means calling AdvancedReduce on an equivalent but different
   * expression could yield different results if limits have been reached. */
  Tree* u = origin->isDependency() ? origin->child(0) : origin;
  Tree* editedExpression = u->clone();
  Context ctx(editedExpression, u, Metric::GetMetric(u));
  // Add initial root
  ctx.m_crcCollection.add(CrcCollection::Hash(u), 0);
#if LOG_NEW_ADVANCED_REDUCTION_VERBOSE >= 1
  std::cout << "\nAdvancedReduce\nInitial tree (" << ctx.m_bestMetric
            << ") is : ";
  u->logSerialize();
  s_indent = 1;
#endif
  AdvancedReduceRec(editedExpression, &ctx);
  editedExpression->removeTree();
  bool result = ApplyPath(u, &ctx.m_bestPath, true);
#if LOG_NEW_ADVANCED_REDUCTION_VERBOSE >= 1
  s_indent = 0;
  std::cout << "Final result (" << ctx.m_bestMetric << ") is : ";
  u->logSerialize();
#endif
  if (origin->isDependency()) {
    // Bubble-up any other dependency that appeared.
    Dependency::ShallowBubbleUpDependencies(origin);
  }
  return result;
}

bool AdvancedSimplification::CrcCollection::add(uint32_t crc, uint8_t depth) {
  if (isFull()) {
    // Behave as if all trees had already been tested.
    return false;
  }
  // TODO: Maybe use a dichotomic search.
  for (size_t i = 0; i < m_length; i++) {
    uint32_t crc_i = m_collection[i];
    if (crc_i < crc) {
      continue;
    }
    if (crc_i == crc) {
      if (m_depth[i] <= depth) {
        return false;
      }
      // There might be new nodes to explore if more resources are available.
      m_depth[i] = depth;
      return true;
    }
    // Insert CRC32 and depth
    memmove(m_collection + i + 1, m_collection + i,
            sizeof(uint32_t) * (m_length - i));
    memmove(m_depth + i + 1, m_depth + i, sizeof(uint8_t) * (m_length - i));
    m_length++;
    m_collection[i] = crc;
    m_depth[i] = depth;
    return true;
  }
  m_depth[m_length] = depth;
  m_collection[m_length++] = crc;
  return true;
}

uint32_t AdvancedSimplification::CrcCollection::Hash(const Tree* u) {
  return Ion::crc32Byte(reinterpret_cast<const uint8_t*>(u), u->treeSize());
}

#if POINCARE_MEMORY_TREE_LOG
void AdvancedSimplification::Direction::log() {
  if (isNextNode()) {
    std::cout << "NextNode";
    if (m_type > 1) {
      std::cout << " * " << m_type;
    }
  } else if (isContract()) {
    std::cout << "Contract";
  } else {
    assert(isExpand());
    std::cout << "Expand";
  }
}
#endif

bool AdvancedSimplification::Direction::combine(Direction other) {
  if (!isNextNode() || !other.isNextNode() ||
      m_type >= k_expandType - other.m_type) {
    return false;
  }
  m_type += other.m_type;
  return true;
}

bool AdvancedSimplification::Direction::decrement() {
  if (!isNextNode() || m_type == k_baseNextNodeType) {
    return false;
  }
  m_type--;
  return true;
}

void AdvancedSimplification::Path::popBaseDirection() {
  assert(m_length > 0);
  if (!m_stack[m_length - 1].decrement()) {
    m_length--;
  }
}

bool AdvancedSimplification::Path::append(Direction direction) {
  if (m_length == 0 || !m_stack[m_length - 1].combine(direction)) {
    if (m_length >= k_size) {
      return false;
    }
    m_stack[m_length] = direction;
    m_length += 1;
  }
  return true;
}

bool AdvancedSimplification::CanApplyDirection(const Tree* u, const Tree* root,
                                               Direction direction) {
  // Optimization: No trees are expected after root, so we can use lastBlock()
  assert(!direction.isNextNode() ||
         (u->nextNode()->block() < SharedEditionPool->lastBlock()) ==
             u->nextNode()->hasAncestor(root, false));
  return !direction.isNextNode() ||
         u->nextNode()->block() < SharedEditionPool->lastBlock();
}

bool AdvancedSimplification::ApplyDirection(Tree** u, Tree* root,
                                            Direction direction,
                                            bool* rootChanged,
                                            bool keepDependencies) {
  if (direction.isNextNode()) {
    do {
      *u = (*u)->nextNode();
    } while (direction.decrement());
    return true;
  }
  assert(direction.isContract() || direction.isExpand());
  assert(!(*u)->isDependency());
  if (!(direction.isContract() ? Simplification::ShallowContract
                               : Simplification::ShallowExpand)(*u, false)) {
    return false;
  }
  // Apply a deep systematic reduction starting from (*u)
  UpwardSystemReduce(root, *u);
  // Move back to root so we only move down trees. Ignore dependencies
  *u = root;
  if (root->isDependency()) {
    if (keepDependencies) {
      *u = root->child(0);
    } else {
      root->child(1)->removeTree();
      root->removeNode();
    }
  }
  *rootChanged = true;
  return true;
}

bool AdvancedSimplification::ApplyPath(Tree* root, const Path* path,
                                       bool keepDependencies) {
  assert(!root->isDependency());
  Tree* u = root;
  bool rootChanged = false;
  for (uint8_t i = 0; i < path->length(); i++) {
    bool didApply = ApplyDirection(&u, root, path->direction(i), &rootChanged,
                                   keepDependencies);
    assert(didApply);
  }
  return rootChanged;
}

bool AdvancedSimplification::AdvancedReduceRec(Tree* u, Context* ctx) {
  bool fullExploration = true;
#if LOG_NEW_ADVANCED_REDUCTION_VERBOSE >= 4
  LogIndent();
  std::cout << "AdvancedReduceRec on subtree: ";
  u->logSerialize();
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
      Direction dir = Direction::SingleDirectionForIndex(i);
      if (ctx->m_mustResetRoot) {
        // Reset root to current path
        ctx->m_root->cloneTreeOverTree(ctx->m_original);
        ApplyPath(ctx->m_root, &ctx->m_path, false);
        ctx->m_mustResetRoot = false;
      }
      Tree* target = u;
      bool rootChanged = false;
      if (!CanApplyDirection(target, ctx->m_root, dir)) {
#if LOG_NEW_ADVANCED_REDUCTION_VERBOSE >= 3
        LogIndent();
        std::cout << "Can't apply ";
        dir.log();
        std::cout << ".\n";
#endif
        continue;
      }
      if (!ApplyDirection(&target, ctx->m_root, dir, &rootChanged, false)) {
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
        hash = CrcCollection::Hash(ctx->m_root);
      }
      /* If unchanged or unexplored, recursively advanced reduce. Otherwise, do
       * not go further. */
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
        bool canAddDir = ctx->m_path.append(dir);
        assert(canAddDir);
        if (!AdvancedReduceRec(target, ctx)) {
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
#else
  std::cout << "\n";
#endif
#endif
  if (metric < ctx->m_bestMetric) {
    ctx->m_bestMetric = metric;
    ctx->m_bestPath = ctx->m_path;
  }
  return fullExploration;
}

bool AdvancedSimplification::UpwardSystemReduce(Tree* root, const Tree* tree) {
  if (root == tree) {
    assert(!Simplification::DeepSystemReduce(root));
    return true;
  }
  assert(root < tree);
  for (Tree* child : root->children()) {
    if (UpwardSystemReduce(child, tree)) {
      Simplification::ShallowSystemReduce(root);
      return true;
    }
  }
  return false;
}

}  // namespace PoincareJ

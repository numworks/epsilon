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

bool AdvancedSimplification::AdvancedReduction(Tree* origin) {
  /* The advanced reduction is capped in depth by Path::k_size and in breadth by
   * CrcCollection::k_size. If this limit is reached, no further possibilities
   * will be explored.
   * This means calling AdvancedReduction on an equivalent but different
   * expression could yield different results if limits have been reached. */
  Tree* u = origin->isDependency() ? origin->child(0) : origin;
  int bestMetric = Metric::GetMetric(u);
  Path bestPath;
  Path currentPath;
  CrcCollection crcCollection;
  // Add initial root
  crcCollection.add(
      Ion::crc32Byte(reinterpret_cast<const uint8_t*>(u), u->treeSize()), 0);
  Tree* editedExpression = u->clone();
#if LOG_NEW_ADVANCED_REDUCTION_VERBOSE >= 1
  std::cout << "\nAdvancedReduction\nInitial tree (" << bestMetric << ") is : ";
  u->logSerialize();
  s_indent = 1;
#endif
  bool didOverflowPath = false;
  bool mustResetRoot = false;
  AdvancedReductionRec(editedExpression, editedExpression, u, &currentPath,
                       &bestPath, &bestMetric, &crcCollection, &didOverflowPath,
                       &mustResetRoot);
  editedExpression->removeTree();
  bool result = ApplyPath(u, &bestPath, true);
#if LOG_NEW_ADVANCED_REDUCTION_VERBOSE >= 1
  s_indent = 0;
  std::cout << "Final result (" << bestMetric << ") is : ";
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
  UpwardSystematicReduction(root, *u);
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

void AdvancedSimplification::AdvancedReductionRec(
    Tree* u, Tree* root, const Tree* original, Path* path, Path* bestPath,
    int* bestMetric, CrcCollection* crcCollection, bool* didOverflowPath,
    bool* mustResetRoot) {
#if LOG_NEW_ADVANCED_REDUCTION_VERBOSE >= 4
  LogIndent();
  std::cout << "AdvancedReductionRec on subtree: ";
  u->logSerialize();
#endif
  if (!path->canAddNewDirection()) {
    *didOverflowPath = true;
#if LOG_NEW_ADVANCED_REDUCTION_VERBOSE >= 1
    LogIndent();
    std::cout << "Full path.\n";
#endif
  } else {
    bool isLeaf = true;
    for (uint8_t i = 0; i < Direction::k_numberOfBaseDirections; i++) {
      Direction dir = Direction::SingleDirectionForIndex(i);
      if (*mustResetRoot) {
        // Reset root to current path
        root->cloneTreeOverTree(original);
        ApplyPath(root, path, false);
        *mustResetRoot = false;
      }
      Tree* target = u;
      bool rootChanged = false;
      if (!CanApplyDirection(target, root, dir)) {
#if LOG_NEW_ADVANCED_REDUCTION_VERBOSE >= 3
        LogIndent();
        std::cout << "Can't apply ";
        dir.log();
        std::cout << ".\n";
#endif
        continue;
      }
      if (!ApplyDirection(&target, root, dir, &rootChanged, false)) {
#if LOG_NEW_ADVANCED_REDUCTION_VERBOSE >= 3
        LogIndent();
        std::cout << "Nothing to ";
        dir.log();
        std::cout << ".\n";
#endif
        continue;
      }
      uint32_t crc32;
      if (rootChanged) {
        // No need for crc32 if root did not change.
        crc32 = Ion::crc32Byte(reinterpret_cast<const uint8_t*>(root),
                               root->treeSize());
      }
      /* If unchanged or unexplored, recursively advanced reduce. Otherwise, do
       * not go further. */
      if (!rootChanged || crcCollection->add(crc32, path->length())) {
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
            root->logSerialize();
          } else {
            std::cout << "\n";
          }
          s_indent++;
        }
#endif
        isLeaf = false;
        bool canAddDir = path->append(dir);
        assert(canAddDir);
        bool didOverflowPathRec = false;
        AdvancedReductionRec(target, root, original, path, bestPath, bestMetric,
                             crcCollection, &didOverflowPathRec, mustResetRoot);
        if (rootChanged && !didOverflowPathRec) {
          // No need to explore this again, even at smaller lengths.
          crcCollection->add(crc32, 0);
        }
        *didOverflowPath |= didOverflowPathRec;
        path->popBaseDirection();
#if LOG_NEW_ADVANCED_REDUCTION_VERBOSE >= 2
        if (shouldLog) {
          assert(s_indent > 0);
          s_indent--;
        }
#endif
      }
#if LOG_NEW_ADVANCED_REDUCTION_VERBOSE >= 1
      else if (crcCollection->isFull()) {
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
        root->logSerialize();
      }
#endif
      if (rootChanged) {
        // root will be reset to current path if needed later.
        *mustResetRoot = true;
      }
    }
    if (!isLeaf) {
      return;
    }
  }
  // Otherwise, root should be reset to current path.
  assert(!*mustResetRoot);
  // All directions are impossible, we are at a leaf. Compare metrics.
  int metric = Metric::GetMetric(root);
#if LOG_NEW_ADVANCED_REDUCTION_VERBOSE >= 1
  LogIndent();
  std::cout << "Leaf reached (" << metric << " VS " << *bestMetric << ")";
#if LOG_NEW_ADVANCED_REDUCTION_VERBOSE <= 1
  std::cout << ": ";
  root->logSerialize();
#else
  std::cout << "\n";
#endif
#endif
  if (metric < *bestMetric) {
    *bestMetric = metric;
    *bestPath = *path;
  }
}

bool AdvancedSimplification::UpwardSystematicReduction(Tree* root,
                                                       const Tree* tree) {
  if (root == tree) {
    assert(!Simplification::DeepSystematicReduce(root));
    return true;
  }
  assert(root < tree);
  for (Tree* child : root->children()) {
    if (UpwardSystematicReduction(child, tree)) {
      Simplification::ShallowSystematicReduce(root);
      return true;
    }
  }
  return false;
}

}  // namespace PoincareJ

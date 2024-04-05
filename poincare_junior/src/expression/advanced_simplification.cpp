#include "advanced_simplification.h"

#include <ion.h>
#include <poincare_junior/src/expression/dependency.h>
#include <poincare_junior/src/expression/metric.h>
#include <poincare_junior/src/expression/simplification.h>
#include <poincare_junior/src/memory/pattern_matching.h>

#include "k_tree.h"

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
  bool result = ctx.m_bestPath.apply(u, true);
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

bool AdvancedSimplification::Direction::canApply(const Tree* u,
                                                 const Tree* root) const {
  // Optimization: No trees are expected after root, so we can use lastBlock()
  assert(!isNextNode() ||
         (u->nextNode()->block() < SharedTreeStack->lastBlock()) ==
             u->nextNode()->hasAncestor(root, false));
  return !isNextNode() || u->nextNode()->block() < SharedTreeStack->lastBlock();
}

bool AdvancedSimplification::Direction::apply(Tree** u, Tree* root,
                                              bool* rootChanged,
                                              bool keepDependencies) const {
  if (isNextNode()) {
    assert(m_type >= k_baseNextNodeType);
    for (uint8_t i = m_type; i >= k_baseNextNodeType; i--) {
      *u = (*u)->nextNode();
    }
    return true;
  }
  assert(isContract() || isExpand());
  assert(!(*u)->isDependency());
  if (!(isContract() ? ShallowContract : ShallowExpand)(*u, false)) {
    return false;
  }
  // Apply a deep systematic reduction starting from (*u)
  UpwardSystematicReduce(root, *u);
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

#if POINCARE_TREE_LOG
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

bool AdvancedSimplification::Path::apply(Tree* root,
                                         bool keepDependencies) const {
  assert(!root->isDependency());
  Tree* u = root;
  bool rootChanged = false;
  for (uint8_t i = 0; i < length(); i++) {
    bool didApply = m_stack[i].apply(&u, root, &rootChanged, keepDependencies);
    assert(didApply);
  }
  return rootChanged;
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
        ctx->m_path.apply(ctx->m_root, false);
        ctx->m_mustResetRoot = false;
      }
      Tree* target = u;
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
      if (!dir.apply(&target, ctx->m_root, &rootChanged, false)) {
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

bool AdvancedSimplification::UpwardSystematicReduce(Tree* root,
                                                    const Tree* tree) {
  if (root == tree) {
    assert(!Simplification::DeepSystematicReduce(root));
    return true;
  }
  assert(root < tree);
  for (Tree* child : root->children()) {
    if (UpwardSystematicReduce(child, tree)) {
      Simplification::ShallowSystematicReduce(root);
      return true;
    }
  }
  return false;
}

/* Expand/Contract operations */

bool AdvancedSimplification::DeepContract(Tree* e) {
  bool changed = false;
  for (Tree* child : e->children()) {
    changed = DeepContract(child) || changed;
  }
  // TODO: Assert !DeepContract(e)
  return ShallowContract(e, true) || changed;
}

bool AdvancedSimplification::DeepExpand(Tree* e) {
  if (Tree::ApplyShallowInDepth(
          e, [](Tree* e, void* context) { return ShallowExpand(e, true); })) {
    // Bottom-up systematic reduce is necessary.
    Simplification::DeepSystematicReduce(e);
    // TODO_PCJ: Find a solution so we don't have to run this twice.
    bool temp = DeepExpand(e);
    assert(!temp || !DeepExpand(e));
    return true;
  }
  return false;
}

bool AdvancedSimplification::TryAllOperations(Tree* e,
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
  assert(!Simplification::DeepSystematicReduce(e));
  while (failures < numberOfOperations) {
    failures = operations[i % numberOfOperations](e) ? 0 : failures + 1;
    // EveryOperation should preserve e's reduced status
    assert(!Simplification::DeepSystematicReduce(e));
    i++;
  }
  return i > numberOfOperations;
}

bool AdvancedSimplification::TryOneOperation(Tree* e,
                                             const Tree::Operation* operations,
                                             int numberOfOperations) {
  assert(!Simplification::DeepSystematicReduce(e));
  for (size_t i = 0; i < numberOfOperations; i++) {
    if (operations[i](e)) {
      assert(!Simplification::DeepSystematicReduce(e));
      return true;
    }
  }
  return false;
}

bool AdvancedSimplification::ExpandImRe(Tree* ref) {
  return
      // im(A+B?) = im(A) + im(B)
      PatternMatching::MatchReplaceAndSimplify(
          ref, KIm(KAdd(KA, KB_p)), KAdd(KIm(KA), KIm(KAdd(KB_p)))) ||
      // re(A+B?) = re(A) + re(B)
      PatternMatching::MatchReplaceAndSimplify(
          ref, KRe(KAdd(KA, KB_p)), KAdd(KRe(KA), KRe(KAdd(KB_p)))) ||
      // im(A*B?) = im(A)re(B) + re(A)im(B)
      PatternMatching::MatchReplaceAndSimplify(
          ref, KIm(KMult(KA, KB_p)),
          KAdd(KMult(KIm(KA), KRe(KMult(KB_p))),
               KMult(KRe(KA), KIm(KMult(KB_p))))) ||
      // re(A*B?) = re(A)*re(B) - im(A)*im(B)
      PatternMatching::MatchReplaceAndSimplify(
          ref, KRe(KMult(KA, KB_p)),
          KAdd(KMult(KRe(KA), KRe(KMult(KB_p))),
               KMult(-1_e, KIm(KA), KIm(KMult(KB_p))))) ||
      // Replace im and re in additions only to prevent infinitely expanding
      // A? + B?*im(C)*D? + E? = A - i*B*C*D + i*B*re(C)*D + E
      PatternMatching::MatchReplaceAndSimplify(
          ref, KAdd(KA_s, KMult(KB_s, KIm(KC), KD_s), KE_p),
          KAdd(KA_s, KMult(-1_e, i_e, KB_s, KC, KD_s),
               KMult(i_e, KB_s, KRe(KC), KD_s), KE_p)) ||
      // A? + B?*re(C)*D? + E? = A + B*C*D - i*B*im(C)*D + E
      PatternMatching::MatchReplaceAndSimplify(
          ref, KAdd(KA_s, KMult(KB_s, KRe(KC), KD_s), KE_p),
          KAdd(KA_s, KMult(KB_s, KC, KD_s),
               KMult(-1_e, i_e, KB_s, KIm(KC), KD_s), KE_p)) ||
      // A? + B?*im(C)*D? = A - i*B*C*D + i*B*re(C)*D
      PatternMatching::MatchReplaceAndSimplify(
          ref, KAdd(KA_p, KMult(KB_s, KIm(KC), KD_s)),
          KAdd(KA_p, KMult(-1_e, i_e, KB_s, KC, KD_s),
               KMult(i_e, KB_s, KRe(KC), KD_s))) ||
      // A? + B?*re(C)*D? = A + B*C*D - i*B*im(C)*D
      PatternMatching::MatchReplaceAndSimplify(
          ref, KAdd(KA_p, KMult(KB_s, KRe(KC), KD_s)),
          KAdd(KA_p, KMult(KB_s, KC, KD_s),
               KMult(-1_e, i_e, KB_s, KIm(KC), KD_s)));
}

bool AdvancedSimplification::ContractAbs(Tree* ref) {
  // A?*|B|*|C|*D? = A*|BC|*D
  return PatternMatching::MatchReplaceAndSimplify(
      ref, KMult(KA_s, KAbs(KB), KAbs(KC), KD_s),
      KMult(KA_s, KAbs(KMult(KB, KC)), KD_s));
}

bool AdvancedSimplification::ExpandAbs(Tree* ref) {
  return
      // |A*B?| = |A|*|B|
      PatternMatching::MatchReplaceAndSimplify(
          ref, KAbs(KMult(KA, KB_p)), KMult(KAbs(KA), KAbs(KMult(KB_p)))) ||
      // |x| = √(re(x)^2+im(x)^2)
      PatternMatching::MatchReplaceAndSimplify(
          ref, KAbs(KA),
          KExp(
              KMult(KHalf, KLn(KAdd(KPow(KRe(KA), 2_e), KPow(KIm(KA), 2_e))))));
}

bool AdvancedSimplification::ExpandExp(Tree* ref) {
  return
      // exp(A?*i*B?) = cos(A*B) + i*sin(A*B)
      PatternMatching::MatchReplaceAndSimplify(
          ref, KExp(KMult(KA_s, i_e, KB_s)),
          KAdd(KTrig(KMult(KA_s, KB_s), 0_e),
               KMult(i_e, KTrig(KMult(KA_s, KB_s), 1_e)))) ||
      // exp(A+B?) = exp(A) * exp(B)
      PatternMatching::MatchReplaceAndSimplify(
          ref, KExp(KAdd(KA, KB_p)), KMult(KExp(KA), KExp(KAdd(KB_p))));
}

bool AdvancedSimplification::ContractExpMult(Tree* ref) {
  return
      // A? * exp(B) * exp(C) * D? = A * exp(B+C) * D
      PatternMatching::MatchReplaceAndSimplify(
          ref, KMult(KA_s, KExp(KB), KExp(KC), KD_s),
          KMult(KA_s, KExp(KAdd(KB, KC)), KD_s)) ||
      // A? + cos(B) + C? + i*sin(B) + D? = A + C + D + exp(i*B)
      PatternMatching::MatchReplaceAndSimplify(
          ref,
          KAdd(KA_s, KTrig(KB, 0_e), KC_s, KMult(i_e, KTrig(KB, 1_e)), KD_s),
          KAdd(KA_s, KC_s, KD_s, KExp(KMult(i_e, KB))));
}

bool AdvancedSimplification::ExpandMult(Tree* ref) {
  // We need at least one factor before or after addition.
  return
      // A?*(B+C?)*D? = A*B*D + A*C*D
      PatternMatching::MatchReplaceAndSimplify(
          ref, KMult(KA_p, KAdd(KB, KC_p), KD_s),
          KAdd(KMult(KA_p, KB, KD_s), KMult(KA_p, KAdd(KC_p), KD_s))) ||
      // (A+B?)*C? = A*C + B*C
      PatternMatching::MatchReplaceAndSimplify(
          ref, KMult(KAdd(KA, KB_p), KC_p),
          KAdd(KMult(KA, KC_p), KMult(KAdd(KB_p), KC_p)));
}

bool AdvancedSimplification::ContractMult(Tree* ref) {
  /* TODO: With  N and M positive, contract
   * A + B*A*C + A^N + D*A^M*E into A*(1 + B*C + A^(N-1) + D*A^(M-1)*E) */
  // A? + B?*C*D? + E? + F?*C*G? + H? = A + C*(B*D+F*G) + E + H
  return PatternMatching::MatchReplaceAndSimplify(
      ref, KAdd(KA_s, KMult(KB_s, KC, KD_s), KE_s, KMult(KF_s, KC, KG_s), KH_s),
      KAdd(KA_s, KMult(KC, KAdd(KMult(KB_s, KD_s), KMult(KF_s, KG_s))), KE_s,
           KH_s));
}

bool AdvancedSimplification::ExpandPower(Tree* ref) {
  // (A?*B)^C = A^C * B^C is currently in SystematicSimplification
  // (A + B?)^2 = (A^2 + 2*A*B + B^2)
  // TODO: Implement a more general (A + B)^C expand.
  return PatternMatching::MatchReplaceAndSimplify(
      ref, KPow(KAdd(KA, KB_p), 2_e),
      KAdd(KPow(KA, 2_e), KMult(2_e, KA, KAdd(KB_p)), KPow(KAdd(KB_p), 2_e)));
}

}  // namespace PoincareJ

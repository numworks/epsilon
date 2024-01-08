#include "simplification.h"

#include <ion.h>
#include <poincare_junior/src/expression/approximation.h>
#include <poincare_junior/src/expression/arithmetic.h>
#include <poincare_junior/src/expression/comparison.h>
#include <poincare_junior/src/expression/complex.h>
#include <poincare_junior/src/expression/decimal.h>
#include <poincare_junior/src/expression/dependency.h>
#include <poincare_junior/src/expression/derivation.h>
#include <poincare_junior/src/expression/dimension.h>
#include <poincare_junior/src/expression/k_tree.h>
#include <poincare_junior/src/expression/list.h>
#include <poincare_junior/src/expression/logarithm.h>
#include <poincare_junior/src/expression/matrix.h>
#include <poincare_junior/src/expression/number.h>
#include <poincare_junior/src/expression/parametric.h>
#include <poincare_junior/src/expression/random.h>
#include <poincare_junior/src/expression/rational.h>
#include <poincare_junior/src/expression/unit.h>
#include <poincare_junior/src/expression/variables.h>
#include <poincare_junior/src/expression/vector.h>
#include <poincare_junior/src/memory/exception_checkpoint.h>
#include <poincare_junior/src/memory/pattern_matching.h>
#include <poincare_junior/src/memory/placeholder.h>
#include <poincare_junior/src/n_ary.h>

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

bool Simplification::CrcCollection::add(uint32_t crc, uint8_t depth) {
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
void Simplification::Direction::log() {
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

bool Simplification::Direction::combine(Direction other) {
  if (!isNextNode() || !other.isNextNode() ||
      m_type >= k_expandType - other.m_type) {
    return false;
  }
  m_type += other.m_type;
  return true;
}

bool Simplification::Direction::decrement() {
  if (!isNextNode() || m_type == k_baseNextNodeType) {
    return false;
  }
  m_type--;
  return true;
}

void Simplification::Path::popBaseDirection() {
  assert(m_length > 0);
  if (!m_stack[m_length - 1].decrement()) {
    m_length--;
  }
}

bool Simplification::Path::append(Direction direction) {
  if (m_length == 0 || !m_stack[m_length - 1].combine(direction)) {
    if (m_length >= k_size) {
      return false;
    }
    m_stack[m_length] = direction;
    m_length += 1;
  }
  return true;
}

bool Simplification::CanApplyDirection(const Tree* u, const Tree* root,
                                       Direction direction) {
  // TODO: Optimize this check
  return !direction.isNextNode() || u->nextNode()->hasAncestor(root, false);
}

bool Simplification::ApplyDirection(Tree** u, Tree* root, Direction direction,
                                    bool* rootChanged) {
  if (direction.isNextNode()) {
    do {
      *u = (*u)->nextNode();
    } while (direction.decrement());
    return true;
  }
  assert(direction.isContract() || direction.isExpand());
  if (!(direction.isContract() ? ShallowContract : ShallowExpand)(*u, false)) {
    return false;
  }
  // Apply a deep systematic reduction starting from (*u)
  UpwardSystematicReduction(root, *u);
  // Move back to root so we only move down trees.
  *u = root;
  *rootChanged = true;
  return true;
}

bool Simplification::ApplyPath(Tree* u, const Path* path) {
  Tree* root = u;
  bool rootChanged = false;
  for (uint8_t i = 0; i < path->length(); i++) {
    bool didApply = ApplyDirection(&u, root, path->direction(i), &rootChanged);
    assert(didApply);
  }
  return rootChanged;
}

void Simplification::AdvancedReductionRec(Tree* u, Tree* root,
                                          const Tree* original, Path* path,
                                          Path* bestPath, int* bestMetric,
                                          CrcCollection* crcCollection) {
#if LOG_NEW_ADVANCED_REDUCTION_VERBOSE >= 4
  LogIndent();
  std::cout << "AdvancedReductionRec on subtree: ";
  u->logSerialize();
#endif
  if (!path->canAddNewDirection()) {
#if LOG_NEW_ADVANCED_REDUCTION_VERBOSE >= 1
    LogIndent();
    std::cout << "Full path.\n";
#endif
  } else if (crcCollection->isFull()) {
#if LOG_NEW_ADVANCED_REDUCTION_VERBOSE >= 1
    LogIndent();
    std::cout << "Full CRC collection.\n";
#endif
  } else {
    bool isLeaf = true;
    for (uint8_t i = 0; i < Direction::k_numberOfBaseDirections; i++) {
      Direction dir = Direction::SingleDirectionForIndex(i);
      Tree* target = u;
      // Apply direction if effective:
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
      if (!ApplyDirection(&target, root, dir, &rootChanged)) {
#if LOG_NEW_ADVANCED_REDUCTION_VERBOSE >= 3
        LogIndent();
        std::cout << "Nothing to ";
        dir.log();
        std::cout << ".\n";
#endif
        continue;
      }
      /* If unchanged or unexplored, recursively advanced reduce. Otherwise, do
       * not go further. */
      if (!rootChanged ||
          crcCollection->add(
              Ion::crc32Byte(reinterpret_cast<const uint8_t*>(root),
                             root->treeSize()),
              path->length())) {
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
        AdvancedReductionRec(target, root, original, path, bestPath, bestMetric,
                             crcCollection);
        path->popBaseDirection();
#if LOG_NEW_ADVANCED_REDUCTION_VERBOSE >= 2
        if (shouldLog) {
          assert(s_indent > 0);
          s_indent--;
        }
#endif
      }
#if LOG_NEW_ADVANCED_REDUCTION_VERBOSE >= 3
      else if (crcCollection->isFull()) {
        LogIndent();
        std::cout << "Full CRC collection.\n";
      } else {
        LogIndent();
        std::cout << "Already applied ";
        dir.log();
        std::cout << ": ";
        root->logSerialize();
      }
#endif
      // Undo changes on root.
      // TODO : Many times, undoing changes is unnecessary here.
      if (rootChanged) {
        root->cloneTreeOverTree(original);
        ApplyPath(root, path);
      }
    }
    if (!isLeaf) {
      return;
    }
  }
  // All directions are impossible, we are at a leaf. Compare metrics.
  int metric = GetMetric(root);
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

bool Simplification::AdvancedReduction(Tree* u) {
  /* The advanced reduction is capped in depth by Path::k_size and in breadth by
   * CrcCollection::k_size. If this limit is reached, no further possibilities
   * will be explored.
   * This means calling AdvancedReduction on an equivalent but different
   * expression could yield different results if limits have been reached. */
  int bestMetric = GetMetric(u);
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
  AdvancedReductionRec(editedExpression, editedExpression, u, &currentPath,
                       &bestPath, &bestMetric, &crcCollection);
  editedExpression->removeTree();
  bool result = ApplyPath(u, &bestPath);
#if LOG_NEW_ADVANCED_REDUCTION_VERBOSE >= 1
  s_indent = 0;
  std::cout << "Final result (" << bestMetric << ") is : ";
  u->logSerialize();
#endif
  return result;
}

bool Simplification::UpwardSystematicReduction(Tree* root, const Tree* tree) {
  if (root == tree) {
    assert(!DeepSystematicReduce(root));
    return true;
  }
  assert(root < tree);
  for (Tree* child : root->children()) {
    if (UpwardSystematicReduction(child, tree)) {
      ShallowSystematicReduce(root);
      return true;
    }
  }
  return false;
}

bool Simplification::DeepSystematicReduce(Tree* u) {
  /* Although they are also flattened in ShallowSystematicReduce, flattening
   * here could save multiple ShallowSystematicReduce and flatten calls. */
  bool modified =
      (u->isMultiplication() || u->isAddition()) && NAry::Flatten(u);
  for (Tree* child : u->children()) {
    modified |= DeepSystematicReduce(child);
    assert(!child->isUndefined());
  }
#if ASSERTIONS
  EditionReference previousTree = u->clone();
#endif
  bool shallowModified = ShallowSystematicReduce(u);
#if ASSERTIONS
  assert(shallowModified != u->treeIsIdenticalTo(previousTree));
  previousTree->removeTree();
#endif
  return shallowModified || modified;
}

/* Approximate all children if one of them is already float. Return true if the
 * entire tree have been approximated. */
bool CanApproximateTree(Tree* u, bool* changed) {
  for (Tree* child : u->children()) {
    if (child->isFloat()) {
      if (Approximation::ApproximateAndReplaceEveryScalar(u)) {
        *changed = true;
        if (u->isFloat()) {
          return true;
        }
      }
      break;
    }
  }
  return false;
}

bool Simplification::ShallowSystematicReduce(Tree* u) {
  // This assert is quite costly, should be an assert level 2 ?
  assert(Dimension::DeepCheckDimensions(u));
  if (u->numberOfChildren() == 0) {
    // No childless trees have a reduction pattern.
    return false;
  }
  bool changed = false;
  /* During a PatternMatching replace KPow(KA, KB) -> KExp(KMult(KLn(KA), KB))
   * with KA a Float and KB a UserVariable. We need to
   * ApproximateAndReplaceEveryScalar again on ShallowSystematicReduce. */
  if (CanApproximateTree(u, &changed)) {
    return true;
  }
  changed |= SimplifySwitch(u);
  if (Dependency::ShallowBubbleUpDependencies(u)) {
    ShallowSystematicReduce(u->nextNode());
    changed = true;
  }
  return changed;
}

bool Simplification::SimplifySwitch(Tree* u) {
  switch (u->type()) {
    case BlockType::Power:
      return SimplifyPower(u);
    case BlockType::Addition:
      return SimplifyAddition(u);
    case BlockType::Multiplication:
      return SimplifyMultiplication(u);
    case BlockType::PowerReal:
      return SimplifyPowerReal(u);
    case BlockType::Abs:
      return SimplifyAbs(u);
    case BlockType::TrigDiff:
      return Trigonometry::SimplifyTrigDiff(u);
    case BlockType::Trig:
      return Trigonometry::SimplifyTrig(u);
    case BlockType::ATrig:
      return Trigonometry::SimplifyATrig(u);
    case BlockType::Derivative:
      return Derivation::ShallowSimplify(u);
    case BlockType::Ln:
      return Logarithm::SimplifyLn(u);
    case BlockType::Exponential:
      return SimplifyExp(u);
    case BlockType::Complex:
      return SimplifyComplex(u);
    case BlockType::ComplexArgument:
      return SimplifyComplexArgument(u);
    case BlockType::ImaginaryPart:
      return SimplifyImaginaryPart(u);
    case BlockType::RealPart:
      return SimplifyRealPart(u);
    case BlockType::Sum:
    case BlockType::Product:
      return Parametric::SimplifySumOrProduct(u);
    case BlockType::GCD:
      return Arithmetic::SimplifyGCD(u);
    case BlockType::LCM:
      return Arithmetic::SimplifyLCM(u);
    case BlockType::Quotient:
    case BlockType::Remainder:
      return Arithmetic::SimplifyQuotientOrRemainder(u);
    case BlockType::Factorial:
      return Arithmetic::SimplifyFactorial(u);
    case BlockType::Binomial:
      return Arithmetic::SimplifyBinomial(u);
    case BlockType::Permute:
      return Arithmetic::SimplifyPermute(u);
    case BlockType::Sign:
      return SimplifySign(u);
    case BlockType::Floor:
      return Arithmetic::SimplifyFloor(u);
    case BlockType::Round:
      return Arithmetic::SimplifyRound(u);
    case BlockType::ListSort:
    case BlockType::Median:
      return List::ShallowApplyListOperators(u);
    case BlockType::Dim:
      if (Dimension::GetDimension(u->child(0)).isMatrix()) {
        return false;
      }
      return List::ShallowApplyListOperators(u);
    default:
      if (u->type().isListToScalar()) {
        return List::ShallowApplyListOperators(u);
      }
      return false;
  }
}

bool Simplification::SimplifyExp(Tree* u) {
  Tree* child = u->nextNode();
  if (child->isLn()) {
    // exp(ln(x)) -> x
    u->removeNode();
    u->removeNode();
    return true;
  }
  if (child->isZero()) {
    // exp(0) = 1
    u->cloneTreeOverTree(1_e);
    return true;
  }
  PatternMatching::Context ctx;
  if (PatternMatching::Match(KExp(KMult(KA, KLn(KB))), u, &ctx) &&
      ctx.getNode(KA)->isInteger()) {
    // exp(n*ln(x)) -> x^n with n an integer
    u->moveTreeOverTree(PatternMatching::CreateAndSimplify(KPow(KB, KA), ctx));
    return true;
  }
  return false;
}

bool Simplification::SimplifyAbs(Tree* u) {
  assert(u->isAbs());
  Tree* child = u->nextNode();
  bool changed = false;
  if (child->isAbs()) {
    // ||x|| -> |x|
    child->removeNode();
    changed = true;
  }
  if (child->isComplex()) {
    assert(Complex::IsSanitized(child));
    // |x+iy| = √(x^2+y^2) if x and y are reals
    return PatternMatching::MatchReplaceAndSimplify(
               u, KAbs(KComplex(KA, KB)),
               KExp(KMult(KHalf, KLn(KAdd(KPow(KA, 2_e), KPow(KB, 2_e)))))) ||
           changed;
  }
  if (!child->isNumber()) {
    return changed;
  }
  if (Number::Sign(child).isPositive()) {
    // |3| -> 3
    u->removeNode();
  } else {
    // |-3| -> (-1)*(-3)
    u->cloneTreeOverNode(KMult(-1_e));
    NAry::SetNumberOfChildren(u, 2);
    SimplifyMultiplication(u);
  }
  return true;
}

bool Simplification::SimplifyPower(Tree* u) {
  assert(u->isPower());
  Tree* v = u->child(0);
  // 1^x -> 1
  if (v->isOne()) {
    u->cloneTreeOverTree(1_e);
    return true;
  }
  // u^n
  EditionReference n = u->child(1);
  // After systematic reduction, a power can only have integer index.
  if (!n->isInteger()) {
    // TODO: Handle 0^x with x > 0 before to avoid ln(0)
    return PatternMatching::MatchReplaceAndSimplify(u, KPow(KA, KB),
                                                    KExp(KMult(KLn(KA), KB)));
  }
  // 0^n -> 0
  if (v->isZero()) {
    if (!n->isZero() && Rational::Sign(n).isStrictlyPositive()) {
      u->cloneTreeOverTree(0_e);
      return true;
    }
    ExceptionCheckpoint::Raise(ExceptionType::ZeroPowerZero);
  }
  if (v->isRational()) {
    u->moveTreeOverTree(Rational::IntegerPower(v, n));
    return true;
  }
  assert(n->isInteger());
  // v^0 -> 1
  if (n->isZero()) {
    if (Variables::HasVariables(v)) {
      return PatternMatching::MatchAndReplace(u, KPow(KA, 0_e),
                                              KDep(1_e, KSet(KPow(KA, -1_e))));
    } else {
      // TODO use sign to check if it may be null
      u->cloneTreeOverTree(1_e);
      return true;
    }
  }
  // v^1 -> v
  if (n->isOne()) {
    u->moveTreeOverTree(v);
    return true;
  }
  if (v->isComplex() && v->nextNode()->isZero()) {
    // (0 + A*i)^n -> ±(A^n) or (0±(A^n)*i)
    Tree* remainder =
        IntegerHandler::Remainder(Integer::Handler(n), IntegerHandler(4));
    int rem = Integer::Handler(remainder).to<uint8_t>();
    remainder->removeTree();
    v->nextNode()->removeTree();
    v->removeNode();
    // A^n
    SimplifyPower(u);
    // u could be any tree from this point forward
    if (rem > 1) {
      // -u
      u->moveTreeAtNode((-1_e)->clone());
      u->moveNodeAtNode(SharedEditionPool->push<BlockType::Multiplication>(2));
      SimplifyMultiplication(u);
    }
    if (rem % 2 == 1) {
      // u is a pure imaginary
      u->moveTreeAtNode((0_e)->clone());
      u->moveNodeAtNode(SharedEditionPool->push(BlockType::Complex));
      assert(!SimplifyComplex(u));
    }
    return true;
  }
  // (w^p)^n -> w^(p*n)
  if (v->isPower()) {
    EditionReference p = v->child(1);
    assert(p->nextTree() == static_cast<Tree*>(n));
    // PowU PowV w p n
    v->removeNode();
    MoveNodeAtNode(p, SharedEditionPool->push<BlockType::Multiplication>(2));
    // PowU w Mult<2> p n
    SimplifyMultiplication(p);
    SimplifyPower(u);
    return true;
  }
  // (w1*...*wk)^n -> w1^n * ... * wk^n
  if (v->isMultiplication()) {
    for (Tree* w : v->children()) {
      EditionReference m = SharedEditionPool->push(BlockType::Power);
      w->clone();
      n->clone();
      w->moveTreeOverTree(m);
      SimplifyPower(m);
    }
    n->removeTree();
    u->removeNode();
    SimplifyMultiplication(u);
    return true;
  }
  // exp(a)^b -> exp(a*b)
  return PatternMatching::MatchReplaceAndSimplify(u, KPow(KExp(KA), KB),
                                                  KExp(KMult(KA, KB)));
}

const Tree* Base(const Tree* u) { return u->isPower() ? u->child(0) : u; }

const Tree* Exponent(const Tree* u) { return u->isPower() ? u->child(1) : 1_e; }

void Simplification::ConvertPowerRealToPower(Tree* u) {
  u->cloneNodeOverNode(KPow);
  SimplifyPower(u);
}

bool Simplification::SimplifyPowerReal(Tree* u) {
  assert(u->isPowerReal());
  /* Return :
   * - x^y if x is complex or positive or y is integer
   * - PowerReal(x,y) if y is not a rational
   * - Looking at y's reduced rational form p/q :
   *   * PowerReal(x,y) if x is of unknown sign and p odd
   *   * Nonreal if q is even and x negative
   *   * |x|^y if p is even
   *   * -|x|^y if p is odd
   */
  Tree* x = u->child(0);
  Tree* y = u->child(1);
  bool xIsNumber = x->isNumber();
  bool xIsPositiveNumber = xIsNumber && Number::Sign(x).isPositive();
  bool xIsNegativeNumber = xIsNumber && !xIsPositiveNumber;
  if (xIsPositiveNumber || x->isComplex() || y->isInteger()) {
    // TODO : Handle sign and complex status not only on numbers
    ConvertPowerRealToPower(u);
    return true;
  }

  if (!y->isRational()) {
    // We don't know enough to simplify further.
    return false;
  }

  bool pIsEven = Rational::Numerator(y).isEven();
  bool qIsEven = Rational::Denominator(y).isEven();
  // y is simplified, both p and q can't be even
  assert(!qIsEven || !pIsEven);

  if (!pIsEven && !xIsNumber) {
    // We don't know enough to simplify further.
    return false;
  }
  assert(xIsNegativeNumber || pIsEven);

  if (xIsNegativeNumber && qIsEven) {
    ExceptionCheckpoint::Raise(ExceptionType::Nonreal);
  }

  // We can fallback to |x|^y
  x->cloneNodeAtNode(KAbs);
  SimplifyAbs(x);
  ConvertPowerRealToPower(u);

  if (xIsNegativeNumber && !pIsEven) {
    // -|x|^y
    u->cloneTreeAtNode(KMult(-1_e));
    NAry::SetNumberOfChildren(u, 2);
    SimplifyMultiplication(u);
  }
  return true;
}

bool Simplification::MergeMultiplicationChildWithNext(Tree* child) {
  Tree* next = child->nextTree();
  Tree* merge = nullptr;
  if (child->isNumber() && next->isNumber() &&
      !((child->isConstant()) || next->isConstant())) {
    // Merge numbers
    merge = Number::Multiplication(child, next);
  } else if (Base(child)->treeIsIdenticalTo(Base(next))) {
    // t^m * t^n -> t^(m+n)
    merge = PatternMatching::CreateAndSimplify(
        KPow(KA, KAdd(KB, KC)),
        {.KA = Base(child), .KB = Exponent(child), .KC = Exponent(next)});
    assert(!merge->isMultiplication());
  } else if (child->isComplex() || next->isComplex()) {
    // (A+B*i)*(C+D*i) -> ((AC-BD)+(AD+BC)*i)
    merge = PatternMatching::CreateAndSimplify(
        KComplex(KAdd(KMult(KA, KC), KMult(-1_e, KB, KD)),
                 KAdd(KMult(KA, KD), KMult(KB, KC))),
        {.KA = Complex::UnSanitizedRealPart(child),
         .KB = Complex::UnSanitizedImagPart(child),
         .KC = Complex::UnSanitizedRealPart(next),
         .KD = Complex::UnSanitizedImagPart(next)});
  }
  if (!merge) {
    return false;
  }
  // Replace both child and next with merge
  next->moveTreeOverTree(merge);
  child->removeTree();
  return true;
}

bool Simplification::MergeMultiplicationChildrenFrom(Tree* child, int index,
                                                     int* numberOfSiblings,
                                                     bool* zero) {
  bool changed = false;
  while (index < *numberOfSiblings) {
    if (child->isZero()) {
      *zero = true;
      return false;
    }
    if (child->isOne()) {
      child->removeTree();
    } else if (!(index + 1 < *numberOfSiblings &&
                 MergeMultiplicationChildWithNext(child))) {
      // Child is neither 0, 1 and can't be merged with next child (or is last).
      return changed;
    }
    (*numberOfSiblings)--;
    changed = true;
  }
  return changed;
}

bool Simplification::SimplifyMultiplicationChildRec(Tree* child, int index,
                                                    int* numberOfSiblings,
                                                    bool* multiplicationChanged,
                                                    bool* zero) {
  assert(index < *numberOfSiblings);
  // Merge child with right siblings as much as possible.
  bool childChanged =
      MergeMultiplicationChildrenFrom(child, index, numberOfSiblings, zero);
  // Simplify starting from next child.
  if (!*zero && index + 1 < *numberOfSiblings &&
      SimplifyMultiplicationChildRec(child->nextTree(), index + 1,
                                     numberOfSiblings, multiplicationChanged,
                                     zero)) {
    // Next child changed, child may now merge with it.
    assert(!*zero);
    childChanged =
        MergeMultiplicationChildrenFrom(child, index, numberOfSiblings, zero) ||
        childChanged;
  }
  if (*zero) {
    return false;
  }
  *multiplicationChanged = *multiplicationChanged || childChanged;
  return childChanged;
}

bool Simplification::SimplifySortedMultiplication(Tree* multiplication) {
  int n = multiplication->numberOfChildren();
  bool changed = false;
  bool zero = false;
  /* Recursively merge children.
   * Keep track of n, changed status and presence of zero child. */
  SimplifyMultiplicationChildRec(multiplication->nextNode(), 0, &n, &changed,
                                 &zero);
  NAry::SetNumberOfChildren(multiplication, n);
  if (zero) {
    multiplication->cloneTreeOverTree(0_e);
    return true;
  }
  if (!changed || NAry::SquashIfUnary(multiplication) ||
      NAry::SquashIfEmpty(multiplication)) {
    return changed;
  }
  /* Merging children can un-sort the multiplication. It must then be simplified
   * again once sorted again. For example:
   * 3*a*i*i -> Simplify -> 3*a*-1 -> Sort -> -1*3*a -> Simplify -> -3*a */
  if (NAry::Sort(multiplication, Comparison::Order::PreserveMatrices)) {
    SimplifySortedMultiplication(multiplication);
  }
  return true;
}

bool Simplification::SimplifyMultiplication(Tree* u) {
  assert(u->isMultiplication());
  bool changed = NAry::Flatten(u);
  if (changed && CanApproximateTree(u, &changed)) {
    /* In case of successful flatten, approximateAndReplaceEveryScalar must be
     * tried again to properly handle possible new float children. */
    return true;
  }
  if (NAry::SquashIfUnary(u) || NAry::SquashIfEmpty(u)) {
    return true;
  }
  changed = NAry::Sort(u, Comparison::Order::PreserveMatrices) || changed;
  changed = SimplifySortedMultiplication(u) || changed;
  assert(!changed || !u->isMultiplication() || !SimplifyMultiplication(u));
  return changed;
}

bool TermsAreEqual(const Tree* u, const Tree* v) {
  if (!u->isMultiplication()) {
    if (!v->isMultiplication()) {
      return u->treeIsIdenticalTo(v);
    }
    return TermsAreEqual(v, u);
  }
  if (!v->isMultiplication()) {
    return u->numberOfChildren() == 2 && u->child(0)->isRational() &&
           u->child(1)->treeIsIdenticalTo(v);
  }
  bool uHasRational = u->child(0)->isRational();
  bool vHasRational = v->child(0)->isRational();
  int n = u->numberOfChildren() - uHasRational;
  if (n != v->numberOfChildren() - vHasRational) {
    return false;
  }
  const Tree* childU = u->child(uHasRational);
  const Tree* childV = v->child(vHasRational);
  for (int i = 0; i < n; i++) {
    if (!childU->treeIsIdenticalTo(childV)) {
      return false;
    }
    childU = childU->nextTree();
    childV = childV->nextTree();
  }
  return true;
}

// The term of 2ab is ab
Tree* PushTerm(const Tree* u) {
  Tree* c = u->clone();
  if (u->isMultiplication() && u->child(0)->isRational()) {
    NAry::RemoveChildAtIndex(c, 0);
    NAry::SquashIfUnary(c);
  }
  return c;
}

// The constant of 2ab is 2
const Tree* Constant(const Tree* u) {
  if (u->isMultiplication() && u->child(0)->isRational()) {
    return u->child(0);
  }
  return 1_e;
}

bool Simplification::MergeAdditionChildWithNext(Tree* child, Tree* next) {
  assert(next == child->nextTree());
  Tree* merge = nullptr;
  if (child->isNumber() && next->isNumber() &&
      !((child->isConstant()) || next->isConstant())) {
    // Merge numbers
    merge = Number::Addition(child, next);
  } else if (TermsAreEqual(child, next)) {
    // k1 * a + k2 * a -> (k1+k2) * a
    Tree* term = PushTerm(child);
    merge = PatternMatching::CreateAndSimplify(
        KMult(KAdd(KA, KB), KC),
        {.KA = Constant(child), .KB = Constant(next), .KC = term});
    term->removeTree();
    merge = term;
  } else if (child->isComplex() || next->isComplex()) {
    // (A+B*i)+(C+D*i) -> ((A+C)+(B+D)*i)
    merge = PatternMatching::CreateAndSimplify(
        KComplex(KAdd(KA, KC), KAdd(KB, KD)),
        {.KA = Complex::UnSanitizedRealPart(child),
         .KB = Complex::UnSanitizedImagPart(child),
         .KC = Complex::UnSanitizedRealPart(next),
         .KD = Complex::UnSanitizedImagPart(next)});
  }
  if (!merge) {
    return false;
  }
  // Replace both child and next with merge
  next->moveTreeOverTree(merge);
  child->removeTree();
  return true;
}

bool Simplification::SimplifyAddition(Tree* u) {
  assert(u->isAddition());
  bool modified = NAry::Flatten(u);
  if (modified && CanApproximateTree(u, &modified)) {
    /* In case of successful flatten, approximateAndReplaceEveryScalar must be
     * tried again to properly handle possible new float children. */
    return true;
  }
  if (NAry::SquashIfUnary(u)) {
    return true;
  }
  modified = NAry::Sort(u) || modified;
  int n = u->numberOfChildren();
  int i = 0;
  Tree* child = u->nextNode();
  while (i < n) {
    if (child->isZero()) {
      child->removeTree();
      n--;
      continue;
    }
    Tree* next = child->nextTree();
    if (i + 1 < n && MergeAdditionChildWithNext(child, next)) {
      // 1 + (a + b)/2 + (a + b)/2 -> 1 + a + b
      if (child->isAddition()) {
        n += child->numberOfChildren() - 1;
        child->removeNode();
        // n may remain equal to u->numberOfChildren()
        modified = true;
      }
      n--;
    } else {
      child = next;
      i++;
    }
  }
  if (n == u->numberOfChildren()) {
    return modified;
  }
  NAry::SetNumberOfChildren(u, n);
  if (NAry::SquashIfUnary(u) || NAry::SquashIfEmpty(u)) {
    return true;
  }
  /* TODO: SimplifyAddition may encounter the same issues as the multiplication.
   * If this assert can't be preserved, SimplifyAddition must handle one or both
   * of this cases as handled in multiplication:
   * With a,b and c the sorted addition children (a < b < c), M(a,b) the result
   * of merging children a and b (with MergeAdditionChildWithNext) if it exists.
   * - M(a,b) > c or a > M(b,c) (Addition must be sorted again)
   * - M(a,b) doesn't exists, but M(a,M(b,c)) does (previous child should try
   *   merging again when child merged with nextCHild) */
  assert(!SimplifyAddition(u));
  return true;
}

bool Simplification::SimplifyComplex(Tree* tree) {
  assert(tree->isComplex());
  Tree* imag = tree->child(1);
  if (imag->isZero()) {
    // (A+0*i) -> A
    imag->removeTree();
    tree->removeNode();
    return true;
  }
  if (PatternMatching::MatchAndReplace(tree, KComplex(KRe(KA), KIm(KA)), KA)) {
    // re(x)+i*im(x) = x
    return true;
  }
  if (Complex::IsSanitized(tree)) {
    return false;
  }
  // x+iy = (re(x)-im(y)) + i*(im(x)+re(y))
  bool result = PatternMatching::MatchReplaceAndSimplify(
      tree, KComplex(KA, KB),
      KComplex(KAdd(KRe(KA), KMult(-1_e, KIm(KB))), KAdd(KIm(KA), KRe(KB))));
  assert(result && Complex::IsSanitized(tree));
  return result;
}

bool Simplification::SimplifyComplexArgument(Tree* tree) {
  assert(tree->isComplexArgument());
  Tree* child = tree->child(0);
  if (child->isNumber()) {
    Sign::Sign sign = Number::Sign(child);
    if (sign.isZero()) {
      ExceptionCheckpoint::Raise(ExceptionType::Unhandled);
    }
    tree->cloneTreeOverTree(sign.isStrictlyPositive() ? 0_e : π_e);
    return true;
  }
  // TODO: Implement for complexes
  return false;
}

bool Simplification::SimplifyRealPart(Tree* tree) {
  assert(tree->isRealPart());
  Tree* child = tree->child(0);
  if (child->isComplex() || Complex::IsReal(child)) {
    assert(Complex::IsSanitized(child));
    // re(x+i*y) = x if x and y are reals
    tree->cloneTreeOverTree(Complex::UnSanitizedRealPart(child));
    return true;
  }
  // re(x+y) = re(x)+re(z)
  return (child->isAddition()) &&
         Simplification::DistributeOverNAry(
             tree, BlockType::RealPart, BlockType::Addition,
             BlockType::Addition, SimplifyRealPart);
}

bool Simplification::SimplifyImaginaryPart(Tree* tree) {
  assert(tree->isImaginaryPart());
  Tree* child = tree->child(0);
  if (child->isComplex() || Complex::IsReal(child)) {
    assert(Complex::IsSanitized(child));
    // im(x+i*y) = y if x and y are reals
    tree->cloneTreeOverTree(Complex::UnSanitizedImagPart(child));
    return true;
  }
  // im(x+y) = im(x)+im(z)
  return (child->isAddition()) &&
         Simplification::DistributeOverNAry(
             tree, BlockType::ImaginaryPart, BlockType::Addition,
             BlockType::Addition, SimplifyImaginaryPart);
}

bool Simplification::SimplifySign(Tree* expr) {
  assert(expr->isSign());
  Sign::Sign sign = Sign::GetSign(expr->firstChild());
  const Tree* result;
  if (sign.isZero()) {
    result = 0_e;
  } else if (sign.isStrictlyPositive()) {
    result = 1_e;
  } else if (sign.isStrictlyNegative()) {
    result = -1_e;
  } else {
    return false;
  }
  expr->cloneTreeOverTree(result);
  return true;
}

bool ShouldApproximateOnSimplify(Dimension dimension) {
  // Only angle units are expected not to be approximated.
  return (dimension.isUnit() && !dimension.isAngleUnit());
}

bool RelaxProjectionContext(void* context) {
  ProjectionContext* projectionContext =
      static_cast<ProjectionContext*>(context);
  if (projectionContext->m_strategy == Strategy::ApproximateToFloat) {
    // Nothing more can be done.
    return false;
  }
  projectionContext->m_strategy =
      (projectionContext->m_strategy == Strategy::NumbersToFloat)
          ? Strategy::ApproximateToFloat
          : Strategy::NumbersToFloat;
  return true;
}

bool Simplification::Simplify(Tree* ref, ProjectionContext projectionContext) {
  // Clone the tree, and use an adaptive strategy to handle pool overflow.
  SharedEditionPool->executeAndReplaceTree(
      [](void* context, const void* data) {
        SimplifyLastTree(static_cast<const Tree*>(data)->clone(),
                         *static_cast<ProjectionContext*>(context));
      },
      &projectionContext, ref, RelaxProjectionContext);
  /* TODO: Due to projection/beautification cycles, SimplifyLastTree will most
   *       likely return true everytime anyway. */
  return true;
}

bool Simplification::SimplifyLastTree(Tree* ref,
                                      ProjectionContext projectionContext) {
  assert(SharedEditionPool->lastBlock() == ref->nextTree()->block());
  ExceptionTryAfterBlock(ref->block()) {
    if (!Dimension::DeepCheckDimensions(ref) ||
        !Dimension::DeepCheckListLength(ref)) {
      // TODO: Raise appropriate exception in DeepCheckDimensions.
      ExceptionCheckpoint::Raise(ExceptionType::UnhandledDimension);
    }
    projectionContext.m_dimension = Dimension::GetDimension(ref);
    if (projectionContext.m_strategy != Strategy::ApproximateToFloat &&
        ShouldApproximateOnSimplify(projectionContext.m_dimension)) {
      ExceptionCheckpoint::Raise(ExceptionType::RelaxContext);
    }
    bool changed = false;
    // Seed random nodes before anything is merged/duplicated.
    changed = Random::SeedTreeNodes(ref) > 0;
    changed =
        Projection::DeepSystemProjection(ref, projectionContext) || changed;
    Tree* variables = Variables::GetUserSymbols(ref);
    SwapTrees(&ref, &variables);
    Variables::ProjectToId(ref, variables);
    changed = DeepSystematicReduce(ref) || changed;
    changed = DeepApplyMatrixOperators(ref) || changed;
    assert(!DeepSystematicReduce(ref));
    assert(!DeepApplyMatrixOperators(ref));
    changed =
        List::BubbleUp(
            ref, [](Tree* e) -> bool { return ShallowSystematicReduce(e); }) ||
        changed;
    changed = AdvancedReduction(ref) || changed;

    if (projectionContext.m_strategy == Strategy::ApproximateToFloat) {
      // Approximate again in case exact numbers appeared during simplification.
      changed = Approximation::ApproximateAndReplaceEveryScalar(ref);
    }
    changed = Beautification::DeepBeautify(ref, projectionContext) || changed;
    Variables::BeautifyToName(ref, variables);
    variables->removeTree();
    return changed;
  }
  ExceptionCatch(type) {
    switch (type) {
      case ExceptionType::BadType:
      case ExceptionType::Nonreal:
      case ExceptionType::ZeroPowerZero:
      case ExceptionType::ZeroDivision:
      case ExceptionType::UnhandledDimension:
      case ExceptionType::Unhandled:
        (type == ExceptionType::Nonreal ? KNonreal : KUndef)->clone();
        return true;
      default:
        ExceptionCheckpoint::Raise(type);
    }
  }
}

bool Simplification::DistributeOverNAry(Tree* ref, BlockType target,
                                        BlockType naryTarget,
                                        BlockType naryOutput,
                                        Operation operation, int childIndex) {
  assert(naryTarget == BlockType::Addition ||
         naryTarget == BlockType::Multiplication);
  assert(naryOutput == BlockType::Addition ||
         naryOutput == BlockType::Multiplication);
  if (ref->type() != target) {
    return false;
  }
  int numberOfChildren = ref->numberOfChildren();
  assert(childIndex < numberOfChildren);
  EditionReference children = ref->child(childIndex);
  if (children->type() != naryTarget) {
    // Apply operation anyway, as if it was in a squashed NAry
    // f(A,E) -> f'(A,E)
    return operation(ref);
  }
  int numberOfGrandChildren = children->numberOfChildren();
  size_t childIndexOffset = children->block() - ref->block();
  // f(+(A,B,C),E)
  children->cloneTreeBeforeNode(0_e);
  children = children->detachTree();
  // f(0,E) ... +(A,B,C)
  Tree* grandChild = children->nextNode();
  EditionReference output =
      naryOutput == BlockType::Addition
          ? SharedEditionPool->push<BlockType::Addition>(numberOfGrandChildren)
          : SharedEditionPool->push<BlockType::Multiplication>(
                numberOfGrandChildren);
  // f(0,E) ... +(A,B,C) ... *(,,)
  for (int i = 0; i < numberOfGrandChildren; i++) {
    EditionReference clone = ref->clone();
    // f(0,E) ... +(A,B,C) ... *(f(0,E),,)
    /* Since it is constant, use a childIndexOffset to avoid child calls:
     * clone.child(childIndex)=Tree(clone.block()+childIndexOffset) */
    EditionReference(clone->block() + childIndexOffset)
        ->moveTreeOverTree(grandChild);
    // f(0,E) ... +(,B,C) ... *(f(A,E),,)
    operation(clone);
    // f(0,E) ... +(,B,C) ... *(f'(A,E),,)
  }
  // f(0,E) ... +(,,) ... *(f'(A,E), f'(B,E), f'(C,E))
  children->removeNode();
  // f(0,E) ... *(f'(A,E), f'(B,E), f'(C,E))
  ref = ref->moveTreeOverTree(output);
  // *(f'(A,E), f'(B,E), f'(C,E)) ...
  ShallowSystematicReduce(ref);
  return true;
}

bool Simplification::TryAllOperations(Tree* e, const Operation* operations,
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
  assert(!DeepSystematicReduce(e));
  while (failures < numberOfOperations) {
    failures = operations[i % numberOfOperations](e) ? 0 : failures + 1;
    // EveryOperation should preserve e's reduced status
    assert(!DeepSystematicReduce(e));
    i++;
  }
  return i > numberOfOperations;
}

bool Simplification::TryOneOperations(Tree* e, const Operation* operations,
                                      int numberOfOperations) {
  assert(!DeepSystematicReduce(e));
  for (size_t i = 0; i < numberOfOperations; i++) {
    if (operations[i](e)) {
      assert(!DeepSystematicReduce(e));
      return true;
    }
  }
  return false;
}

bool Simplification::DeepContract(Tree* e) {
  bool changed = false;
  for (Tree* child : e->children()) {
    changed = DeepContract(child) || changed;
  }
  // TODO: Assert !DeepContract(e)
  return ShallowContract(e, true) || changed;
}

bool Simplification::DeepExpand(Tree* e) {
  if (Tree::ApplyShallowInDepth(
          e, [](Tree* e, void* context) { return ShallowExpand(e, true); })) {
    // Bottom-up systematic reduce is necessary.
    DeepSystematicReduce(e);
    // TODO: Find a solution so we don't have to run this twice.
    bool temp = DeepExpand(e);
    assert(!temp || !DeepExpand(e));
    return true;
  }
  return false;
}

bool Simplification::ContractAbs(Tree* ref) {
  // A?*|B|*|C|*D? = A*|BC|*D
  return PatternMatching::MatchReplaceAndSimplify(
      ref, KMult(KTA, KAbs(KB), KAbs(KC), KTD),
      KMult(KTA, KAbs(KMult(KB, KC)), KTD));
}

bool Simplification::ExpandAbs(Tree* ref) {
  // |A*B?| = |A|*|B|
  return PatternMatching::MatchReplaceAndSimplify(
      ref, KAbs(KMult(KA, KTB)), KMult(KAbs(KA), KAbs(KMult(KTB))));
}

bool Simplification::ExpandExp(Tree* ref) {
  return
      // exp(A+iB) = exp(A)*(cos(B) + i*sin(B))
      PatternMatching::MatchReplaceAndSimplify(
          ref, KExp(KComplex(KA, KB)),
          KMult(KExp(KA), KComplex(KTrig(KB, 0_e), KTrig(KB, 1_e)))) ||
      // exp(A+B?) = exp(A) * exp(B)
      PatternMatching::MatchReplaceAndSimplify(
          ref, KExp(KAdd(KA, KTB)), KMult(KExp(KA), KExp(KAdd(KTB))));
}

bool Simplification::ContractExpMult(Tree* ref) {
  // A? * exp(B) * exp(C) * D? = A * exp(B+C) * D
  return PatternMatching::MatchReplaceAndSimplify(
      ref, KMult(KTA, KExp(KB), KExp(KC), KTD),
      KMult(KTA, KExp(KAdd(KB, KC)), KTD));
}

bool Simplification::ExpandMult(Tree* ref) {
  // A?*(B+C?)*D? = A*B*D + A*C*D
  return PatternMatching::MatchReplaceAndSimplify(
      ref, KMult(KTA, KAdd(KB, KTC), KTD),
      KAdd(KMult(KTA, KB, KTD), KMult(KTA, KAdd(KTC), KTD)));
}

bool Simplification::ContractMult(Tree* ref) {
  // A? + B?*C*D? + E? + F?*C*G? + H? = A + C*(B*D+F*G) + E + H
  return PatternMatching::MatchReplaceAndSimplify(
      ref, KAdd(KTA, KMult(KTB, KC, KTD), KTE, KMult(KTF, KC, KTG), KTH),
      KAdd(KTA, KMult(KC, KAdd(KMult(KTB, KTD), KMult(KTF, KTG))), KTE, KTH));
}

bool Simplification::ExpandPowerComplex(Tree* ref) {
  // (A + B*i)^2 = (A^2 -2*B^2 + 2*A*B*i)
  return PatternMatching::MatchReplaceAndSimplify(
      ref, KPow(KComplex(KA, KB), 2_e),
      KComplex(KAdd(KPow(KA, 2_e), KMult(-1_e, KPow(KB, 2_e))),
               KMult(2_e, KA, KB)));
}

bool Simplification::ExpandPower(Tree* ref) {
  // (A?*B)^C = A^C * B^C is currently in SystematicSimplification
  // (A? + B)^2 = (A^2 + 2*A*B + B^2)
  // TODO: Implement a more general (A + B)^C expand.
  return PatternMatching::MatchReplaceAndSimplify(
      ref, KPow(KAdd(KTA, KB), 2_e),
      KAdd(KPow(KAdd(KTA), 2_e), KMult(2_e, KAdd(KTA), KB), KPow(KB, 2_e)));
}

bool Simplification::ShallowApplyMatrixOperators(Tree* tree, void* context) {
  if (tree->numberOfChildren() < 1) {
    return false;
  }
  Tree* child = tree->child(0);
  if (tree->isIdentity()) {
    tree->moveTreeOverTree(Matrix::Identity(child));
    return true;
  }
  if (tree->isMultiplication()) {
    int numberOfMatrices = tree->numberOfChildren();
    // Find first matrix
    const Tree* firstMatrix = nullptr;
    for (const Tree* child : tree->children()) {
      if (child->isMatrix()) {
        firstMatrix = child;
        break;
      }
      numberOfMatrices--;
    }
    if (!firstMatrix) {
      return false;
    }
    Tree* result = firstMatrix->clone();
    Tree* child = tree->nextNode();
    // Merge matrices
    while (child < firstMatrix) {
      result->moveTreeOverTree(Matrix::ScalarMultiplication(child, result));
      child = child->nextTree();
    }
    while (--numberOfMatrices) {
      child = child->nextTree();
      result->moveTreeOverTree(Matrix::Multiplication(result, child));
    }
    tree->moveTreeOverTree(result);
    return true;
  }
  if (!child->isMatrix()) {
    return false;
  }
  if (tree->isAddition()) {
    int n = tree->numberOfChildren() - 1;
    Tree* result = child->clone();
    while (n--) {
      child = child->nextTree();
      result->moveTreeOverTree(Matrix::Addition(result, child));
    }
    tree->moveTreeOverTree(result);
    return true;
  }
  if (tree->isPowerMatrix()) {
    Tree* index = child->nextTree();
    if (!Integer::Is<int>(index)) {
      // TODO: Raise to rely on approximation.
      return false;
    }
    tree->moveTreeOverTree(
        Matrix::Power(child, Integer::Handler(index).to<int>()));
    return true;
  }
  if (tree->numberOfChildren() == 2) {
    Tree* child2 = child->nextTree();
    if (!child2->isMatrix()) {
      return false;
    }
    switch (tree->type()) {
      case BlockType::Cross:
        tree->moveTreeOverTree(Vector::Cross(child, child2));
        return true;
      case BlockType::Dot:
        tree->moveTreeOverTree(Vector::Dot(child, child2));
        return true;
      default:
        return false;
    }
  }
  switch (tree->type()) {
    case BlockType::Inverse:
      tree->moveTreeOverTree(Matrix::Inverse(child));
      return true;
    case BlockType::Ref:
      Matrix::RowCanonize(child, false);
      tree->removeNode();
      return true;
    case BlockType::Rref:
      Matrix::RowCanonize(child, true);
      tree->removeNode();
      return true;
    case BlockType::Trace:
      tree->moveTreeOverTree(Matrix::Trace(child));
      return true;
    case BlockType::Transpose:
      tree->moveTreeOverTree(Matrix::Transpose(child));
      return true;
    case BlockType::Dim: {
      assert(child->isMatrix());
      Tree* dim = SharedEditionPool->push<BlockType::Matrix>(1, 2);
      Integer::Push(Matrix::NumberOfRows(child));
      Integer::Push(Matrix::NumberOfColumns(child));
      tree->moveTreeOverTree(dim);
      return true;
    }
    case BlockType::Det: {
      Tree* determinant;
      Matrix::RowCanonize(child, true, &determinant);
      tree->moveTreeOverTree(determinant);
      return true;
    }
    case BlockType::Norm:
      tree->moveTreeOverTree(Vector::Norm(child));
      return true;
    default:
      return false;
  }
}

bool Simplification::DeepApplyMatrixOperators(Tree* tree) {
  bool changed = false;
  for (Tree* child : tree->children()) {
    changed |= DeepApplyMatrixOperators(child);
  }
  changed |= ShallowApplyMatrixOperators(tree);
  return changed;
}

}  // namespace PoincareJ

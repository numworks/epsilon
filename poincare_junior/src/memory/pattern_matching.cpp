#include "pattern_matching.h"

#include <poincare_junior/src/expression/simplification.h>

#include "../n_ary.h"
#include "poincare_junior/src/memory/type_block.h"

using namespace PoincareJ;

bool PatternMatching::Context::isUninitialized() const {
  for (const Tree* node : m_array) {
    if (node) {
      return false;
    }
  }
  return true;
}

#if POINCARE_TREE_LOG
void PatternMatching::Context::log() const {
  std::cout << "<Context>\n";
  for (int i = 0; i < Placeholder::Tag::NumberOfTags; i++) {
    int numberOfTress = m_numberOfTrees[i];
    char tagName[2] = {static_cast<char>('A' + i), 0};
    std::cout << "  <PlaceHolder tag=" << tagName << " trees=" << numberOfTress
              << ">\n";
    const Tree* tree = m_array[i];
    if (tree) {
      for (int j = 0; j < numberOfTress; j++) {
        tree->log(std::cout, true, true, 2);
        tree = tree->nextTree();
      }
    } else {
      std::cout << "    <Uninitialized/>\n";
    }
    std::cout << "  </PlaceHolder>\n";
  }
  std::cout << "</Context>\n";
}

void logGlobal(const Tree* globalRoot, const Block* globalRootEnd,
               const Tree* localRoot, const Block* localRootEnd) {
  bool localEndPrinted = false;
  for (const Tree* u : globalRoot->selfAndDescendants()) {
    assert(u != globalRootEnd);
    if (u == localRoot) {
      std::cout << " ( ";
    }
    if (u == localRootEnd) {
      localEndPrinted = true;
      std::cout << " ) ";
    }
    std::cout << "[";
    u->logName(std::cout);
    u->logAttributes(std::cout);
    std::cout << "]";
  }
  if (!localEndPrinted) {
    std::cout << " ) ";
  }
  std::cout << "\n";
}

void PatternMatching::MatchContext::log() const {
  std::cout << "\tSource  : ";
  logGlobal(m_globalSourceRoot, m_globalSourceEnd, m_localSourceRoot,
            m_localSourceEnd);
  std::cout << "\tPattern : ";
  logGlobal(m_globalPatternRoot, m_globalPatternEnd, m_localPatternRoot,
            m_localPatternEnd);
}
#endif

PatternMatching::MatchContext::MatchContext(const Tree* source,
                                            const Tree* pattern)
    : m_localSourceRoot(source),
      m_localSourceEnd(source->nextTree()->block()),
      m_localPatternRoot(pattern),
      m_localPatternEnd(pattern->nextTree()->block()),
      m_globalSourceRoot(source),
      m_globalPatternRoot(pattern),
      m_globalSourceEnd(m_localSourceEnd),
      m_globalPatternEnd(m_localPatternEnd) {}

int PatternMatching::MatchContext::remainingLocalTrees(const Tree* node) const {
  if (ReachedLimit(node, m_localSourceEnd)) {
    return 0;
  }
  assert(m_localSourceRoot->isSimpleNAry());
  // Parent is expected to be m_localSourceRoot, but we need nodePosition.
  int nodePosition;
  const Tree* parent =
      m_localSourceRoot->parentOfDescendant(node, &nodePosition);
  assert(parent && parent == m_localSourceRoot);
  return m_localSourceRoot->numberOfChildren() - nodePosition;
}

void PatternMatching::MatchContext::setLocal(const Tree* source,
                                             const Tree* pattern) {
  m_localSourceRoot = source;
  m_localSourceEnd = source->nextTree()->block();
  m_localPatternRoot = pattern;
  m_localPatternEnd = pattern->nextTree()->block();
}

void PatternMatching::MatchContext::setLocalToParent() {
  const Tree* sourceParent =
      m_globalSourceRoot->parentOfDescendant(m_localSourceRoot);
  const Tree* patternParent =
      m_globalPatternRoot->parentOfDescendant(m_localPatternRoot);
  assert(patternParent && sourceParent &&
         (sourceParent->type() == patternParent->type()));
  setLocal(sourceParent, patternParent);
}

bool PatternMatching::MatchAnyTrees(Placeholder::Tag tag, const Tree* source,
                                    const Tree* pattern, Context* context,
                                    MatchContext matchContext) {
  Placeholder::Filter filter = Placeholder::NodeToFilter(pattern);
  assert(filter == Placeholder::Filter::OneOrMore ||
         filter == Placeholder::Filter::ZeroOrMore);
  int maxNumberOfTrees = matchContext.remainingLocalTrees(source);
  int numberOfTrees = (filter == Placeholder::Filter::OneOrMore) ? 1 : 0;
  context->setNode(tag, source, numberOfTrees, true);
  if (filter == Placeholder::Filter::OneOrMore) {
    source = source->nextTree();
  }
  Context newResult = *context;

  // Give the placeholder a growing number of trees until everything matches.
  while (!MatchNodes(source, pattern->nextNode(), &newResult, matchContext)) {
    if (numberOfTrees >= maxNumberOfTrees) {
      return false;
    }
    source = source->nextTree();
    numberOfTrees++;
    // Reset newResult
    newResult = *context;
    newResult.setNumberOfTrees(tag, numberOfTrees);
  }
  *context = newResult;
  return true;
}

// Only additions and multiplications can be squashed.
bool CannotBeSquashed(const Tree* pattern) {
  return !pattern->isAddition() && !pattern->isMultiplication();
}

bool PatternMatching::MatchSourceWithSquashedPattern(const Tree* source,
                                                     const Tree* pattern,
                                                     Context* context) {
  if (CannotBeSquashed(pattern)) {
    return false;
  }
  // If pattern requires more than 1 child, it can't be squashed.
  int minimalNumberOfChildren = 0;
  /* Single tree the pattern can be squashed to (if we need to match it with
   * source). */
  const Tree* ChildToCheck = nullptr;
  // Used to keep track of emptied placeholders.
  Context emptiedPlaceholders;

  for (const Tree* child : pattern->children()) {
    if (child->isPlaceholder()) {
      Placeholder::Tag tag = Placeholder::NodeToTag(child);
      Placeholder::Filter filter = Placeholder::NodeToFilter(child);
      if (context->getNode(tag)) {
        if (emptiedPlaceholders.getNode(tag)) {
          /* If an emptied placeholder is met more than once, unmark it so we
           * can't set it to one child later. */
          emptiedPlaceholders.setNode(tag, nullptr, 0, true);
        }
        int sourceNodeChildren = context->getNumberOfTrees(tag);
        minimalNumberOfChildren += sourceNodeChildren;
        if (sourceNodeChildren > 0) {
          ChildToCheck = child;
        }
      } else if (filter != Placeholder::Filter::ZeroOrMore) {
        minimalNumberOfChildren++;
        context->setNode(tag, source, 1,
                         filter == Placeholder::Filter::OneOrMore);
      } else {
        // Set unassigned ZeroOrMore placeholders to 0 children for now.
        context->setNode(tag, source, 0, true);
        emptiedPlaceholders.setNode(tag, source, 1, true);
      }
    } else {
      minimalNumberOfChildren++;
      ChildToCheck = child;
    }
    if (minimalNumberOfChildren > 1) {
      // Too many children, pattern can't be squashed.
      return false;
    }
  }
  if (minimalNumberOfChildren == 0) {
    // All pattern children are empty placeholders.
    assert(!ChildToCheck);
    // Find a placeholder that have been emptied here, and met only once.
    /* TODO: An arbitrary tag is being chosen here. Multiple solutions could
     *       be tried. */
    Placeholder::Tag emptiedPlaceholderTag = Placeholder::Tag::NumberOfTags;
    for (uint8_t i = 0; i < Placeholder::Tag::NumberOfTags; i++) {
      if (emptiedPlaceholders.getNode(i)) {
        emptiedPlaceholderTag = static_cast<Placeholder::Tag>(i);
      }
    }
    if (emptiedPlaceholderTag == Placeholder::Tag::NumberOfTags) {
      // All placeholders were already empty, pattern can be matched with
      // addition/multiplication neutral only.
      return ((pattern->isAddition() && source->isZero()) ||
              (pattern->isMultiplication() && source->isOne()));
    }
    // Update the emptied placeholder to hold the source as single child instead
    /* TODO: If source is addition/multiplication neutral, leaving the
     *       placeholder empty could also work. */
    context->setNode(emptiedPlaceholderTag, source, 1, true);
  }
  /* Since the parent context in source isn't an NAry, we avoid using MatchNodes
   * with unmatched placeholders. This is why we already matched them (and
   * updated context). Otherwise, we still need to check that source match the
   * squashed pattern. */
  return ChildToCheck ? MatchNodes(source, ChildToCheck, context,
                                   MatchContext(source, ChildToCheck))
                      : true;
}

bool PatternMatching::MatchNodes(const Tree* source, const Tree* pattern,
                                 Context* context, MatchContext matchContext) {
  while (!matchContext.reachedLimit(pattern, true, false)) {
    bool onlyEmptyPlaceholders = false;
    if (matchContext.reachedLimit(pattern, false, false)) {
      // Local pattern has been entirely matched.
      if (!matchContext.reachedLimit(source, false, true)) {
        // Local source has not been entirely checked.
        return false;
      }
      // Update the local pattern.
      matchContext.setLocalToParent();
      // Continue in case parents also reached limits.
      continue;
    }
    /* If source has been entirely checked but pattern nodes are remaining it
     * can only be empty tree placeholders. */
    onlyEmptyPlaceholders = matchContext.reachedLimit(source, false, true);

    if (pattern->isPlaceholder()) {
      Placeholder::Tag tag = Placeholder::NodeToTag(pattern);
      const Tree* tagNode = context->getNode(tag);
      if (tagNode) {
        // AnyTrees status should be preserved if the Placeholder is reused.
        assert(context->isAnyTree(tag) == (Placeholder::NodeToFilter(pattern) !=
                                           Placeholder::Filter::One));
        // Tag has already been set. Check the trees are the same.
        for (int i = 0; i < context->getNumberOfTrees(tag); i++) {
          if (onlyEmptyPlaceholders || !tagNode->treeIsIdenticalTo(source)) {
            return false;
          }
          tagNode = tagNode->nextTree();
          source = source->nextTree();
        }
      } else if (onlyEmptyPlaceholders && Placeholder::NodeToFilter(pattern) !=
                                              Placeholder::Filter::ZeroOrMore) {
        return false;
      } else if (Placeholder::NodeToFilter(pattern) !=
                 Placeholder::Filter::One) {
        /* MatchAnyTrees will try to absorb consecutive trees in this
         * placeholder (as little as possible) and match the rest of the nodes.
         */
        return MatchAnyTrees(tag, source, pattern, context, matchContext);
      } else {
        // Set the tag to source's tree
        context->setNode(tag, source, 1, false);
        source = source->nextTree();
      }
      pattern = pattern->nextNode();
      continue;
    }
    if (onlyEmptyPlaceholders) {
      // pattern's node cannot be an empty placeholder.
      return false;
    }
    /* AnyTrees placeholders are expected among children of simple NArys.
     * The number of children is therefore not expected to match. */
    bool simpleNAryMatch =
        source->isSimpleNAry() && pattern->type() == source->type();
    if (simpleNAryMatch || source->nodeIsIdenticalTo(pattern)) {
      if (source->numberOfChildren() > 0) {
        // Set the new local context so that AnyTrees placeholder cannot match
        // consecutive Trees inside and outside this node.
        matchContext.setLocal(source, pattern);
      }
      source = source->nextNode();
      pattern = pattern->nextNode();
      continue;
    }
    // Try to squash the pattern: KMult(KA_p) may match with a single tree.
    if (!MatchSourceWithSquashedPattern(source, pattern, context)) {
      // Tree* cannot match exactly.
      return false;
    }
    source = source->nextTree();
    pattern = pattern->nextTree();
  }
  /* Pattern has been entirely and successfully matched.
   * Return false if source has not been entirely checked. */
  return matchContext.reachedLimit(source, true, true);
}

/* Since we use Match a lot with pattern's type not matching source, escape
 * early here to optimize a costly MatchContext constructor. */
bool CanEarlyEscape(const Tree* pattern, const Tree* source) {
  if (pattern->type() == source->type() || pattern->isPlaceholder()) {
    return false;
  }
  if (CannotBeSquashed(pattern)) {
    return true;
  }
  // We can still match x wit KA_s*KB
  int minimalNumberOfChildren = 0;
  for (const Tree* child : pattern->children()) {
    if (child->isPlaceholder() &&
        (Placeholder::NodeToFilter(child) == Placeholder::Filter::ZeroOrMore)) {
      continue;
    }
    minimalNumberOfChildren++;
    if (minimalNumberOfChildren > 1) {
      return true;
    }
  }
  return false;
}

bool PatternMatching::Match(const Tree* pattern, const Tree* source,
                            Context* context) {
  if (CanEarlyEscape(pattern, source)) {
    return false;
  }
  // Use a temporary context to preserve context in case no match is found.
  Context tempContext = *context;
  // Match nodes between the tree's bounds.
  bool success =
      MatchNodes(source, pattern, &tempContext, MatchContext(source, pattern));
  if (success) {
    *context = tempContext;
  }
  return success;
}

Tree* PatternMatching::CreateTree(const Tree* structure, const Context context,
                                  Tree* insertedNAry, bool simplify) {
  Tree* top = Tree::FromBlocks(SharedEditionPool->lastBlock());
  const Block* lastStructureBlock = structure->nextTree()->block();
  const bool withinNAry = insertedNAry != nullptr;
  // Skip NAry structure node because it has already been inserted.
  const Tree* node = withinNAry ? structure->nextNode() : structure;
  while (node->block() < lastStructureBlock) {
    if (!node->isPlaceholder()) {
      int numberOfChildren = node->numberOfChildren();
      if (node->isSimpleNAry()) {
        /* Insert the entire tree recursively so that its number of children can
         * be updated. */
        Tree* insertedNode = SharedEditionPool->clone(node, false);
        /* Use node and not node->nextNode() so that lastStructureBlock can be
         * computed in CreateTree. */
        CreateTree(node, context, insertedNode, simplify);
        if (simplify) {
          Simplification::ShallowSystematicReduce(insertedNode);
        } else {
          // TODO proper fix
          if (!insertedNode->isSet()) {
            NAry::Sanitize(insertedNode);
          }
        }
        node = node->nextTree();
      } else if (withinNAry && numberOfChildren > 0) {
        // Insert the tree recursively to locally remove insertedNAry
        CreateTree(node, context, nullptr, simplify);
        node = node->nextTree();
      } else {
        Tree* result = SharedEditionPool->clone(node, false);
        node = node->nextNode();
        if (simplify) {
          for (size_t i = 0; i < numberOfChildren; i++) {
            CreateTree(node, context, nullptr, simplify);
            node = node->nextTree();
          }
          Simplification::ShallowSystematicReduce(result);
        }
      }
      continue;
    }
    Placeholder::Tag tag = Placeholder::NodeToTag(node);
    const Tree* nodeToInsert = context.getNode(tag);
    int treesToInsert = context.getNumberOfTrees(tag);
    // nodeToInsert must be initialized even with 0 treesToInsert
    assert(nodeToInsert && treesToInsert >= 0);
    // Created tree must match AnyTrees status of the Placeholder used to match
    assert(context.isAnyTree(tag) ==
           (Placeholder::NodeToFilter(node) != Placeholder::Filter::One));
    /* AnyTreesPlaceholders trees can only be inserted into simple nArys, even
     * with 1 treesToInsert. */
    assert(!(context.isAnyTree(tag) && !withinNAry));
    if (treesToInsert == 0) {
      /* Insert nothing and decrement the number of children which accounted for
       * the empty placeholder. */
      NAry::SetNumberOfChildren(insertedNAry,
                                insertedNAry->numberOfChildren() - 1);
      // Since withinNAry is true, insertedNAry will be sanitized afterward
      node = node->nextNode();
      continue;
    }
    if (treesToInsert > 1) {
      NAry::SetNumberOfChildren(
          insertedNAry, insertedNAry->numberOfChildren() + treesToInsert - 1);
      // Since withinNAry is true, insertedNAry will be sanitized afterward
      for (int i = 0; i < treesToInsert - 1; i++) {
        Tree* inserted = SharedEditionPool->clone(nodeToInsert, true);
        assert(!(simplify && Simplification::DeepSystematicReduce(inserted)));
        nodeToInsert = nodeToInsert->nextTree();
      }
    }
    Tree* inserted = SharedEditionPool->clone(nodeToInsert, true);
    assert(!(simplify && Simplification::DeepSystematicReduce(inserted)));
    node = node->nextNode();
  }
  return top;
}

Tree* PatternMatching::MatchAndCreate(const Tree* source, const Tree* pattern,
                                      const Tree* structure) {
  Context ctx;
  if (!Match(pattern, source, &ctx)) {
    return nullptr;
  }
  return Create(structure, ctx);
}

bool PatternMatching::PrivateMatchAndReplace(Tree* node, const Tree* pattern,
                                             const Tree* structure,
                                             bool simplify) {
  /* TODO: When possible this could be optimized by deleting all non-placeholder
   * pattern nodes and then inserting all the non-placeholder structure nodes.
   * For example : Pattern : +{4} A 1 B C A     Structure : *{4} 2 B A A
   *                                                EditionPool : +{4} x 1 y z x
   * 1 - Only keep structure's matched placeholders
   *                                                EditionPool : y x
   * 2 - Insert structure Nodes
   *                                                EditionPool : *{4} 2 y x A
   * 3 - Replace duplicated placeholders
   *                                                EditionPool : *{4} 2 y x x
   *
   * Some difficulties:
   *  - Detect if it is possible : BA->AB isn't but ABCBA->BCA is.
   *  - Handle PlaceHolder's CreateFilter such as FirstChild.
   *  - Implement a method allowing the insertion of uncompleted nodes :
   *      void insert(Block * startSrc, Block * endSrc, Block * dst)
   */

  Context ctx;
  // Escape case for full matches like A -> cos(A)
  if (pattern->isPlaceholder()) {
    ctx.setNode(Placeholder::NodeToTag(pattern), node, 1, false);
    node->moveTreeOverTree(Create(structure, ctx, simplify));
    return true;
  }

  // Step 1 - Match the pattern
  if (!Match(pattern, node, &ctx)) {
    return false;
  }
  /* Following this example :
   * this (EditionReference): (x + y) * z
   * pattern: (A + B) * C
   * structure: A * C + B * C
   *
   * EditionPool: ..... | *{2} +{2} x y z | ....
   * With :
   * - | delimiting this reference
   * - *{2} a two children multiplication node
   * - +{2} a two children addition
   * - _{2} a two children systemList */

  // Step 2 - Detach placeholder matches
  /* Create ZeroBlock for each context node to be detached so that tree size is
   * preserved. */
  EditionReference treeNext = node->nextTree();
  int initializedPlaceHolders = 0;
  EditionReference placeholders[Placeholder::Tag::NumberOfTags];
  for (uint8_t i = 0; i < Placeholder::Tag::NumberOfTags; i++) {
    if (!ctx.getNode(i)) {
      continue;
    }
    for (int j = 0; j < ctx.getNumberOfTrees(i); j++) {
      initializedPlaceHolders++;
      treeNext->cloneTreeBeforeNode(0_e);
    }
    // Keep track of placeholder matches before detaching them
    int numberOfTrees = ctx.getNumberOfTrees(i);
    if (!ctx.getNode(i)) {
      placeholders[i] = EditionReference();
    } else if (numberOfTrees == 0) {
      // Use the last block so that placeholders[i] stays initialized
      placeholders[i] = EditionReference(SharedEditionPool->lastBlock());
    } else {
      // the context is known to point on non const parts of the source
      placeholders[i] = EditionReference(const_cast<Tree*>(ctx.getNode(i)));
    }
    // Invalidate context before anything is detached.
    ctx.setNode(i, nullptr, numberOfTrees, ctx.isAnyTree(i));
  }

  // EditionPool: ..... | *{2} +{2} x y z | 0 0 0 ....

  // Detach placeholder matches at the end of the EditionPool in a system list
  EditionReference placeholderMatches(
      SharedEditionPool->push<Type::List>(initializedPlaceHolders));

  // EditionPool: ..... | *{2} +{2} x y z | 0 0 0 .... _{3}

  for (uint8_t i = 0; i < Placeholder::Tag::NumberOfTags; i++) {
    if (placeholders[i].isUninitialized()) {
      continue;
    }
    // Get a Tree to the first placeholder tree, and detach as many as necessary
    Tree* trees = Tree::FromBlocks(placeholders[i]->block());
    for (int j = 0; j < ctx.getNumberOfTrees(i); j++) {
      if (j == 0) {
        placeholders[i] = trees->detachTree();
      } else {
        trees->detachTree();
      }
    }
  }

  // EditionPool: ..... | *{2} +{2} 0 0 0 | .... _{3} x y z

  // Step 3 - Replace with placeholder matches only
  node->moveTreeOverTree(placeholderMatches);

  // EditionPool: ..... | _{3} x y z | ....

  // Step 4 - Update context with new placeholder matches position
  for (uint8_t i = 0; i < Placeholder::Tag::NumberOfTags; i++) {
    if (!placeholders[i].isUninitialized()) {
      ctx.setNode(i, placeholders[i], ctx.getNumberOfTrees(i),
                  ctx.isAnyTree(i));
    }
  }

  // Step 5 - Build the PatternMatching replacement
  Tree* created = Create(structure, ctx, simplify);

  // EditionPool: ..... | _{3} x y z | .... +{2} *{2} x z *{2} y z

  // Step 6 - Replace with created structure
  node->moveTreeOverTree(created);

  // EditionPool: ..... | +{2} *{2} x z *{2} y z | ....
  return true;
}

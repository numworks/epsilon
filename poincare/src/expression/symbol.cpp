#include "symbol.h"

#include <poincare/sign.h>
#include <poincare/src/memory/tree_stack.h>
#include <string.h>

#include <algorithm>

namespace Poincare::Internal {

char* Symbol::CopyName(const Tree* e, char* buffer, size_t bufferSize) {
  assert(e->isUserNamed());
  size_t length = Length(e);
  assert(GetName(e)[length] == 0);
  return buffer + strlcpy(buffer, GetName(e), std::min(bufferSize, length + 1));
}

const char* Symbol::GetName(const Tree* e) {
  assert(e->isUserNamed());
  // BlockType, Size, Name
  return reinterpret_cast<const char*>(e->block()->nextNth(2));
}

ComplexSign Symbol::GetComplexSign(const Tree* e) {
  /* Undefined global variables are considered scalar (and real finite for
   * UserSymbols) */
  assert(e->isUserNamed());
  return e->isUserSymbol() ? ComplexSign::RealFinite() : ComplexSign::Unknown();
}

bool involvesCircularity(const Tree* e, Poincare::Context* context,
                         int maxDepth, const char** visitedSymbols,
                         int numberOfVisitedSymbols) {
  // Check for circularity only when a symbol/function is encountered
  if (!e->isUserFunction() && !e->isUserSymbol()) {
    for (const Tree* child : e->children()) {
      if (involvesCircularity(child, context, maxDepth, visitedSymbols,
                              numberOfVisitedSymbols)) {
        return true;
      }
    }
    return false;
  }
  const char* name = Symbol::GetName(e);

  // Check if this symbol has already been visited.
  for (int i = 0; i < numberOfVisitedSymbols; i++) {
    if (strcmp(name, visitedSymbols[i]) == 0) {
      return true;
    }
  }

  // Check children of this (useful for function parameters)
  for (const Tree* child : e->children()) {
    if (involvesCircularity(child, context, maxDepth, visitedSymbols,
                            numberOfVisitedSymbols)) {
      return true;
    }
  }

  // Check for circularity in the expression of the symbol and decrease depth
  maxDepth--;
  if (maxDepth < 0) {
    /* We went too deep into the check and consider the expression to be
     * circular. */
    return true;
  }
  visitedSymbols[numberOfVisitedSymbols] = name;
  numberOfVisitedSymbols++;

  Tree* symbol = Tree::FromBlocks(SharedTreeStack->lastBlock());
  if (e->isUserFunction()) {
    // This is like cloning, but without the symbol.
    e->cloneNode();
    KUnknownSymbol->cloneTree();
  } else {
    e->cloneTree();
  }

  const Tree* definition =
      context ? context->expressionForSymbolAbstract(symbol) : nullptr;
  bool isCircular =
      definition && involvesCircularity(definition, context, maxDepth,
                                        visitedSymbols, numberOfVisitedSymbols);
  symbol->removeTree();
  return isCircular;
}

bool Symbol::InvolvesCircularity(const Tree* e, Poincare::Context* context) {
  const char* visitedSymbols[k_maxSymbolReplacementsCount];
  if (involvesCircularity(e, context, k_maxSymbolReplacementsCount,
                          visitedSymbols, 0)) {
    return true;
  }
  return false;
}

}  // namespace Poincare::Internal

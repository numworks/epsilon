#include "symbol_store.h"

#include <poincare/context.h>
#include <poincare/expression.h>
#include <poincare/helpers/store.h>
#include <poincare/src/expression/symbol.h>
#include <poincare/src/layout/parser.h>
#include <poincare/src/memory/tree.h>
#include <string.h>

namespace PoincareTest {
using Tree = Poincare::Internal::Tree;

char SymbolNameFromString(const char* symbolString) {
  // Only one character symbols are allowed
  assert(strlen(symbolString) == 1);
  return symbolString[0];
}

char SymbolNameFromTree(const Tree* symbolTree) {
  return SymbolNameFromString(Poincare::Internal::Symbol::GetName(symbolTree));
}

void store(const char* storeExpression,
           Poincare::VariableStore& variableStore) {
  Poincare::UserExpression storeUserExpression =
      Poincare::UserExpression::Parse(storeExpression, variableStore);
  variableStore.setExpressionForUserNamed(
      Poincare::StoreHelper::Value(storeUserExpression),
      Poincare::StoreHelper::Symbol(storeUserExpression));
}

SymbolStore::SymbolWithExpression::SymbolWithExpression(
    char name, Poincare::Context::UserNamedType type, const Tree* e)
    : m_name{name}, m_type{type} {
  assert(e->treeSize() <= k_expressionBufferSize);
  e->copyTreeTo(m_expressionBuffer.data());
}

const Poincare::Internal::Tree* SymbolStore::SymbolWithExpression::expression()
    const {
  return Tree::FromBlocks(m_expressionBuffer.data());
}

const Tree* SymbolStore::expressionForUserNamed(const Tree* symbol) const {
  char symbolName = SymbolNameFromTree(symbol);
  for (const SymbolWithExpression& element : m_symbolTable) {
    if (element.name() == symbolName) {
      return element.expression();
    }
  }
  return nullptr;
}

Poincare::Context::UserNamedType SymbolStore::expressionTypeForIdentifier(
    const char* identifier, int length) const {
  char symbolName = SymbolNameFromString(identifier);
  for (const SymbolWithExpression& element : m_symbolTable) {
    if (element.name() == symbolName) {
      return element.type();
    }
  }
  return UserNamedType::None;
}

bool SymbolStore::setExpressionForUserNamed(const Tree* expression,
                                            const Tree* symbol) {
  m_symbolTable.push(SymbolWithExpression(SymbolNameFromTree(symbol),
                                          UserNamedType::Symbol, expression));
  return true;
}

}  // namespace PoincareTest

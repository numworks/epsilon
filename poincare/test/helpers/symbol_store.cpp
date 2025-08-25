#include "symbol_store.h"

#include <omg/string.h>
#include <poincare/context.h>
#include <poincare/helpers/store.h>
#include <poincare/src/expression/k_tree.h>
#include <poincare/src/expression/symbol.h>
#include <poincare/src/expression/variables.h>
#include <poincare/src/layout/parser.h>
#include <poincare/src/memory/tree.h>

#include <algorithm>
#include <string_view>

namespace PoincareTest {
using Tree = Poincare::Internal::Tree;

std::string_view SymbolNameFromTree(const Tree* symbolTree) {
  return std::string_view(Poincare::Internal::Symbol::GetName(symbolTree));
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
    std::string_view name, Poincare::Context::UserNamedType type, const Tree* e)
    : m_type{type} {
  assert(name.size() <= k_maxNameSize);
  m_name = name;
  assert(e->treeSize() <= k_expressionBufferSize);
  e->copyTreeTo(m_expressionBuffer.data());
}

const Poincare::Internal::Tree* SymbolStore::SymbolWithExpression::expression()
    const {
  return Tree::FromBlocks(m_expressionBuffer.data());
}

const Tree* SymbolStore::expressionForUserNamed(const Tree* symbol) const {
  const SymbolWithExpression* existingSymbol =
      findSymbolInStore(SymbolNameFromTree(symbol));
  if (!existingSymbol) {
    return nullptr;
  }
  return existingSymbol->expression();
}

Poincare::Context::UserNamedType SymbolStore::expressionTypeForIdentifier(
    std::string_view identifier) const {
  const SymbolWithExpression* existingSymbol = findSymbolInStore(identifier);
  if (!existingSymbol) {
    return UserNamedType::None;
  }
  return existingSymbol->type();
}

const SymbolStore::SymbolWithExpression* SymbolStore::findSymbolInStore(
    std::string_view symbolName) const {
  const SymbolWithExpression* result =
      std::find_if(m_symbolTable.begin(), m_symbolTable.end(),
                   [symbolName](const SymbolWithExpression& storedSymbol) {
                     return symbolName == storedSymbol.name();
                   });
  if (result == m_symbolTable.end()) {
    return nullptr;
  }
  return result;
}

SymbolStore::SymbolWithExpression* SymbolStore::findSymbolInStore(
    std::string_view symbolName) {
  SymbolWithExpression* result =
      std::find_if(m_symbolTable.begin(), m_symbolTable.end(),
                   [symbolName](const SymbolWithExpression& storedSymbol) {
                     return symbolName == storedSymbol.name();
                   });
  if (result == m_symbolTable.end()) {
    return nullptr;
  }
  return result;
}

bool SymbolStore::push(const Tree* expression, std::string_view symbolName,
                       UserNamedType symbolType) {
  SymbolWithExpression* existingSymbol = findSymbolInStore(symbolName);
  if (existingSymbol) {
    *existingSymbol = SymbolWithExpression(symbolName, symbolType, expression);
    return true;
  }
  m_symbolTable.push(SymbolWithExpression(symbolName, symbolType, expression));
  return true;
}

bool SymbolStore::setExpressionForUserSymbol(const Tree* expression,
                                             std::string_view symbolName) {
  UserNamedType symbolType = UserNamedType::Symbol;
  if (expression->isList()) {
    symbolType = UserNamedType::List;
  }
  return push(expression, symbolName, symbolType);
}

bool SymbolStore::setExpressionForUserFunctionOrSequence(
    const Poincare::Internal::Tree* expression,
    const Poincare::Internal::Tree* functionalSymbol) {
  assert(functionalSymbol->isUserFunction() ||
         functionalSymbol->isUserSequence());
  const Tree* variableSymbol = functionalSymbol->child(0);
  assert(variableSymbol->isUserSymbol());
  // Replace the user symbol with KUnknown in the expression
  Tree* storedTree = expression->cloneTree();
  Poincare::Internal::Variables::ReplaceSymbolWithTree(
      storedTree, variableSymbol, Poincare::Internal::KUnknownSymbol);

  bool success =
      push(storedTree, SymbolNameFromTree(functionalSymbol),
           functionalSymbol->isUserFunction() ? UserNamedType::Function
                                              : UserNamedType::Sequence);
  storedTree->removeTree();
  return success;
}

bool SymbolStore::setExpressionForUserNamed(const Tree* expression,
                                            const Tree* symbol) {
  assert(symbol->isUserNamed());
  if (symbol->isUserSymbol()) {
    return setExpressionForUserSymbol(expression, SymbolNameFromTree(symbol));
  }

  return setExpressionForUserFunctionOrSequence(expression, symbol);
}

}  // namespace PoincareTest

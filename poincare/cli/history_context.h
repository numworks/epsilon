#pragma once

#include <poincare/old/context.h>
#include <poincare/src/expression/symbol.h>
#include <poincare/src/memory/tree.h>

#include <string>
#include <vector>

#include "commands.h"

using namespace Poincare::Internal;

// Save trees of previous outputs o0, o1...
class HistoryContext : public Poincare::Context {
 public:
  int addOutput(const Tree* tree) {
    int n = tree->treeSize();
    std::vector<Block> item(n);
    std::memcpy(item.data(), tree->block(), n);
    m_history.push_back(item);
    return m_history.size() - 1;
  }

  UserNamedType expressionTypeForIdentifier(const char* identifier,
                                            int length) override {
    if (!s_isInteractive) {
      return UserNamedType::None;
    }
    if (length < 2 || identifier[0] != 'o') {
      return UserNamedType::None;
    }
    return UserNamedType::Symbol;
  }

  const Tree* expressionForUserNamed(const Tree* symbol) override {
    if (!s_isInteractive) {
      return nullptr;
    }
    if (!symbol->isUserSymbol()) {
      return nullptr;
    }
    const char* name = Symbol::GetName(symbol);
    if (name[0] != 'o' || strlen(name) < 2) {
      return nullptr;
    }
    int n = std::stoi(name + 1);
    if (n < 0 || n >= m_history.size()) {
      return nullptr;
    }
    return Tree::FromBlocks(m_history[n].data());
  }

  bool setExpressionForUserNamed(const Tree* expression,
                                 const Tree* symbol) override {
    return false;
  }

 private:
  std::vector<std::vector<Block>> m_history;
};

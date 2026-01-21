#pragma once

#include <poincare/src/expression/symbol.h>
#include <poincare/src/memory/tree.h>
#include <poincare/symbol_context.h>

#include <string>
#include <vector>

#include "commands.h"

using namespace Poincare::Internal;

// Save trees of previous outputs o0, o1...
class HistoryContext : public Poincare::SymbolContext {
 public:
  int addOutput(const Tree* tree) {
    int n = tree->treeSize();
    std::vector<Block> item(n);
    std::memcpy(item.data(), tree->block(), n);
    m_history.push_back(item);
    return m_history.size() - 1;
  }

  SymbolType symbolTypeForIdentifier(
      std::string_view identifier) const override {
    if (!s_isInteractive) {
      return SymbolType::None;
    }
    if (identifier.length() < 2 || identifier[0] != 'o') {
      return SymbolType::None;
    }
    return SymbolType::Symbol;
  }

  const Tree* expressionForSymbol(const Tree* symbol) const override {
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

 private:
  std::vector<std::vector<Block>> m_history;
};

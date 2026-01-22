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

 protected:
  Poincare::SymbolContext::SymbolDescription symbolDescriptionForIdentifier(
      std::string_view identifier) const override {
    if (!s_isInteractive || identifier.length() < 2 || identifier[0] != 'o') {
      return {};
    }
    // Skip first character 'o'
    std::string indexStr(identifier.substr(1));
    int n = std::stoi(indexStr);
    if (n < 0 || n >= m_history.size()) {
      return {};
    }
    return {SymbolType::Symbol, Tree::FromBlocks(m_history[n].data())};
  }

 private:
  std::vector<std::vector<Block>> m_history;
};

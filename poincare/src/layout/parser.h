#pragma once

#include <poincare/helpers/parser.h>
#include <poincare/src/memory/tree.h>
#include <poincare/symbol_context.h>

#include "parsing/layout_parser.h"

namespace Poincare::Internal {

namespace Parser {

// Parses a top-level layout.
inline Tree* Parse(const Tree* l,
                   const SymbolContext& symbolContext = EmptySymbolContext{},
                   ParserHelper::ParsingParameters params = {}) {
  return LayoutParser::Parse(l, {.context = &symbolContext,
                                 .params = params,
                                 .metadata = {.isTopLevelRack = true}});
}

}  // namespace Parser

}  // namespace Poincare::Internal

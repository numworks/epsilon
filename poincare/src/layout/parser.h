#pragma once

#include <poincare/helpers/parser.h>
#include <poincare/src/memory/tree.h>
#include <poincare/symbol_context.h>

#include "parsing/layout_parser.h"

namespace Poincare::Internal {

namespace Parser {

/* Besides this entrypoint, the parser is divided into RackParser and
 * LayoutParser to follow the Rack/Layouts alternance. */

// Parse a top-level layout. Return nullptr if a syntax error is encountered.
Tree* Parse(const Tree* l,
            const SymbolContext& symbolContext = EmptySymbolContext{},
            ParserHelper::ParsingParameters params = {});

}  // namespace Parser

}  // namespace Poincare::Internal

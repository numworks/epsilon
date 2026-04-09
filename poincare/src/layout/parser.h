#pragma once

#include <poincare/helpers/parser.h>
#include <poincare/src/memory/tree.h>
#include <poincare/symbol_context.h>
#include <poincare/syntax_error.h>

namespace Poincare::Internal {

namespace Parser {

/* Besides this entrypoint, the parser is divided into RackParser and
 * LayoutParser to follow the Rack/Layouts alternance. */

struct Output {
  Tree* tree;
  SyntaxError error;
};

// Parse a top-level layout. Return nullptr if a syntax error is encountered.
Output Parse(const Tree* l,
             const SymbolContext& symbolContext = EmptySymbolContext{},
             ParserHelper::ParsingParameters params = {});
/* Use this function when the layout is guaranteed to be correct, syntax error
 * will not be handled, this cannot return nullptr. */
Tree* UnsafeParse(const Tree* l,
                  const SymbolContext& symbolContext = EmptySymbolContext{},
                  ParserHelper::ParsingParameters params = {});

}  // namespace Parser

}  // namespace Poincare::Internal

#ifndef POINCARE_LAYOUT_PARSER_H
#define POINCARE_LAYOUT_PARSER_H

#include <poincare/helpers/parser.h>
#include <poincare/old/context.h>
#include <poincare/src/memory/tree.h>
#include <poincare/src/memory/tree_ref.h>

#include "parsing/parsing_context.h"

namespace Poincare::Internal {

namespace Parser {

// This is called mainly internally by the parser
Tree* Parse(const Tree* l, ParsingContext parsingContext);

// Call this to parse a top-level layout
inline Tree* ParseTopLevel(const Tree* l, Context* context = nullptr,
                           ParserHelper::ParsingParameters params = {}) {
  return Parse(l, {.context = context,
                   .params = params,
                   .metadata = {.isTopLevelRack = true}});
}

}  // namespace Parser

}  // namespace Poincare::Internal

#endif

#ifndef POINCARE_LAYOUT_PARSER_H
#define POINCARE_LAYOUT_PARSER_H

#include <poincare/context.h>
#include <poincare/helpers/parser.h>
#include <poincare/src/memory/tree.h>

#include "parsing/layout_parser.h"

namespace Poincare::Internal {

namespace Parser {

// Parses a top-level layout.
inline Tree* Parse(const Tree* l, const Context& context = EmptyContext{},
                   ParserHelper::ParsingParameters params = {}) {
  return LayoutParser::Parse(l, {.context = &context,
                                 .params = params,
                                 .metadata = {.isTopLevelRack = true}});
}

}  // namespace Parser

}  // namespace Poincare::Internal

#endif

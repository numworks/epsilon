#ifndef POINCARE_LAYOUT_PARSER_H
#define POINCARE_LAYOUT_PARSER_H

#include <poincare/helpers/parser.h>
#include <poincare/old/context.h>
#include <poincare/src/memory/tree.h>
#include <poincare/src/memory/tree_ref.h>

#include "parsing/parsing_context.h"

namespace Poincare::Internal {

namespace Parser {

Tree* Parse(const Tree* l, ParsingContext parsingContext);

}  // namespace Parser

}  // namespace Poincare::Internal

#endif

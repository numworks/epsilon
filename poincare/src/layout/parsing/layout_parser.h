#pragma once

#include <poincare/src/memory/tree.h>

#include "parsing_context.h"

namespace Poincare::Internal::Parser {

namespace LayoutParser {

// Parse layout into an expression on the tree stack. Never return nullptr.
Tree* Parse(const Tree* l, ParsingContext parsingContext);

}  // namespace LayoutParser

}  // namespace Poincare::Internal::Parser

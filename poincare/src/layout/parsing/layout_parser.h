#pragma once

#include <poincare/src/memory/tree.h>

#include "parsing_context.h"

namespace Poincare::Internal {

namespace LayoutParser {

Tree* Parse(const Tree* l, ParsingContext parsingContext);

}  // namespace LayoutParser

}  // namespace Poincare::Internal

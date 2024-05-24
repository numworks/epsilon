#ifndef POINCARE_LAYOUT_PARSER_H
#define POINCARE_LAYOUT_PARSER_H

#include <poincare/old/context.h>
#include <poincare/src/memory/tree.h>
#include <poincare/src/memory/tree_ref.h>

#include "parsing/parsing_context.h"

namespace Poincare::Internal {

class Parser final {
 public:
  static Tree* Parse(const Tree* node, Poincare::Context* context,
                     ParsingContext::ParsingMethod method =
                         ParsingContext::ParsingMethod::Classic);
};

}  // namespace Poincare::Internal

#endif

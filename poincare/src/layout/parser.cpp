#include "parser.h"

#include <poincare/src/memory/tree_stack_checkpoint.h>

namespace Poincare::Internal::Parser {

Tree* Parse(const Tree* l, const SymbolContext& symbolContext,
            ParserHelper::ParsingParameters params) {
  assert(l->isRackLayout());
  ExceptionTry {
    return LayoutParser::Parse(l, {.context = &symbolContext,
                                   .params = params,
                                   .metadata = {.isTopLevelRack = true}});
  }
  ExceptionCatch(type) {
    if (type != ExceptionType::ParseFail) {
      TreeStackCheckpoint::Raise(type);
    }
  }
  return nullptr;
}

}  // namespace Poincare::Internal::Parser

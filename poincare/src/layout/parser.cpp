#include "parser.h"

#include <poincare/src/memory/tree_stack_checkpoint.h>

#include "parsing/rack_parser.h"

namespace Poincare::Internal::Parser {

Tree* Parse(const Tree* l, const SymbolContext& symbolContext,
            ParserHelper::ParsingParameters params) {
  SyntaxError syntaxError = SyntaxError::None;
  assert(l->isRackLayout());
  ExceptionTry {
    return RackParser(l, {.context = &symbolContext,
                          .params = params,
                          .syntaxError = &syntaxError})
        .parseTopLevelRack();
  }
  ExceptionCatch(type) {
    if (type != ExceptionType::ParseFail) {
      TreeStackCheckpoint::Raise(type);
    }
  }
  // Use syntaxError
  return nullptr;
}

}  // namespace Poincare::Internal::Parser

#include "parser.h"

#include <poincare/src/memory/tree_stack_checkpoint.h>

#include "parsing/rack_parser.h"

namespace Poincare::Internal::Parser {

Output Parse(const Tree* l, const SymbolContext& symbolContext,
             ParserHelper::ParsingParameters params) {
  volatile SyntaxError syntaxError = SyntaxError::None;
  assert(l->isRackLayout());
  ExceptionTry {
    Tree* tree = RackParser(l, {.context = &symbolContext,
                                .params = params,
                                .syntaxError = &syntaxError})
                     .parseTopLevelRack();
    assert(syntaxError == SyntaxError::None && tree);
    return {tree, SyntaxError::None};
  }
  ExceptionCatch(type) {
    if (type != ExceptionType::ParseFail) {
      TreeStackCheckpoint::Raise(type);
    }
  }
  assert(syntaxError != SyntaxError::None);
  return {nullptr, syntaxError};
}

}  // namespace Poincare::Internal::Parser

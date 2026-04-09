#include "parser.h"

#include <poincare/src/memory/tree_stack_checkpoint.h>

#include "parsing/rack_parser.h"

namespace Poincare::Internal::Parser {

Output Parse(const Tree* l, const SymbolContext& symbolContext,
             ParserHelper::ParsingParameters params) {
  SyntaxError syntaxError = SyntaxError::None;
  assert(l->isRackLayout());
  ExceptionTry {
    Tree* tree = RackParser(l, {.context = &symbolContext, .params = params})
                     .parseTopLevelRack();
    assert(syntaxError == SyntaxError::None && tree);
    return {tree, SyntaxError::None};
  }
  ExceptionCatchWithData(type, data) {
    if (type != ExceptionType::ParseFail) {
      TreeStackCheckpoint::Raise(type, data);
    }
    syntaxError = static_cast<SyntaxError>(data);
  }
  assert(syntaxError != SyntaxError::None);
  return {nullptr, syntaxError};
}

}  // namespace Poincare::Internal::Parser

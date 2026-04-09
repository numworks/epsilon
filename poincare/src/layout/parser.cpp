#include "parser.h"

#include <omg/unreachable.h>
#include <poincare/src/memory/tree_stack_checkpoint.h>

#include "parsing/rack_parser.h"

namespace Poincare::Internal::Parser {

Output Parse(const Tree* l, const SymbolContext& symbolContext,
             ParserHelper::ParsingParameters params) {
  ExceptionTry {
    return {UnsafeParse(l, symbolContext, params), SyntaxError::None};
  }
  ExceptionCatchWithData(type, data) {
    if (type != ExceptionType::ParseFail) {
      TreeStackCheckpoint::Raise(type, data);
    }
    SyntaxError error = static_cast<SyntaxError>(data);
    assert(error != SyntaxError::None);
    return {nullptr, static_cast<SyntaxError>(error)};
  }
  OMG_UNREACHABLE;
}

Tree* UnsafeParse(const Tree* l, const SymbolContext& symbolContext,
                  ParserHelper::ParsingParameters params) {
  assert(l->isRackLayout());
  Tree* tree = RackParser(l, {.context = &symbolContext, .params = params})
                   .parseTopLevelRack();
  assert(tree);
  return tree;
}

}  // namespace Poincare::Internal::Parser

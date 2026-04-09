#pragma once

#include <poincare/helpers/parser.h>
#include <poincare/symbol_context.h>

namespace Poincare::Internal {

/* These metadata are filled and used during parsing  */
struct ParsingMetadata {
  /* this is set to true when parsing the name of a var/function if isAssignment
   * is true in ParsingParameters. */
  bool isAssignmentDeclaration = false;
  /* isCommaSeparatedList: the current rack is a comma-separated list */
  bool isCommaSeparatedList = false;
  /* isUnitConversion: 3m is understood as 3meters even if there is a value
   * stored in the variable m */
  bool isUnitConversion = false;
  /* isImplicitAdditionBetweenUnits: 4h40min30s is understood as 4h+40min+30s
   * and not 4*h40*mi*n30*s */
  bool isImplicitAdditionBetweenUnits = false;
};

struct ParsingContext {
  /* The Parser uses a `const Context*` internally, because it can change the
   * Context that is currently used for parsing to another temporary Context.
   * Re-assigning to another value is possible with a pointer-to-const, but
   * would not be with a const reference, so we need a pointer-to-const in the
   * Parser. */
  const Poincare::SymbolContext* context = nullptr;
  const ParserHelper::ParsingParameters params = {};
  ParsingMetadata metadata = {};
  ParsingContext cloneWithoutMetadata() const {
    return ParsingContext{context, params, ParsingMetadata{}};
  }
};

}  // namespace Poincare::Internal

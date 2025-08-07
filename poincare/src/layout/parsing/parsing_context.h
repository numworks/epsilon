#ifndef POINCARE_LAYOUT_PARSING_CONTEXT_H
#define POINCARE_LAYOUT_PARSING_CONTEXT_H

#include <poincare/helpers/parser.h>
#include <poincare/old/context.h>

namespace Poincare::Internal {

/* These metadata are filled and used during parsing  */
struct ParsingMetadata {
  /* isTopLevelRack: true if the current rack is the top-level rack of the
   * layout */
  bool isTopLevelRack = false;
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
  const Poincare::Context* context = nullptr;
  const ParserHelper::ParsingParameters params = {};
  ParsingMetadata metadata = {};
  ParsingContext cloneWithoutMetadata() const {
    return ParsingContext{context, params, ParsingMetadata{}};
  }
};

}  // namespace Poincare::Internal

#endif

#include "parsing_helper.h"
#include "token.h"

namespace Poincare {

const Expression::FunctionHelper * const * ParsingHelper::GetReservedFunction(const char * name, size_t nameLength) {
  const Expression::FunctionHelper * const * reservedFunction = s_reservedFunctions;
  while (reservedFunction < s_reservedFunctionsUpperBound) {
    int nameDifference = Token::CompareNonNullTerminatedName(name, nameLength, (**reservedFunction).name());
    if (nameDifference == 0) {
      return reservedFunction;
    }
    if (nameDifference < 0) {
      break;
    }
    reservedFunction++;
  }
  return nullptr;
}

bool ParsingHelper::IsSpecialIdentifierName(const char * name, size_t nameLength) {
  for (int i = 0; i < k_numberOfSpecialIdentifiers; i++) {
    if (Token::CompareNonNullTerminatedName(name, nameLength, s_specialIdentifierNames[i]) == 0) {
      return true;
    }
  }
  return false;
}

}

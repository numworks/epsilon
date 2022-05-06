#include "helper.h"
#include "token.h"

namespace Poincare {

constexpr const Expression::FunctionHelper * ParsingHelper::s_reservedFunctions[];
constexpr const char * ParsingHelper::s_specialIdentifierNames[];

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
    int nameDifference = Token::CompareNonNullTerminatedName(name, nameLength, s_specialIdentifierNames[i]);
    if (nameDifference == 0) {
      return true;
    }
    if (nameDifference < 0) {
      break;
    }
  }
  return false;
}

}

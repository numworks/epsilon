#include "helper.h"

#include <omg/unicode_helper.h>
#include <omg/utf8_helper.h>
#include <poincare/src/expression/binary.h>

namespace Poincare::Internal {
bool ParsingHelper::IsLogicalOperator(LayoutSpan name,
                                      Token::Type* returnType) {
  if (CompareLayoutSpanWithNullTerminatedString(
          name, Binary::OperatorName(Type::LogicalNot)) == 0) {
    *returnType = Token::Type::Not;
    return true;
  }
  Type operatorType;
  if (Binary::IsBinaryLogicalOperator(name, &operatorType)) {
    switch (operatorType) {
      case Type::LogicalAnd:
        *returnType = Token::Type::And;
        break;
      case Type::LogicalOr:
        *returnType = Token::Type::Or;
        break;
      case Type::LogicalXor:
        *returnType = Token::Type::Xor;
        break;
      case Type::LogicalNand:
        *returnType = Token::Type::Nand;
        break;
      case Type::LogicalNor:
        *returnType = Token::Type::Nor;
        break;
      default:
        assert(false);
    }
    return true;
  }
  return false;
}

}  // namespace Poincare::Internal

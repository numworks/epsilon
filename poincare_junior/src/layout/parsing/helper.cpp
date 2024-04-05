#include "helper.h"

#include <ion/unicode/utf8_helper.h>
#include <omgpj/unicode_helper.h>
#include <poincare_junior/src/expression/binary.h>

namespace PoincareJ {
bool ParsingHelper::IsLogicalOperator(const CPL* name, size_t nameLength,
                                      Token::Type* returnType) {
  if (OMG::CompareCPLWithNullTerminatedString(
          name, nameLength, Binary::OperatorName(Type::LogicalNot)) == 0) {
    *returnType = Token::Type::Not;
    return true;
  }
  Type operatorType;
  if (Binary::IsBinaryLogicalOperator(name, nameLength, &operatorType)) {
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

}  // namespace PoincareJ

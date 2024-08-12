#include "helper.h"

#include <omg/unicode_helper.h>
#include <omg/utf8_helper.h>
#include <poincare/src/expression/binary.h>
#include <poincare/src/expression/builtin.h>
#include <poincare/src/expression/integer.h>

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

bool ParsingHelper::ExtractInteger(const Tree* e, int* value) {
  bool isOpposite = false;
  if (e->isOpposite()) {
    e = e->child(0);
    isOpposite = true;
  }
  if (e->isRational()) {
    IntegerHandler intHandler = Integer::Handler(e);
    if (intHandler.is<int>()) {
      *value = intHandler.to<int>() * (isOpposite ? -1 : 1);
      return true;
    }
  }
  return false;
}

const Builtin* ParsingHelper::GetInverseFunction(const Builtin* builtin) {
  switch (builtin->type()) {
    case Type::Cos:
      return Builtin::GetReservedFunction(Type::ACos);
    case Type::Sin:
      return Builtin::GetReservedFunction(Type::ASin);
    case Type::Tan:
      return Builtin::GetReservedFunction(Type::ATan);
    default:
      return nullptr;
  }
}

bool ParsingHelper::IsPowerableFunction(const Builtin* builtin) {
  // return TypeBlock::IsAnyTrigonometryFunction(builtin->type());
  return builtin->type().isAnyTrigonometryFunction();
}

}  // namespace Poincare::Internal

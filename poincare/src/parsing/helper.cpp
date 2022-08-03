#include "helper.h"
#include "token.h"

namespace Poincare {

const Expression::FunctionHelper * const * ParsingHelper::GetReservedFunction(const char * name, size_t nameLength) {
  const Expression::FunctionHelper * const * reservedFunction = s_reservedFunctions;
  while (reservedFunction < s_reservedFunctionsUpperBound) {
    int nameDifference = (**reservedFunction).aliasesList().maxDifferenceWith(name, nameLength);
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

const Expression::FunctionHelper * const * ParsingHelper::GetInverseFunction(const char * name, size_t nameLength) {
  const FunctionMapping * functionMapping = s_inverses;
  while (functionMapping < s_inverseFunctionsUpperBound) {
    int nameDifference = ((*functionMapping).mainFunction)->aliasesList().maxDifferenceWith(name, nameLength);
    if (nameDifference == 0) {
      return &(*functionMapping).inverseFunction;
    }
    if (nameDifference < 0) {
      break;
    }
    functionMapping++;
  }
  return nullptr;
}

bool ParsingHelper::IsSpecialIdentifierName(const char * name, size_t nameLength) {
  return SpecialIdentifierIndexForName(name, nameLength) >= 0;
}

bool ParsingHelper::IsLogicalOperator(const char * name, size_t nameLength, Token::Type * returnType) {
  // TODO: Factorize non null terminated comparison with AliasesList
  if (strncmp(name, NotOperatorNode::k_name, nameLength) == 0 && *(NotOperatorNode::k_name + nameLength + 1) == 0) {
    *returnType = Token::Not;
    return true;
  }
  BinaryLogicalOperatorNode::OperatorType operatorType;
  if (BinaryLogicalOperatorNode::IsBinaryLogicalOperator(name, nameLength, &operatorType)) {
    switch (operatorType) {
    case BinaryLogicalOperatorNode::OperatorType::And:
      *returnType = Token::And;
      break;
    case BinaryLogicalOperatorNode::OperatorType::Or:
      *returnType = Token::Or;
      break;
    case BinaryLogicalOperatorNode::OperatorType::Xor:
      *returnType = Token::Xor;
      break;
    case BinaryLogicalOperatorNode::OperatorType::Nand:
      *returnType = Token::Nand;
      break;
    default:
      assert(operatorType == BinaryLogicalOperatorNode::OperatorType::Nor);
      *returnType = Token::Nor;
    }
    return true;
  }
  return false;
}


bool ParsingHelper::IsParameteredExpression(const Expression::FunctionHelper * helper) {
  return helper == &Derivative::s_functionHelper
      || helper == &Derivative::s_functionHelperFirstOrder
      || helper == &Integral::s_functionHelper
      || helper == &ListSequence::s_functionHelper
      || helper == &Product::s_functionHelper
      /* The string "sum" will give off the ListSum function helper. Since we call IsParameteredExpression before parsing the parameters, we cannot distinguish between the two. We make sure in parser.cpp that being considered a parametered expression does not cause problems for the parsing of ListSum. */
      || helper == &ListSum::s_functionHelper;
}

bool ParsingHelper::IsSquarableFunction(const Expression::FunctionHelper * helper) {
  return helper == &ArcCosine::s_functionHelper
      || helper == &ArcCosecant::s_functionHelper
      || helper == &HyperbolicArcCosine::s_functionHelper
      || helper == &ArcCotangent::s_functionHelper
      || helper == &ArcSecant::s_functionHelper
      || helper == &ArcSine::s_functionHelper
      || helper == &ArcTangent::s_functionHelper
      || helper == &HyperbolicArcSine::s_functionHelper
      || helper == &HyperbolicArcTangent::s_functionHelper
      || helper == &Cosine::s_functionHelper
      || helper == &Cosecant::s_functionHelper
      || helper == &HyperbolicCosine::s_functionHelper
      || helper == &Cotangent::s_functionHelper
      || helper == &Secant::s_functionHelper
      || helper == &Sine::s_functionHelper
      || helper == &HyperbolicSine::s_functionHelper
      || helper == &Tangent::s_functionHelper
      || helper == &HyperbolicTangent::s_functionHelper;
}

const ParsingHelper::IdentifierBuilder ParsingHelper::GetIdentifierBuilder(const char * name, size_t nameLength) {
  int identifierIndex = SpecialIdentifierIndexForName(name, nameLength);
  assert(identifierIndex >= 0);
  return s_specialIdentifiers[identifierIndex].identifierBuilder;
}

int ParsingHelper::SpecialIdentifierIndexForName(const char * name, size_t nameLength) {
  for (int i = 0; i < k_numberOfSpecialIdentifiers; i++) {
    if (s_specialIdentifiers[i].identifierAliasesList.contains(name, nameLength)) {
      return i;
    }
  }
  return -1;
}

}
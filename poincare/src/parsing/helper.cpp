#include "helper.h"
#include "token.h"

namespace Poincare {

const Expression::FunctionHelper * const * ParsingHelper::GetReservedFunction(const char * name, size_t nameLength) {
  const Expression::FunctionHelper * const * reservedFunction = s_reservedFunctions;
  while (reservedFunction < s_reservedFunctionsUpperBound) {
    int nameDifference = (**reservedFunction).name().comparedWith(name, nameLength);
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
    int nameDifference = ((*functionMapping).mainFunction)->name().comparedWith(name, nameLength);
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
  for (int i = 0; i < k_numberOfSpecialIdentifiers; i++) {
    if (s_specialIdentifiers[i].identifierName.isAliasOf(name, nameLength)) {
      return true;
    }
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
  for (int i = 0; i < k_numberOfSpecialIdentifiers; i++) {
    if (s_specialIdentifiers[i].identifierName.isAliasOf(name, nameLength)) {
      return s_specialIdentifiers[i].identifierBuilder;
    }
  }
  assert(false);
  return s_specialIdentifiers[0].identifierBuilder; // silence compiler
}

}
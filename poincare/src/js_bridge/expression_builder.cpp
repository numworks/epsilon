
#include <emscripten/bind.h>
#include <emscripten/val.h>
#include <poincare/k_tree.h>
#include <poincare/old/empty_context.h>
#include <poincare/old/junior_expression.h>
#include <poincare/src/expression/rational.h>
#include <poincare/src/memory/pattern_matching.h>
#include <poincare/src/memory/tree_stack.h>
#include <poincare/src/memory/type_block.h>

#include <string>

#include "expression_types.h"

using namespace emscripten;
using namespace Poincare::Internal;

namespace Poincare::JSBridge {

// --- Simple builders (Int, Float, Rational, Latex) ---

TypedSystemExpression BuildSystemInt(int32_t value) {
  JuniorExpression result = JuniorExpression::Builder(value);
  return *reinterpret_cast<TypedSystemExpression*>(&result);
}

TypedUserExpression BuildUserInt(int32_t value) {
  JuniorExpression result = JuniorExpression::Builder(value);
  return *reinterpret_cast<TypedUserExpression*>(&result);
}

TypedSystemExpression BuildSystemFloat(double value) {
  JuniorExpression result = JuniorExpression::Builder<double>(value);
  return *reinterpret_cast<TypedSystemExpression*>(&result);
}

TypedUserExpression BuildUserFloat(double value) {
  JuniorExpression result = JuniorExpression::Builder<double>(value);
  return *reinterpret_cast<TypedUserExpression*>(&result);
}

TypedSystemExpression BuildSystemRational(int32_t numerator,
                                          int32_t denominator) {
  JuniorExpression result = JuniorExpression::Builder(
      Rational::Push(IntegerHandler(numerator), IntegerHandler(denominator)));
  return *reinterpret_cast<TypedSystemExpression*>(&result);
}

TypedUserExpression BuildUserRational(int32_t numerator, int32_t denominator) {
  JuniorExpression result = JuniorExpression::Builder(
      Rational::Push(IntegerHandler(numerator), IntegerHandler(denominator)));
  return *reinterpret_cast<TypedUserExpression*>(&result);
}

TypedUserExpression BuildFromLatexString(std::string latex) {
  EmptyContext context;
  JuniorExpression result =
      JuniorExpression::ParseLatex(latex.c_str(), &context);
  return *reinterpret_cast<TypedUserExpression*>(&result);
}

// --- Build from pattern ---

using CustomBuilder = bool (*)(const char* children, size_t childrenLength);

struct TreePatternBuilder {
  const char* identifier;
  Type type;
  CustomBuilder customBuilder = nullptr;
};

#define BUILDER(TYPE) \
  {                   \
#TYPE, Type::TYPE \
  }

// This list is filled with all the types in expression/types.h
constexpr TreePatternBuilder treePatternBuilders[] = {
    // 1 - Numbers
    // 1.1.a - Negative Rationals
    /* RationalNegBig -> Use BuildRational ✅ */
    /* RationalNegShort -> Use BuildRational ✅ */
    // 1.2 - Integers
    /* IntegerNegBig -> Use BuildInteger ✅ */
    /* IntegerNegShort -> Use BuildInteger ✅ */
    BUILDER(MinusOne),
    BUILDER(Zero),
    BUILDER(One),
    BUILDER(Two),
    /* IntegerPosShort -> Use BuildInteger ✅ */
    /* IntegerPosBig -> Use BuildInteger ✅ */
    // 1.1.b - Positive rationals
    BUILDER(Half),
    /* RationalPosShort -> Use BuildRational ✅ */
    /* RationalPosBig -> Use BuildRational ✅ */
    // 1.3 - Floats
    /* SingleFloat -> Not implemented ❌ */
    /* DoubleFloat -> Use BuildFloat ✅ */
    // 1.4 - Mathematical constants
    BUILDER(EulerE),
    BUILDER(Pi),

    // 2 - Order dependant expressions
    BUILDER(Mult),
    BUILDER(Pow),
    BUILDER(Add),
    /* UserSymbol -> Custom builder, see at end of the list ✅ */
    /* UserFunction -> Not implemented ❌ */
    /* UserSequence -> Not implemented ❌ */
    /* Random -> Not implemented ❌ */
    /* RandInt -> Not implemented ❌ */
    /* RandIntNoRep -> Not implemented ❌ */
    BUILDER(Cos),
    BUILDER(Tan),
    BUILDER(Sin),
    BUILDER(Trig),
    BUILDER(ACos),
    BUILDER(ATan),
    BUILDER(ASin),
    BUILDER(ATrig),
    BUILDER(ATanRad),
    BUILDER(Sec),
    BUILDER(Csc),
    BUILDER(Cot),
    BUILDER(ASec),
    BUILDER(ACsc),
    BUILDER(ACot),
    BUILDER(CosH),
    BUILDER(SinH),
    BUILDER(TanH),
    BUILDER(ArCosH),
    BUILDER(ArSinH),
    BUILDER(ArTanH),
    BUILDER(Ln),
    BUILDER(LnReal),
    BUILDER(Log),
    BUILDER(LogBase),

    // 3 - Other expressions in Alphabetic order
    BUILDER(Abs),
    BUILDER(Arg),
    BUILDER(Binomial),
    BUILDER(Ceil),
    BUILDER(ComplexI),
    BUILDER(Conj),
    BUILDER(Decimal),
    /* Distribution -> Not implemented ❌ */
    BUILDER(Div),
    BUILDER(Exp),
    BUILDER(Fact),
    BUILDER(Factor),
    BUILDER(Floor),
    BUILDER(Frac),
    BUILDER(GCD),
    BUILDER(Im),
    BUILDER(Inf),
    BUILDER(LCM),
    BUILDER(MixedFraction),
    BUILDER(Opposite),
    BUILDER(PercentSimple),
    BUILDER(PercentAddition),
    BUILDER(Permute),
    BUILDER(Polynomial),
    BUILDER(PowReal),
    BUILDER(Quo),
    BUILDER(Re),
    BUILDER(Rem),
    BUILDER(Round),
    BUILDER(Sign),
    BUILDER(Sqrt),
    BUILDER(Root),
    BUILDER(Sub),
    BUILDER(TrigDiff),
    /* Var -> Partially implemented, see at end of the list ✅ */

    // 4 - Parametric types
    BUILDER(Sum),
    BUILDER(Product),
    BUILDER(Diff),
    BUILDER(Integral),
    BUILDER(IntegralWithAlternatives),
    BUILDER(ListSequence),

    // 5 - Matrix and vector builtins
    BUILDER(Dot),
    BUILDER(Norm),
    BUILDER(Trace),
    BUILDER(Cross),
    BUILDER(Det),
    BUILDER(Dim),
    BUILDER(Identity),
    BUILDER(Inverse),
    BUILDER(Ref),
    BUILDER(Rref),
    BUILDER(Transpose),
    BUILDER(PowMatrix),
    /* Matrix -> Not implemented ❌ */

    // 6 - Lists
    BUILDER(List),
    BUILDER(ListSort),
    BUILDER(ListElement),
    BUILDER(ListSlice),
    BUILDER(Mean),
    BUILDER(StdDev),
    BUILDER(Median),
    BUILDER(Variance),
    BUILDER(SampleStdDev),
    BUILDER(Min),
    BUILDER(Max),
    BUILDER(ListSum),
    BUILDER(ListProduct),
    BUILDER(Point),

    // 7 - Booleans
    BUILDER(False),
    BUILDER(True),
    BUILDER(LogicalNot),
    BUILDER(LogicalAnd),
    BUILDER(LogicalOr),
    BUILDER(LogicalXor),
    BUILDER(LogicalNor),
    BUILDER(LogicalNand),
    BUILDER(Equal),
    BUILDER(NotEqual),
    BUILDER(Superior),
    BUILDER(Inferior),
    BUILDER(SuperiorEqual),
    BUILDER(InferiorEqual),

    // 8 - Units
    /* Unit -> Not implemented ❌ */
    /* PhysicalConstant -> Not implemented ❌ */

    // 9 - Order dependant expressions
    BUILDER(Piecewise),
    BUILDER(Dependency),
    BUILDER(Dependencies),
    BUILDER(Set),
    BUILDER(Parentheses),
    /* Empty -> Not implemented ❌ */

    // 10 - Undefined expressions
    BUILDER(NonReal),
    BUILDER(UndefZeroPowerZero),
    BUILDER(UndefZeroDivision),
    BUILDER(UndefUnhandled),
    BUILDER(UndefUnhandledDimension),
    BUILDER(UndefBadType),
    BUILDER(UndefOutOfDefinition),
    BUILDER(UndefNotDefined),
    BUILDER(UndefForbidden),
    BUILDER(Undef),

    // 11 - Operations on expressions
    BUILDER(Store),
    BUILDER(UnitConversion),
    BUILDER(SequenceExplicit),
    BUILDER(SequenceSingleRecurrence),
    BUILDER(SequenceDoubleRecurrence),
    /* PointOfInterest -> Not implemented ❌ */

    // 13 - Custom builders
    {"UserSymbol", Type::Undef,
     [](const char* children, size_t childrenLength) -> bool {
       if (!children || childrenLength < 1) {
         return false;
       }
       TreeStack::SharedTreeStack->pushUserSymbol(children, childrenLength + 1);
       return true;
     }},
    {"VarK", Type::Undef,
     [](const char* children, size_t childrenLength) -> bool {
       if (children) {
         return false;
       }
       KVarK->cloneTree();
       return true;
     }},
    {"VarX", Type::Undef,
     [](const char* children, size_t childrenLength) -> bool {
       if (children) {
         return false;
       }
       KVarX->cloneTree();
       return true;
     }},
};

TreePatternBuilder findTreePatternBuilder(const char* identifier,
                                          int identifierLength) {
  for (const TreePatternBuilder& treeBuilder : treePatternBuilders) {
    if (strncmp(treeBuilder.identifier, identifier, identifierLength) == 0) {
      return treeBuilder;
    }
  }
  return {"", Type::Undef};
}

Tree* buildTreeFromPattern(const char* buffer, const char* bufferEnd,
                           const TypedUserExpression* contextExprs,
                           size_t nContextExprs) {
  // Skip whitespaces
  while (*buffer == ' ' && buffer < bufferEnd) {
    buffer++;
  }

  if (bufferEnd - buffer < 1) {
    return nullptr;
  }

  TreeStack* stack = TreeStack::SharedTreeStack;
  Tree* result = reinterpret_cast<Tree*>(stack->lastBlock());

  // Detect the presence of children in parenthesis
  const char* childrenBuffer = buffer + 1;
  while (*childrenBuffer != '(' && childrenBuffer < bufferEnd) {
    childrenBuffer++;
  }

  bool hasChildren = *childrenBuffer == '(';

  // Count number of children
  int numberOfChildren = hasChildren && (*(childrenBuffer + 1) != ')');
  if (hasChildren) {
    if (*(bufferEnd - 1) != ')') {
      // Shoud end with ')'
      return nullptr;
    }

    int innerParenthesis = 0;
    for (const char* tempBuffer = childrenBuffer + 1;
         tempBuffer < bufferEnd - 1; tempBuffer++) {
      if (*tempBuffer == ')') {
        innerParenthesis--;
        if (innerParenthesis < 0) {
          return nullptr;
        }
      } else if (*tempBuffer == '(') {
        innerParenthesis++;
      } else if (*tempBuffer == ',' && innerParenthesis == 0) {
        numberOfChildren++;
      }
    }

    if (innerParenthesis != 0) {
      // Parenthesis are not balanced
      return nullptr;
    }
  }

  // Detect the identifier
  int identifierLength = childrenBuffer - buffer;

  // 1. Identifier is of the form K[0-9]
  if (identifierLength == 2 && *buffer == 'K' &&
      *(buffer + 1) - '0' < nContextExprs) {
    if (hasChildren) {
      return nullptr;
    }
    // Clone the context expression
    contextExprs[*(buffer + 1) - '0'].tree()->cloneTree();
    return result;
  }

  // 2. Identifier is a known tree builder
  TreePatternBuilder treeBuilder =
      findTreePatternBuilder(buffer, identifierLength);
  int len = strlen(treeBuilder.identifier);

  if (len == 0) {
    // Identifier not found
    return nullptr;
  }

  // 2.1. Use a custom builder
  if (treeBuilder.customBuilder != nullptr) {
    bool success = treeBuilder.customBuilder(childrenBuffer + 1,
                                             bufferEnd - childrenBuffer - 2);
    return success ? result : nullptr;
  }

  // 2.2. Use the default builder
  bool isNAry = TypeBlock::IsNAry(treeBuilder.type);
  if (!isNAry &&
      TypeBlock::NumberOfChildren(treeBuilder.type) != numberOfChildren) {
    // Invalid number of children
    return nullptr;
  }

  stack->pushBlock(treeBuilder.type);

  if (isNAry) {
    if (!hasChildren) {
      stack->flushFromBlock(result);
      return nullptr;
    }
    stack->pushBlock(numberOfChildren);
  }

  if (numberOfChildren == 0) {
    return result;
  }

  // 3. Parse children
  int innerParenthesis = 0;
  const char* childStart = childrenBuffer + 1;
  for (const char* tempBuffer = childStart; tempBuffer < bufferEnd;
       tempBuffer++) {
    if ((innerParenthesis == 0 && *tempBuffer == ',') ||
        tempBuffer == bufferEnd - 1) {
      Tree* childTree = buildTreeFromPattern(childStart, tempBuffer,
                                             contextExprs, nContextExprs);
      if (!childTree) {
        return nullptr;
      }
      childStart = tempBuffer + 1;
      continue;
    }
    if (*tempBuffer == ')') {
      innerParenthesis--;
    } else if (*tempBuffer == '(') {
      innerParenthesis++;
    }
  }

  return result;
}

TypedUserExpression BuildFromPatternImpl(
    std::string pattern, const TypedUserExpression* contextExprs,
    size_t nContextExprs) {
  const char* buffer = pattern.c_str();
  const char* bufferEnd = buffer + pattern.size();
  Tree* resultTree =
      buildTreeFromPattern(buffer, bufferEnd, contextExprs, nContextExprs);
  JuniorExpression resultExpr = JuniorExpression::Builder(resultTree);
  return *reinterpret_cast<TypedUserExpression*>(&resultExpr);
}

template <typename... Expressions>
TypedUserExpression BuildFromPattern(std::string buffer,
                                     Expressions... expressions) {
  const TypedUserExpression contextExprs[] = {expressions...};
  return BuildFromPatternImpl(buffer, contextExprs, sizeof...(expressions));
}

// --- Bindings ---

/* Macro to create binding functions for different numbers of
 * TypedUserExpression arguments */
#define BIND_BUILD_FROM_PATTERN(N) \
  class_function("FromPattern", &BuildFromPattern<REPEAT_ARGS(N)>)

#define REPEAT_ARGS_1 const TypedUserExpression
#define REPEAT_ARGS_2 REPEAT_ARGS_1, const TypedUserExpression
#define REPEAT_ARGS_3 REPEAT_ARGS_2, const TypedUserExpression
#define REPEAT_ARGS_4 REPEAT_ARGS_3, const TypedUserExpression
#define REPEAT_ARGS_5 REPEAT_ARGS_4, const TypedUserExpression
#define REPEAT_ARGS_6 REPEAT_ARGS_5, const TypedUserExpression
#define REPEAT_ARGS_7 REPEAT_ARGS_6, const TypedUserExpression
#define REPEAT_ARGS_8 REPEAT_ARGS_7, const TypedUserExpression
#define REPEAT_ARGS_9 REPEAT_ARGS_8, const TypedUserExpression
#define REPEAT_ARGS_10 REPEAT_ARGS_9, const TypedUserExpression

#define REPEAT_ARGS(N) REPEAT_ARGS_##N

class DummyClass {};  // Dummy class to bind static functions

EMSCRIPTEN_BINDINGS(expression_builder) {
  class_<DummyClass>("BuildExpression")
      .class_function("SystemInt", &BuildSystemInt)
      .class_function("UserInt", &BuildUserInt)
      .class_function("SystemFloat", &BuildSystemFloat)
      .class_function("UserFloat", &BuildUserFloat)
      .class_function("SystemRational", &BuildSystemRational)
      .class_function("UserRational", &BuildUserRational)
      .class_function("FromLatex", &BuildFromLatexString)
      .class_function("FromPattern", &BuildFromPattern<>)
      .BIND_BUILD_FROM_PATTERN(1)
      .BIND_BUILD_FROM_PATTERN(2)
      .BIND_BUILD_FROM_PATTERN(3)
      .BIND_BUILD_FROM_PATTERN(4)
      .BIND_BUILD_FROM_PATTERN(5)
      .BIND_BUILD_FROM_PATTERN(6)
      .BIND_BUILD_FROM_PATTERN(7)
      .BIND_BUILD_FROM_PATTERN(8)
      .BIND_BUILD_FROM_PATTERN(9)
      .BIND_BUILD_FROM_PATTERN(10);
}

}  // namespace Poincare::JSBridge

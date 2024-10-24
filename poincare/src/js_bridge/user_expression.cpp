#include <emscripten/bind.h>
#include <poincare/helpers/expression_equal_sign.h>
#include <poincare/k_tree.h>
#include <poincare/old/empty_context.h>
#include <poincare/src/expression/projection.h>
#include <poincare/src/expression/rational.h>
#include <poincare/src/memory/pattern_matching.h>
#include <poincare/src/memory/tree.h>
#include <poincare/src/memory/tree_stack.h>
#include <poincare/src/memory/type_block.h>

#include <string>

#include "typed_expression.h"

using namespace emscripten;
using namespace Poincare::Internal;

namespace Poincare::JSBridge {

// === 1. Builders ===

// === 1.1. Build from numbers ===

TypedUserExpression BuildUserInt(int32_t value) {
  JuniorExpression result = JuniorExpression::Builder(value);
  return TypedUserExpression::Cast(result);
}

TypedUserExpression BuildUserFloat(double value) {
  JuniorExpression result = JuniorExpression::Builder<double>(value);
  return TypedUserExpression::Cast(result);
}

TypedUserExpression BuildUserRational(int32_t numerator, int32_t denominator) {
  JuniorExpression result = JuniorExpression::Builder(
      Rational::Push(IntegerHandler(numerator), IntegerHandler(denominator)));
  return TypedUserExpression::Cast(result);
}

// === 1.2. Build from Latex string ===

TypedUserExpression BuildFromLatex(std::string latex) {
  EmptyContext context;
  return TypedUserExpression::Cast(
      JuniorExpression::ParseLatex(latex.c_str(), &context));
}

TypedUserExpression BuildFromLatexWithAssignmentParam(std::string latex,
                                                      bool parseForAssignment) {
  EmptyContext context;
  return TypedUserExpression::Cast(JuniorExpression::ParseLatex(
      latex.c_str(), &context, true, parseForAssignment));
}

// === 1.3. Build from pattern ===

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
    /* BUILDER(Trig) -> System node, not allowed in UserExpressions ⛔️ */
    BUILDER(ACos),
    BUILDER(ATan),
    BUILDER(ASin),
    /* BUILDER(ATrig) -> System node, not allowed in UserExpressions ⛔️ */
    /* BUILDER(ATanRad) -> System node, not allowed in UserExpressions ⛔️ */
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
    BUILDER(LnUser),
    /* BUILDER(Ln) -> System node, not allowed in UserExpressions ⛔️ */
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
    /* BUILDER(Polynomial) -> ? */
    /* BUILDER(PowReal) -> System node, not allowed in UserExpressions ⛔️ */
    BUILDER(Quo),
    BUILDER(Re),
    BUILDER(Rem),
    BUILDER(Round),
    BUILDER(Sign),
    BUILDER(Sqrt),
    BUILDER(Root),
    BUILDER(Sub),
    /* BUILDER(TrigDiff) -> System node, not allowed in UserExpressions ⛔️ */
    /* Var -> Partially implemented, see at end of the list ✅ */

    // 4 - Parametric types
    BUILDER(Sum),
    BUILDER(Product),
    BUILDER(Diff),
    BUILDER(Integral),
    /* BUILDER(IntegralWithAlternatives) -> System node, not allow in
       UserExpressions ⛔️ */
    BUILDER(ListSequence),

    // 5 - Matrix and vector builtins
    /* BUILDER(Dot) -> Not usable in PoincareJS for now 🏗️ */
    /* BUILDER(Norm) -> Not usable in PoincareJS for now 🏗️ */
    /* BUILDER(Trace) -> Not usable in PoincareJS for now 🏗️ */
    /* BUILDER(Cross) -> Not usable in PoincareJS for now 🏗️ */
    /* BUILDER(Det) -> Not usable in PoincareJS for now 🏗️ */
    /* BUILDER(Dim) -> Not usable in PoincareJS for now 🏗️ */
    /* BUILDER(Identity) -> Not usable in PoincareJS for now 🏗️ */
    /* BUILDER(Inverse) -> Not usable in PoincareJS for now 🏗️ */
    /* BUILDER(Ref) -> Not usable in PoincareJS for now 🏗️ */
    /* BUILDER(Rref) -> Not usable in PoincareJS for now 🏗️ */
    /* BUILDER(Transpose) -> Not usable in PoincareJS for now 🏗️ */
    /* BUILDER(PowMatrix) -> System node, not allowed in UserExpressions ⛔️ */
    /* Matrix -> Not implemented ❌ */

    // 6 - Lists
    /* BUILDER(List) -> Not usable in PoincareJS for now 🏗️ */
    /* BUILDER(ListSort) -> Not usable in PoincareJS for now 🏗️ */
    /* BUILDER(ListElement) -> Not usable in PoincareJS for now 🏗️ */
    /* BUILDER(ListSlice) -> Not usable in PoincareJS for now 🏗️ */
    /* BUILDER(Mean) -> Not usable in PoincareJS for now 🏗️ */
    /* BUILDER(StdDev) -> Not usable in PoincareJS for now 🏗️ */
    /* BUILDER(Median) -> Not usable in PoincareJS for now 🏗️ */
    /* BUILDER(Variance) -> Not usable in PoincareJS for now 🏗️ */
    /* BUILDER(SampleStdDev) -> Not usable in PoincareJS for now 🏗️ */
    /* BUILDER(Min) -> Not usable in PoincareJS for now 🏗️ */
    /* BUILDER(Max) -> Not usable in PoincareJS for now 🏗️ */
    /* BUILDER(ListSum) -> Not usable in PoincareJS for now 🏗️ */
    /* BUILDER(ListProduct) -> Not usable in PoincareJS for now 🏗️ */
    /* BUILDER(Point) -> Not usable in PoincareJS for now 🏗️ */

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
    /* BUILDER(Dependency) -> System node ⛔️ */
    /* BUILDER(Dependencies) -> System node ⛔️ */
    /* BUILDER(Set) -> System node ⛔️ */
    BUILDER(Parentheses),
    /* Empty -> Not implemented ❌ */

    // 10 - Undefined expressions
    BUILDER(NonReal),
    /* BUILDER(UndefZeroPowerZero) -> System node ⛔️ */
    /* BUILDER(UndefZeroDivision) -> System node ⛔️ */
    /* BUILDER(UndefUnhandled) -> System node ⛔️ */
    /* BUILDER(UndefUnhandledDimension) -> System node ⛔️ */
    /* BUILDER(UndefBadType) -> System node ⛔️ */
    /* BUILDER(UndefOutOfDefinition) -> System node ⛔️ */
    /* BUILDER(UndefNotDefined) -> System node ⛔️ */
    /* BUILDER(UndefForbidden) -> System node ⛔️ */
    BUILDER(Undef),

    // 11 - Operations on expressions
    /* BUILDER(Store) -> Not usable in PoincareJS for now 🏗️ */
    /* BUILDER(UnitConversion) -> Not usable in PoincareJS for now 🏗️ */
    /* BUILDER(SequenceExplicit) -> ? */
    /* BUILDER(SequenceSingleRecurrence) -> ? */
    /* BUILDER(SequenceDoubleRecurrence) -> ? */
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

// Parses the pattern and builds the tree from it
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
  Tree* result = Tree::FromBlocks(stack->lastBlock());

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
      // Should end with ')'
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
      // Parentheses are not balanced
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
        stack->flushFromBlock(result);
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

template <typename... Expressions>
TypedUserExpression BuildFromPattern(std::string pattern,
                                     Expressions... expressions) {
  const char* buffer = pattern.c_str();
  const char* bufferEnd = buffer + pattern.size();
  const TypedUserExpression contextExprs[] = {expressions...};
  Tree* resultTree = buildTreeFromPattern(buffer, bufferEnd, contextExprs,
                                          sizeof...(expressions));
  return TypedUserExpression::Cast(JuniorExpression::Builder(resultTree));
}

// === 2. Methods ===

bool ExactAndApproximateExpressionsAreStrictlyEqualWrapper(
    const TypedUserExpression& exact, const TypedUserExpression& approximate,
    Preferences::ComplexFormat complexFormat,
    Preferences::AngleUnit angleUnit) {
  ProjectionContext ctx{.m_complexFormat = complexFormat,
                        .m_angleUnit = angleUnit};
  return ExactAndApproximateExpressionsAreStrictlyEqual(exact, approximate,
                                                        &ctx);
}

std::string typedToLatex(const TypedUserExpression& expression,
                         int numberOfSignificantDigits,
                         bool withThousandsSeparators) {
  constexpr int k_bufferSize = 1024;  // TODO: make this bigger ? or malloc ?
  char buffer[k_bufferSize];
  EmptyContext context;
  expression.toLatex(buffer, k_bufferSize, Preferences::PrintFloatMode::Decimal,
                     numberOfSignificantDigits, &context,
                     withThousandsSeparators);
  return std::string(buffer, strlen(buffer));
}

std::string typedToLatexWithThousandsSeparators(
    const TypedUserExpression& expression, int numberOfSignificantDigits) {
  return typedToLatex(expression, numberOfSignificantDigits, true);
}

std::string typedToLatexWith7DigitsAndThousandsSeparators(
    const TypedUserExpression& expression) {
  return typedToLatexWithThousandsSeparators(expression, 7);
}

TypedSystemExpression typedCloneAndReduce(
    const TypedUserExpression& expr, const ReductionContext& reductionContext) {
  JuniorExpression result = expr.cloneAndReduce(reductionContext);
  return TypedSystemExpression::Cast(result);
}

// === 3. Bindings ===

/* Macro to create binding functions for different numbers of
 * TypedUserExpression arguments */
#define BIND_BUILD_FROM_PATTERN(N) \
  class_function("BuildFromPattern", &BuildFromPattern<REPEAT_ARGS(N)>)

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

EMSCRIPTEN_BINDINGS(user_expression) {
  register_type<TypedUserExpression::JsTree>("UserExpressionTree");
  class_<TypedUserExpression, base<JuniorExpression>>("PCR_UserExpression")
      .constructor<>()
      .class_function("BuildFromTree", &TypedUserExpression::BuildFromJsTree)
      .function("getTree", &TypedUserExpression::getJsTree)
      .function("clone", &TypedUserExpression::typedClone)
      .function("cloneChildAtIndex",
                &TypedUserExpression::typedCloneChildAtIndex)
      .class_function("BuildInt", &BuildUserInt)
      .class_function("BuildFloat", &BuildUserFloat)
      .class_function("BuildRational", &BuildUserRational)
      .class_function("BuildFromLatex", &BuildFromLatex)
      .class_function("BuildFromLatex", &BuildFromLatexWithAssignmentParam)
      .class_function("BuildFromPattern", &BuildFromPattern<>)
      .BIND_BUILD_FROM_PATTERN(1)
      .BIND_BUILD_FROM_PATTERN(2)
      .BIND_BUILD_FROM_PATTERN(3)
      .BIND_BUILD_FROM_PATTERN(4)
      .BIND_BUILD_FROM_PATTERN(5)
      .BIND_BUILD_FROM_PATTERN(6)
      .BIND_BUILD_FROM_PATTERN(7)
      .BIND_BUILD_FROM_PATTERN(8)
      .BIND_BUILD_FROM_PATTERN(9)
      .BIND_BUILD_FROM_PATTERN(10)
      .class_function("ExactAndApproximateExpressionsAreStrictlyEqual",
                      &ExactAndApproximateExpressionsAreStrictlyEqualWrapper)
      .function("toLatex", &typedToLatex)
      .function("toLatex", &typedToLatexWithThousandsSeparators)
      .function("toLatex", &typedToLatexWith7DigitsAndThousandsSeparators)
      .function("cloneAndReduce", &typedCloneAndReduce);
}
}  // namespace Poincare::JSBridge

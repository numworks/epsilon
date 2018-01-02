#ifndef POINCARE_LAYOUT_ENGINE_H
#define POINCARE_LAYOUT_ENGINE_H

#include <poincare/expression.h>

namespace Poincare {

class LayoutEngine {

public:
  /* Expression to ExpressionLayout */
  static ExpressionLayout * createInfixLayout(const Expression * expression, Expression::FloatDisplayMode floatDisplayMode, Expression::ComplexFormat complexFormat, const char * operatorName);
  static ExpressionLayout * createPrefixLayout(const Expression * expression, Expression::FloatDisplayMode floatDisplayMode, Expression::ComplexFormat complexFormat, const char * operatorName);

  /* Expression to Text */
  typedef bool (*OperandNeedsParenthesis)(const Expression * e);
  static int writeInfixExpressionTextInBuffer(
      const Expression * expression,
      char * buffer,
      int bufferSize,
      const char * operatorName,
      OperandNeedsParenthesis operandNeedsParenthesis = [](const Expression * e) { return e->type() == Expression::Type::Opposite; });
  static int writePrefixExpressionTextInBuffer(
      const Expression * expression,
      char * buffer,
      int bufferSize,
      const char * operatorName);

  /* ExpressionLayout to Text */
  typedef bool (*ChildNeedsParenthesis)(const char * operatorName);
  static int writeInfixExpressionLayoutTextInBuffer(
      const ExpressionLayout * expressionLayout,
      char * buffer,
      int bufferSize,
      const char * operatorName,
      int firstChildIndex = 0,
      int lastChildIndex = -1,
      ChildNeedsParenthesis childNeedsParenthesis = [](const char * operatorName) {
        return (operatorName[1] == 0 && (operatorName[0] == powerChar || operatorName[0] == divideChar)); }); //TODO
  static int writePrefixExpressionLayoutTextInBuffer(
      const ExpressionLayout * expressionLayout,
      char * buffer,
      int bufferSize,
      const char * operatorName,
      bool writeFirstChild = true);

  /* Write one char in buffer */
  static int writeOneCharInBuffer(char * buffer, int bufferSize, char charToWrite);

private:
  static constexpr char powerChar = '^';
  static constexpr char divideChar = '/';
  // These two functions return the index of the null-terminating char.
  static int writeInfixExpressionOrExpressionLayoutTextInBuffer(const Expression * expression, const ExpressionLayout * expressionLayout, char * buffer, int bufferSize, const char * operatorName, int firstChildIndex, int lastChildIndex, OperandNeedsParenthesis operandNeedsParenthesis, ChildNeedsParenthesis childNeedsParenthesis);
  static int writePrefixExpressionOrExpressionLayoutTextInBuffer(const Expression * expression, const ExpressionLayout * expressionLayout, char * buffer, int bufferSize, const char * operatorName, bool writeFirstChild = true);
};

}

#endif

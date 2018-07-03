#ifndef POINCARE_LAYOUT_ENGINE_H
#define POINCARE_LAYOUT_ENGINE_H

#include <poincare/expression.h>
#include <poincare/layout_reference.h>

namespace Poincare {

class LayoutEngine {

public:
  /* Expression to LayoutRef */
  static LayoutRef createInfixLayout(const Expression * expression,  PrintFloat::Mode floatDisplayMode, Expression::ComplexFormat complexFormat, const char * operatorName);
  static LayoutRef createPrefixLayout(const Expression * expression, PrintFloat::Mode floatDisplayMode, Expression::ComplexFormat complexFormat, const char * operatorName);

  /* Create special layouts */
  static LayoutRef createParenthesedLayout(LayoutRef layout, bool cloneLayout);
  static LayoutRef createStringLayout(const char * buffer, int bufferSize, KDText::FontSize fontSize = KDText::FontSize::Large);
  static ExpressionLayout * createLogLayout(ExpressionLayout * argument, ExpressionLayout * index);

  /* Expression to Text */
  static int writeInfixExpressionTextInBuffer(
      const Expression * expression,
      char * buffer,
      int bufferSize,
      int numberOfDigits,
      const char * operatorName);
  static int writePrefixExpressionTextInBuffer(
      const Expression * expression,
      char * buffer,
      int bufferSize,
      int numberOfDigits,
      const char * operatorName);

  /* ExpressionLayout to Text */
  typedef bool (*ChildNeedsParenthesis)(const char * operatorName);
  static int writeInfixExpressionLayoutTextInBuffer(
      const ExpressionLayout * expressionLayout,
      char * buffer,
      int bufferSize,
      int numberOfDigits,
      const char * operatorName,
      int firstChildIndex = 0,
      int lastChildIndex = -1,
      ChildNeedsParenthesis childNeedsParenthesis = [](const char * operatorName) {
        return (operatorName[0] != 0 && operatorName[1] == 0 && (operatorName[0] == divideChar)); });
  static int writePrefixExpressionLayoutTextInBuffer(
      const ExpressionLayout * expressionLayout,
      char * buffer,
      int bufferSize,
      int numberOfDigits,
      const char * operatorName,
      bool writeFirstChild = true);

  /* LayoutReference to Text */
  static int writeInfixTreeRefTextInBuffer(
      const TreeRef treeRef,
      char * buffer,
      int bufferSize,
      int numberOfDigits,
      const char * operatorName,
      int firstChildIndex = 0,
      int lastChildIndex = -1);

  /* Write one char in buffer */
  static int writeOneCharInBuffer(char * buffer, int bufferSize, char charToWrite);

private:
  static constexpr char divideChar = '/';
  // These two functions return the index of the null-terminating char.
  static int writeInfixExpressionOrExpressionLayoutTextInBuffer(const Expression * expression, const ExpressionLayout * expressionLayout, char * buffer, int bufferSize, int numberOfDigits, const char * operatorName, int firstChildIndex, int lastChildIndex, ChildNeedsParenthesis childNeedsParenthesis);
  static int writePrefixExpressionOrExpressionLayoutTextInBuffer(const Expression * expression, const ExpressionLayout * expressionLayout, char * buffer, int bufferSize, int numberOfDigits, const char * operatorName, bool writeFirstChild = true);

  static void writeChildTreeInBuffer(TreeRef childRef, TreeRef parentRef, char * buffer, int bufferSize, int numberOfDigits, int * numberOfChar);
};

}

#endif

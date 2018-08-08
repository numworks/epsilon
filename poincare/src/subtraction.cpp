#include <poincare/subtraction.h>
#include <poincare/allocation_failure_expression_node.h>
#include <poincare/serialization_helper.h>
//#include <poincare/addition.h>
//#include <poincare/matrix.h>
//#include <poincare/multiplication.h>
#include <poincare/opposite.h>
//#include <poincare/rational.h>
#include <assert.h>

namespace Poincare {

SubtractionNode * SubtractionNode::FailedAllocationStaticNode() {
  static AllocationFailureExpressionNode<SubtractionNode> failure;
  return &failure;
}

int SubtractionNode::polynomialDegree(char symbolName) const {
  int degree = 0;
  for (int i = 0; i < numberOfChildren(); i++) {
    int d = childAtIndex(i)->polynomialDegree(symbolName);
    if (d < 0) {
      return -1;
    }
    degree = d > degree ? d : degree;
  }
  return degree;
}

// Private

bool SubtractionNode::needsParenthesesWithParent(const SerializationHelperInterface * parent) const {
  Type types[] = {Type::Subtraction, Type::Opposite, Type::Multiplication, Type::Division, Type::Power, Type::Factorial};
  return static_cast<const ExpressionNode *>(parent)->isOfType(types, 6);
}

LayoutRef SubtractionNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Infix(Expression(this), floatDisplayMode, numberOfSignificantDigits, name());
}

int SubtractionNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
    return SerializationHelper::Infix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, name());
}

template<typename T> MatrixComplex<T> SubtractionNode::computeOnComplexAndMatrix(const std::complex<T> c, const MatrixComplex<T> m) {
  MatrixComplex<T> opposite = computeOnMatrixAndComplex(m, c);
  MatrixComplex<T> result;
  for (int i = 0; i < opposite.numberOfChildren(); i++) {
    result.addChildAtIndexInPlace(OppositeNode::compute(*(static_cast<ComplexNode<T> *>(opposite.complexAtIndex(i).node()))), i, i);
  }
  result.setDimensions(opposite.numberOfRows(), opposite.numberOfColumns());
  return result;
}


Expression SubtractionNode::shallowReduce(Context& context, Preferences::AngleUnit angleUnit) const {
  return Subtraction(this).shallowReduce(context, angleUnit);
}

Expression Subtraction::shallowReduce(Context& context, Preferences::AngleUnit angleUnit) const {
  Expression e = Expression::shallowReduce(context, angleUnit);
  if (e.type() == ExpressionNode::Type::AllocationFailure || e.type() == ExpressionNode::Type::Undefined ) { //TODO use Expression method to make the ||
    return e;
  }
  /*Multiplication m = Multiplication(Rational(-1), childAtIndex(1));
  Expression mReduced = m.shallowReduce(context, angleUnit);
  Addition a = Addition(childAtIndex(0), mReduced);
  return a.shallowReduce(context, angleUnit);*/
}

}

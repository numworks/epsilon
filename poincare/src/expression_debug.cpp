#include <poincare/global_context.h>
#include <poincare/expression.h>
#include <poincare/symbol.h>
#include <poincare/integer.h>
#include <poincare/rational.h>
#include <ion.h>
#include <iostream>

namespace Poincare {

void print_expression(const Expression e, int indentationLevel) {
  if (indentationLevel>0) {
    for (int i=0; i<indentationLevel-1; i++) {
      std::cout << "  ";
    }
    std::cout << "|-";
  }
  GlobalContext context;
  switch (e.type()) {
    case ExpressionNode::Type::AbsoluteValue:
      std::cout << "AbsoluteValue";
      break;
    case ExpressionNode::Type::Addition:
      std::cout << "Addition";
      break;
    case ExpressionNode::Type::ArcCosine:
      std::cout << "ArcCosine";
      break;
    case ExpressionNode::Type::ArcSine:
      std::cout << "ArcSine";
      break;
    case ExpressionNode::Type::ArcTangent:
      std::cout << "ArcTangent";
      break;
    case ExpressionNode::Type::BinomialCoefficient:
      std::cout << "BinomialCoefficient";
      break;
    case ExpressionNode::Type::Ceiling:
      std::cout << "Ceiling";
      break;
    case ExpressionNode::Type::ComplexArgument:
      std::cout << "ComplexArgument";
      break;
    case ExpressionNode::Type::ConfidenceInterval:
      std::cout << "ConfidenceInterval";
      break;
    case ExpressionNode::Type::Conjugate:
      std::cout << "Conjugate";
      break;
    case ExpressionNode::Type::Cosine:
      std::cout << "Cosine";
      break;
    case ExpressionNode::Type::Decimal:
      std::cout << "Decimal(";
      std::cout << e.approximateToScalar<double>(context, Preferences::AngleUnit::Radian);
      std::cout << ")";
      break;
    case ExpressionNode::Type::Derivative:
      std::cout << "Derivative";
      break;
    case ExpressionNode::Type::Determinant:
      std::cout << "Determinant";
      break;
    case ExpressionNode::Type::Division:
      std::cout << "Division";
      break;
    case ExpressionNode::Type::DivisionQuotient:
      std::cout << "DivisionQuotient";
      break;
    case ExpressionNode::Type::DivisionRemainder:
      std::cout << "DivisionRemainder";
      break;
    case ExpressionNode::Type::EmptyExpression:
      std::cout << "EmptyExpression";
      break;
    case ExpressionNode::Type::Equal:
      std::cout << "Equal";
      break;
    case ExpressionNode::Type::Factor:
      std::cout << "Factor";
      break;
    case ExpressionNode::Type::Factorial:
      std::cout << "Factorial";
      break;
    case ExpressionNode::Type::Float:
      std::cout << "Float(";
      std::cout << e.approximateToScalar<double>(context, Preferences::AngleUnit::Radian);
      std::cout << ")";
      break;
    case ExpressionNode::Type::Floor:
      std::cout << "Floor";
      break;
    case ExpressionNode::Type::FracPart:
      std::cout << "FracPart";
      break;
    case ExpressionNode::Type::GreatCommonDivisor:
      std::cout << "GreatCommonDivisor";
      break;
    case ExpressionNode::Type::HyperbolicArcCosine:
      std::cout << "HyperbolicArcCosine";
      break;
    case ExpressionNode::Type::HyperbolicArcSine:
      std::cout << "HyperbolicArcSine";
      break;
    case ExpressionNode::Type::HyperbolicArcTangent:
      std::cout << "HyperbolicArcTangent";
      break;
    case ExpressionNode::Type::HyperbolicCosine:
      std::cout << "HyperbolicCosine";
      break;
    case ExpressionNode::Type::HyperbolicSine:
      std::cout << "HyperbolicSine";
      break;
    case ExpressionNode::Type::HyperbolicTangent:
      std::cout << "HyperbolicTangent";
      break;
    case ExpressionNode::Type::ImaginaryPart:
      std::cout << "ImaginaryPart";
      break;
    case ExpressionNode::Type::Infinity:
      std::cout << "Infinity(";
      std::cout << (e.sign() == ExpressionNode::Sign::Negative ? "Negative" : "Positive");
      std::cout << ")";
      break;
    case ExpressionNode::Type::Integral:
      std::cout << "Integral";
      break;
    case ExpressionNode::Type::LeastCommonMultiple:
      std::cout << "LeastCommonMultiple";
      break;
    case ExpressionNode::Type::Logarithm:
      std::cout << "Logarithm";
      break;
    case ExpressionNode::Type::Matrix:
      std::cout << "Matrix(Rows: ";
      std::cout << static_cast<const Matrix &>(e).numberOfRows();
      std::cout << ", Columns: ";
      std::cout << static_cast<const Matrix &>(e).numberOfColumns();
      std::cout << ")";
      break;
    case ExpressionNode::Type::MatrixDimension:
      std::cout << "MatrixDimension";
      break;
    case ExpressionNode::Type::MatrixInverse:
      std::cout << "MatrixInverse";
      break;
    case ExpressionNode::Type::MatrixTrace:
      std::cout << "MatrixTrace";
      break;
    case ExpressionNode::Type::MatrixTranspose:
      std::cout << "MatrixTranspose";
      break;
    case ExpressionNode::Type::MultiplicationExplicite:
      std::cout << "Multiplication Explicite";
      break;
    case ExpressionNode::Type::MultiplicationImplicite:
      std::cout << "Multiplication Implicite";
      break;
    case ExpressionNode::Type::NaperianLogarithm:
        std::cout << "NaperianLogarithm";
      break;
    case ExpressionNode::Type::NthRoot:
      std::cout << "NthRoot";
      break;
    case ExpressionNode::Type::Opposite:
      std::cout << "Opposite";
      break;
    case ExpressionNode::Type::Parenthesis:
      std::cout << "Parenthesis";
      break;
    case ExpressionNode::Type::PermuteCoefficient:
      std::cout << "PermuteCoefficient";
      break;
    case ExpressionNode::Type::PredictionInterval:
      std::cout << "PredictionInterval";
      break;
    case ExpressionNode::Type::Power:
      std::cout << "Power";
      break;
    case ExpressionNode::Type::Product:
      std::cout << "Product";
      break;
    case ExpressionNode::Type::Random:
      std::cout << "Random";
      break;
    case ExpressionNode::Type::Randint:
      std::cout << "Randint";
      break;
    case ExpressionNode::Type::Rational:
      std::cout << "Rational(";
      std::cout << static_cast<const Rational &>(e).signedIntegerNumerator().approximate<double>();
      std::cout << ", ";
      std::cout << static_cast<const Rational &>(e).integerDenominator().approximate<double>();
      std::cout << ")";
      break;
    case ExpressionNode::Type::RealPart:
      std::cout << "RealPart";
      break;
    case ExpressionNode::Type::Round:
      std::cout << "Round";
      break;
    case ExpressionNode::Type::Sine:
      std::cout << "Sine";
      break;
    case ExpressionNode::Type::SquareRoot:
      std::cout << "SquareRoot";
      break;
    case ExpressionNode::Type::Store:
      std::cout << "Store";
      break;
    case ExpressionNode::Type::Subtraction:
      std::cout << "Subtraction";
      break;
    case ExpressionNode::Type::Sum:
      std::cout << "Sum";
      break;
    case ExpressionNode::Type::Symbol:
      std::cout << "Symbol(";
      UTF8Decoder decoder(static_cast<const Symbol &>(e).name());
      CodePoint firstCodePoint = decoder.nextCodePoint();
      switch (firstCodePoint) {
        case UCodePointGreekSmallLetterPi:
          std::cout << "PI";
          break;
        case UCodePointMathematicalBoldSmallI:
          std::cout << "i";
          break;
        case UCodePointScriptSmallE:
          std::cout << "e";
          break;
        default:
          std::cout << static_cast<const Symbol &>(e).name();
      }
      std::cout << ")";
      break;
    case ExpressionNode::Type::Tangent:
      std::cout << "Tangent";
      break;
    case ExpressionNode::Type::Undefined:
      std::cout << "Undefined";
      break;
    case ExpressionNode::Type::Uninitialized:
      std::cout << "Uninitialized";
      break;
  }
  std::cout << " (identifier: " << e.identifier() << ")" << std::endl;
  for (int i=0; i<e.numberOfChildren(); i++) {
    print_expression(e.childAtIndex(i), indentationLevel+1);
  }
}

void print_prime_factorization(Integer * outputFactors, Integer * outputCoefficients, int outputLength) {
  for (int index = 0; index < outputLength; index++) {
    if (outputCoefficients[index].isEqualTo(Integer(0)) || outputCoefficients[index].isLowerThan(Integer(0))) {
      break;
    }
    std::cout << outputFactors[index].approximate<double>();
    std::cout << "^";
    std::cout << outputCoefficients[index].approximate<double>();
    std::cout << "*";
  }
  std::cout <<"  "<< std::endl;
}

}

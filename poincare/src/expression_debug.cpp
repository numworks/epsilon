#include <poincare/global_context.h>
#include <poincare/expression.h>
#include <poincare/symbol.h>
#include <poincare/integer.h>
#include <poincare/rational.h>
#include <poincare/complex.h>
#include <ion.h>
#include <iostream>

namespace Poincare {

void print_expression(const Expression * e, int indentationLevel) {
  if (indentationLevel>0) {
    for (int i=0; i<indentationLevel-1; i++) {
      std::cout << "  ";
    }
    std::cout << "|-";
  }
  GlobalContext context;
  switch (e->type()) {
    case Expression::Type::AbsoluteValue:
      std::cout << "AbsoluteValue";
      break;
    case Expression::Type::Addition:
      std::cout << "Addition";
      break;
    case Expression::Type::ArcCosine:
      std::cout << "ArcCosine";
      break;
    case Expression::Type::ArcSine:
      std::cout << "ArcSine";
      break;
    case Expression::Type::ArcTangent:
      std::cout << "ArcTangent";
      break;
    case Expression::Type::BinomialCoefficient:
      std::cout << "BinomialCoefficient";
      break;
    case Expression::Type::Ceiling:
      std::cout << "Ceiling";
      break;
    case Expression::Type::Complex:
      std::cout << "Complex(";
      std::cout << static_cast<const Complex<double> *>(e)->a();
      std::cout << ", ";
      std::cout << static_cast<const Complex<double> *>(e)->b();
      std::cout << ")";
      break;
    case Expression::Type::ComplexArgument:
      std::cout << "ComplexArgument";
      break;
    case Expression::Type::ConfidenceInterval:
      std::cout << "ConfidenceInterval";
      break;
    case Expression::Type::Conjugate:
      std::cout << "Conjugate";
      break;
    case Expression::Type::Cosine:
      std::cout << "Cosine";
      break;
    case Expression::Type::Decimal:
      std::cout << "Decimal(";
      std::cout << e->approximateToScalar<double>(context, Expression::AngleUnit::Radian);
      std::cout << ")";
      break;
    case Expression::Type::Derivative:
      std::cout << "Derivative";
      break;
    case Expression::Type::Determinant:
      std::cout << "Determinant";
      break;
    case Expression::Type::Division:
      std::cout << "Division";
      break;
    case Expression::Type::DivisionQuotient:
      std::cout << "DivisionQuotient";
      break;
    case Expression::Type::DivisionRemainder:
      std::cout << "DivisionRemainder";
      break;
    case Expression::Type::Factor:
      std::cout << "Factor";
      break;
    case Expression::Type::Factorial:
      std::cout << "Factorial";
      break;
    case Expression::Type::Floor:
      std::cout << "Floor";
      break;
    case Expression::Type::FracPart:
      std::cout << "FracPart";
      break;
    case Expression::Type::GreatCommonDivisor:
      std::cout << "GreatCommonDivisor";
      break;
    case Expression::Type::HyperbolicArcCosine:
      std::cout << "HyperbolicArcCosine";
      break;
    case Expression::Type::HyperbolicArcSine:
      std::cout << "HyperbolicArcSine";
      break;
    case Expression::Type::HyperbolicArcTangent:
      std::cout << "HyperbolicArcTangent";
      break;
    case Expression::Type::HyperbolicCosine:
      std::cout << "HyperbolicCosine";
      break;
    case Expression::Type::HyperbolicSine:
      std::cout << "HyperbolicSine";
      break;
    case Expression::Type::HyperbolicTangent:
      std::cout << "HyperbolicTangent";
      break;
    case Expression::Type::ImaginaryPart:
      std::cout << "ImaginaryPart";
      break;
    case Expression::Type::Integral:
      std::cout << "Integral";
      break;
    case Expression::Type::LeastCommonMultiple:
      std::cout << "LeastCommonMultiple";
      break;
    case Expression::Type::Logarithm:
      std::cout << "Logarithm";
      break;
    case Expression::Type::Matrix:
      std::cout << "Matrix(Rows: ";
      std::cout << static_cast<const Matrix *>(e)->numberOfRows();
      std::cout << ", Columns: ";
      std::cout << static_cast<const Matrix *>(e)->numberOfColumns();
      std::cout << ")";
      break;
    case Expression::Type::MatrixDimension:
      std::cout << "MatrixDimension";
      break;
    case Expression::Type::MatrixInverse:
      std::cout << "MatrixInverse";
      break;
    case Expression::Type::MatrixTrace:
      std::cout << "MatrixTrace";
      break;
    case Expression::Type::MatrixTranspose:
      std::cout << "MatrixTranspose";
      break;
    case Expression::Type::Multiplication:
      std::cout << "Multiplication";
      break;
    case Expression::Type::NaperianLogarithm:
        std::cout << "NaperianLogarithm";
      break;
    case Expression::Type::NthRoot:
      std::cout << "NthRoot";
      break;
    case Expression::Type::Opposite:
      std::cout << "Opposite";
      break;
    case Expression::Type::Parenthesis:
      std::cout << "Parenthesis";
      break;
    case Expression::Type::PermuteCoefficient:
      std::cout << "PermuteCoefficient";
      break;
    case Expression::Type::PredictionInterval:
      std::cout << "PredictionInterval";
      break;
    case Expression::Type::Power:
      std::cout << "Power";
      break;
    case Expression::Type::Product:
      std::cout << "Product";
      break;
    case Expression::Type::Random:
      std::cout << "Random";
      break;
    case Expression::Type::Randint:
      std::cout << "Randint";
      break;
    case Expression::Type::Rational:
      std::cout << "Rational(";
      std::cout << static_cast<const Rational * >(e)->numerator().approximate<double>();
      std::cout << ", ";
      std::cout << static_cast<const Rational * >(e)->denominator().approximate<double>();
      std::cout << ")";
      break;
    case Expression::Type::RealPart:
      std::cout << "RealPart";
      break;
    case Expression::Type::Round:
      std::cout << "Round";
      break;
    case Expression::Type::SimplificationRoot:
      std::cout << "SimplificationRoot";
      break;
    case Expression::Type::Sine:
      std::cout << "Sine";
      break;
    case Expression::Type::SquareRoot:
      std::cout << "SquareRoot";
      break;
    case Expression::Type::Store:
      std::cout << "Store";
      break;
    case Expression::Type::Subtraction:
      std::cout << "Subtraction";
      break;
    case Expression::Type::Sum:
      std::cout << "Sum";
      break;
    case Expression::Type::Symbol:
      std::cout << "Symbol(";
      switch (((Symbol*)e)->name()) {
        case Ion::Charset::SmallPi:
          std::cout << "PI";
          break;
        case Ion::Charset::IComplex:
          std::cout << "i";
          break;
        case Ion::Charset::Exponential:
          std::cout << "e";
          break;
        default:
          std::cout << ((Symbol*)e)->name();
      }
      std::cout << ")";
      break;
    case Expression::Type::Tangent:
      std::cout << "Tangent";
      break;
    case Expression::Type::Undefined:
      std::cout << "Undefined";
      break;
  }
  std::cout << " at " << (void *)e << " with parent " << (void *)(e->parent()) << std::endl;
  for (int i=0; i<e->numberOfOperands(); i++) {
    print_expression(e->operand(i), indentationLevel+1);
  }
}

void print_prime_factorization(Integer * outputFactors, Integer * outputCoefficients, int outputLength) {
  for (int index = 0; index < outputLength; index++) {
    if (outputCoefficients[index].isEqualTo(Integer(0))) {
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

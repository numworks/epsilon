#include <poincare/global_context.h>
#include <poincare/expression.h>
#include <poincare/symbol.h>
#include <poincare/integer.h>
#include <poincare/rational.h>
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
    case Expression::Type::Addition:
      std::cout << "Addition";
      break;
    case Expression::Type::Multiplication:
      std::cout << "Multiplication";
      break;
    case Expression::Type::NthRoot:
      std::cout << "NthRoot";
      break;
    case Expression::Type::Cosine:
      std::cout << "Cosine";
      break;
    case Expression::Type::Logarithm:
      std::cout << "Log";
      break;
    case Expression::Type::NaperianLogarithm:
      std::cout << "Ln";
      break;
    case Expression::Type::Integer:
      std::cout << "Integer(";
      std::cout << e->approximate<double>(context);
      std::cout << ")";
      break;
    case Expression::Type::Division:
      std::cout << "Division";
      break;
    case Expression::Type::Opposite:
      std::cout << "Opposite";
      break;
      /*
    case Expression::Type::Matrix:
      std::cout << "Matrix";
      break;
      */
    case Expression::Type::Parenthesis:
      std::cout << "Parenthesis";
      break;
    case Expression::Type::Power:
      std::cout << "Power";
      break;
    case Expression::Type::Product:
      std::cout << "Product";
      break;
    case Expression::Type::Rational:
      std::cout << "Rational(";
      std::cout << static_cast<const Rational * >(e)->numerator().approximate<double>(context);
      std::cout << ", ";
      std::cout << static_cast<const Rational * >(e)->denominator().approximate<double>(context);
      std::cout << ")";
      break;
    case Expression::Type::Sine:
      std::cout << "Sine";
      break;
    case Expression::Type::SquareRoot:
      std::cout << "SquareRoot";
      break;
    case Expression::Type::Subtraction:
      std::cout << "Subtraction";
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
  }
  std::cout << " at " << (void *)e << " with parent " << (void *)(e->parent()) << std::endl;
  for (int i=0; i<e->numberOfOperands(); i++) {
    print_expression(e->operand(i), indentationLevel+1);
  }
}

void print_prime_factorization(Integer * outputFactors, Integer * outputCoefficients, int outputLength) {
  GlobalContext context;
  for (int index = 0; index < outputLength; index++) {
    if (outputCoefficients[index].isEqualTo(Integer(0))) {
      break;
    }
    std::cout << outputFactors[index].approximate<double>(context);
    std::cout << "^";
    std::cout << outputCoefficients[index].approximate<double>(context);
    std::cout << "+";
  }
  std::cout <<"  "<< std::endl;
}

}

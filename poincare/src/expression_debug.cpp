#include <poincare/global_context.h>
#include <poincare/expression.h>
#include <poincare/symbol.h>
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
    case Expression::Type::Cosine:
      std::cout << "Cosine";
      break;
    case Expression::Type::NaperianLogarithm:
      std::cout << "Ln";
      break;
    case Expression::Type::Integer:
      std::cout << "Integer(";
      std::cout << e->approximate<double>(context);
      std::cout << ")";
      break;
    case Expression::Type::Fraction:
      std::cout << "Fraction";
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
    case Expression::Type::Sine:
      std::cout << "Sine";
      break;
    case Expression::Type::Subtraction:
      std::cout << "Subtraction";
      break;
    case Expression::Type::Symbol:
      std::cout << "Symbol(" << ((Symbol*)e)->name() << ")";
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

}

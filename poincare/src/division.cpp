extern "C" {
#include <assert.h>
#include <string.h>
#include <float.h>
}

#include <poincare/division.h>
#include <poincare/power.h>
#include <poincare/rational.h>
#include <poincare/tangent.h>
#include <poincare/multiplication.h>
#include <poincare/opposite.h>
#include "layout/fraction_layout.h"

namespace Poincare {

Expression::Type Division::type() const {
  return Type::Division;
}

Expression * Division::clone() const {
  return new Division(m_operands, true);
}

Expression * Division::immediateSimplify() {
  const Expression * powOperands[2] = {operand(1), new Rational(Integer(-1))};
  Power * p = new Power(powOperands, false);
  const Expression * multOperands[2] = {operand(0), p};
  Multiplication * m = new Multiplication(multOperands, 2, false);
  p->simplify();
  detachOperands();
  replaceWith(m, true);
  return m->immediateSimplify();
}

Expression * Division::immediateBeautify() {
  for (int operandIndex = 0; operandIndex < 2; operandIndex++) {
    int k = 0;
    while (true) {
      Expression * sin = factorOfTypeInOperand(Type::Sine, operandIndex, k);
      Expression * cos = factorOfTypeInOperand(Type::Cosine, 1-operandIndex, k);
      if (sin == nullptr || cos == nullptr) {
        break;
      }
      if (sin->operand(0)->compareTo(cos->operand(0)) != 0) {
        continue;
      }
      const Expression * tanOp[1] = {sin->operand(0)};
      Tangent * t = new Tangent(tanOp, true);
      sin->replaceWith(t, true);
      if (cos->parent()->type() == Type::Multiplication) {
        Multiplication * parent = static_cast<Multiplication *>((Expression *)cos->parent());
        parent->removeOperand(cos, true);
        return parent->squashUnaryHierarchy();
      } else if (cos->parent()->type() == Type::Opposite) {
        if (operandIndex == 0) {
          const Expression * oppOperand[1] = {operand(0)};
          Opposite * o = new Opposite(oppOperand, true);
          return replaceWith(o, true);
        } else {
          assert(operandIndex == 1);
          replaceOperand((Expression *)cos->parent(), new Rational(Integer(-1)), true);
        }
      } else {
        if (operandIndex == 0) {
        return replaceWith((Expression *)operand(k), true);
        } else {
          assert(operandIndex == 1);
          replaceOperand(cos, new Rational(Integer(1)), true);
        }
      }
      k++;
    }
  }
  return this;
}

Expression * Division::factorOfTypeInOperand(Type type, int operandIndex, int k) {
  if (operand(operandIndex)->type() == type && k == 0) {
    return (Expression *)operand(operandIndex);
  }
  if (operand(operandIndex)->type() == Type::Multiplication) {
    int counter = -1;
    for (int i = 0; i < operand(operandIndex)->numberOfOperands(); i++) {
      if (operand(operandIndex)->operand(i)->type() == type) {
        counter++;
        if (counter == k) {
          return ((Expression *)operand(operandIndex)->operand(i));
        }
      }
    }
  }
  if (operand(operandIndex)->type() == Type::Opposite) {
    if (operand(operandIndex)->operand(0)->type() == type && k == 0) {
      return ((Expression *)operand(operandIndex)->operand(0));
    } else if (operand(operandIndex)->operand(0)->type() == Type::Multiplication) {
      int counter = -1;
      for (int i = 0; i < operand(operandIndex)->operand(0)->numberOfOperands(); i++) {
        if (operand(operandIndex)->operand(0)->operand(i)->type() == type) {
          counter++;
          if (counter == k) {
            return ((Expression *)operand(operandIndex)->operand(i));
          }
        }
      }

    }
  }
  return nullptr;
}

template<typename T>
Complex<T> Division::compute(const Complex<T> c, const Complex<T> d) {
  T norm = d.a()*d.a() + d.b()*d.b();
  /* We handle the case of c and d pure real numbers apart. Even if the complex
   * division is mathematically correct on real numbers, it requires more
   * operations and is thus more likely to propagate errors due to float exact
   * representation. */
  if (d.b() == 0 && c.b() == 0) {
    return Complex<T>::Float(c.a()/d.a());
  }
  return Complex<T>::Cartesian((c.a()*d.a()+c.b()*d.b())/norm, (d.a()*c.b()-c.a()*d.b())/norm);
}

template<typename T> Evaluation<T> * Division::computeOnComplexAndMatrix(const Complex<T> * c, Evaluation<T> * n) {
  Evaluation<T> * inverse = n->createInverse();
  Evaluation<T> * result = Multiplication::computeOnComplexAndMatrix(c, inverse);
  delete inverse;
  return result;
}

template<typename T> Evaluation<T> * Division::computeOnMatrices(Evaluation<T> * m, Evaluation<T> * n) {
  if (m->numberOfColumns() != n->numberOfColumns()) {
    return nullptr;
  }
  Evaluation<T> * inverse = n->createInverse();
  Evaluation<T> * result = Multiplication::computeOnMatrices(m, inverse);
  delete inverse;
  return result;
}

ExpressionLayout * Division::privateCreateLayout(FloatDisplayMode floatDisplayMode, ComplexFormat complexFormat) const {
  assert(floatDisplayMode != FloatDisplayMode::Default);
  assert(complexFormat != ComplexFormat::Default);
  const Expression * numerator = operand(0)->type() == Type::Parenthesis ? operand(0)->operand(0) : operand(0);
  const Expression * denominator = operand(1)->type() == Type::Parenthesis ? operand(1)->operand(0) : operand(1);
  return new FractionLayout(numerator->createLayout(floatDisplayMode, complexFormat), denominator->createLayout(floatDisplayMode, complexFormat));
}

}

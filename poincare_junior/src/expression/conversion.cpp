#include "conversion.h"

#include <poincare_expressions.h>
#include <poincare_junior/src/expression/builtin.h>
#include <poincare_junior/src/expression/float.h>
#include <poincare_junior/src/expression/integer.h>
#include <poincare_junior/src/expression/matrix.h>
#include <poincare_junior/src/expression/rational.h>
#include <poincare_junior/src/expression/simplification.h>
#include <poincare_junior/src/expression/symbol.h>
#include <poincare_junior/src/layout/layoutter.h>
#include <poincare_junior/src/layout/parser.h>
#include <poincare_junior/src/layout/parsing/rack_parser.h>
#include <poincare_junior/src/layout/rack_from_text.h>
#include <poincare_junior/src/layout/serialize.h>
#include <poincare_junior/src/memory/edition_reference.h>
#include <poincare_junior/src/n_ary.h>

#include "poincare_junior/src/memory/type_block.h"

namespace PoincareJ {

Poincare::OExpression ToPoincareExpressionViaParse(const Tree *exp) {
  EditionReference outputLayout = Layoutter::LayoutExpression(exp->clone());
  constexpr size_t bufferSize = 256;
  char buffer[bufferSize];
  *Serialize(outputLayout, buffer, buffer + bufferSize) = 0;
  outputLayout->removeTree();
  return Poincare::OExpression::Parse(buffer, nullptr, false, false);
}

void PushPoincareExpressionViaParse(Poincare::OExpression exp) {
  constexpr size_t bufferSize = 256;
  char buffer[bufferSize];
  exp.serialize(buffer, bufferSize);
  EditionReference inputLayout = RackFromText(buffer);
  RackParser(inputLayout, nullptr).parse();
  inputLayout->removeTree();
  return;
}

Poincare::ComparisonNode::OperatorType ComparisonToOperator(Type type) {
  switch (type) {
    case Type::Equal:
      return Poincare::ComparisonNode::OperatorType::Equal;
    case Type::NotEqual:
      return Poincare::ComparisonNode::OperatorType::NotEqual;
    case Type::Superior:
      return Poincare::ComparisonNode::OperatorType::Superior;
    case Type::Inferior:
      return Poincare::ComparisonNode::OperatorType::Inferior;
    case Type::SuperiorEqual:
      return Poincare::ComparisonNode::OperatorType::SuperiorEqual;
    case Type::InferiorEqual:
      return Poincare::ComparisonNode::OperatorType::InferiorEqual;
    default:
      assert(false);
  }
}

Poincare::OExpression ToPoincareExpression(const Tree *exp) {
  // NOTE: Make sure new Types are handled here.
  Type type = exp->type();

  if (Builtin::IsReservedFunction(exp)) {
    Poincare::OExpression child = ToPoincareExpression(exp->child(0));
    switch (type) {
      case Type::SquareRoot:
        return Poincare::SquareRoot::Builder(child);
      case Type::NthRoot:
        return Poincare::NthRoot::Builder(child,
                                          ToPoincareExpression(exp->child(1)));
      case Type::Cosine:
        return Poincare::Cosine::Builder(child);
      case Type::Sine:
        return Poincare::Sine::Builder(child);
      case Type::Tangent:
        return Poincare::Tangent::Builder(child);
      case Type::ArcCosine:
        return Poincare::ArcCosine::Builder(child);
      case Type::ArcSine:
        return Poincare::ArcSine::Builder(child);
      case Type::ArcTangent:
        return Poincare::ArcTangent::Builder(child);
      case Type::Secant:
        return Poincare::Secant::Builder(child);
      case Type::Cosecant:
        return Poincare::Cosecant::Builder(child);
      case Type::Cotangent:
        return Poincare::Cotangent::Builder(child);
      case Type::ArcSecant:
        return Poincare::ArcSecant::Builder(child);
      case Type::ArcCosecant:
        return Poincare::ArcCosecant::Builder(child);
      case Type::ArcCotangent:
        return Poincare::ArcCotangent::Builder(child);
      case Type::HyperbolicCosine:
        return Poincare::HyperbolicCosine::Builder(child);
      case Type::HyperbolicSine:
        return Poincare::HyperbolicSine::Builder(child);
      case Type::HyperbolicTangent:
        return Poincare::HyperbolicTangent::Builder(child);
      case Type::HyperbolicArcCosine:
        return Poincare::HyperbolicArcCosine::Builder(child);
      case Type::HyperbolicArcSine:
        return Poincare::HyperbolicArcSine::Builder(child);
      case Type::HyperbolicArcTangent:
        return Poincare::HyperbolicArcTangent::Builder(child);
      case Type::Abs:
        return Poincare::AbsoluteValue::Builder(child);
      case Type::Ceiling:
        return Poincare::Ceiling::Builder(child);
      case Type::Floor:
        return Poincare::Floor::Builder(child);
      case Type::FracPart:
        return Poincare::FracPart::Builder(child);
      case Type::Log:
        return Poincare::Logarithm::Builder(child);
      case Type::Logarithm:
        return Poincare::Logarithm::Builder(
            child, ToPoincareExpression(exp->child(1)));
      case Type::Binomial:
        return Poincare::BinomialCoefficient::Builder(
            child, ToPoincareExpression(exp->child(1)));
      case Type::Permute:
        return Poincare::PermuteCoefficient::Builder(
            child, ToPoincareExpression(exp->child(1)));
      case Type::Round:
        return Poincare::Round::Builder(child,
                                        ToPoincareExpression(exp->child(1)));
      case Type::Cross:
        return Poincare::VectorCross::Builder(
            child, ToPoincareExpression(exp->child(1)));
      case Type::Dot:
        return Poincare::VectorDot::Builder(
            child, ToPoincareExpression(exp->child(1)));
      case Type::Det:
        return Poincare::Determinant::Builder(child);
      case Type::Dim:
        return Poincare::Dimension::Builder(child);
      case Type::Identity:
        return Poincare::MatrixIdentity::Builder(child);
      case Type::Inverse:
        return Poincare::MatrixInverse::Builder(child);
      case Type::Ln:
        return Poincare::NaperianLogarithm::Builder(child);
      case Type::Norm:
        return Poincare::AbsoluteValue::Builder(child);
      case Type::Ref:
        return Poincare::MatrixRowEchelonForm::Builder(child);
      case Type::Rref:
        return Poincare::MatrixReducedRowEchelonForm::Builder(child);
      case Type::Trace:
        return Poincare::MatrixTrace::Builder(child);
      case Type::Transpose:
        return Poincare::MatrixTranspose::Builder(child);
      case Type::ComplexArgument:
        return Poincare::ComplexArgument::Builder(child);
      case Type::Conjugate:
        return Poincare::Conjugate::Builder(child);
      case Type::ImaginaryPart:
        return Poincare::ImaginaryPart::Builder(child);
      case Type::RealPart:
        return Poincare::RealPart::Builder(child);
      case Type::PercentSimple:
        return Poincare::PercentSimple::Builder(child);
      case Type::PercentAddition:
        return Poincare::PercentAddition::Builder(
            child, ToPoincareExpression(exp->child(1)));
      case Type::Derivative: {
        Poincare::OExpression symbol = child;
        if (symbol.otype() != Poincare::ExpressionNode::Type::Symbol) {
          return Poincare::Undefined::Builder();
        }
        return Poincare::Derivative::Builder(
            ToPoincareExpression(exp->child(2)),
            static_cast<Poincare::Symbol &>(symbol),
            ToPoincareExpression(exp->child(1)));
      }
      case Type::Integral: {
        Poincare::OExpression symbol = child;
        if (symbol.otype() != Poincare::ExpressionNode::Type::Symbol) {
          return Poincare::Undefined::Builder();
        }
        return Poincare::Integral::Builder(
            ToPoincareExpression(exp->child(3)),
            static_cast<Poincare::Symbol &>(symbol),
            ToPoincareExpression(exp->child(1)),
            ToPoincareExpression(exp->child(2)));
      }
      case Type::Sum: {
        Poincare::OExpression symbol = child;
        if (symbol.otype() != Poincare::ExpressionNode::Type::Symbol) {
          return Poincare::Undefined::Builder();
        }
        return Poincare::Sum::Builder(ToPoincareExpression(exp->child(3)),
                                      static_cast<Poincare::Symbol &>(symbol),
                                      ToPoincareExpression(exp->child(1)),
                                      ToPoincareExpression(exp->child(2)));
      }
      case Type::Product: {
        Poincare::OExpression symbol = child;
        if (symbol.otype() != Poincare::ExpressionNode::Type::Symbol) {
          return Poincare::Undefined::Builder();
        }
        return Poincare::Product::Builder(
            ToPoincareExpression(exp->child(3)),
            static_cast<Poincare::Symbol &>(symbol),
            ToPoincareExpression(exp->child(1)),
            ToPoincareExpression(exp->child(2)));
      }
      case Type::Dependency: {
        assert(exp->child(1)->isSet());
        Poincare::OList listOfDependencies = Poincare::OList::Builder();
        for (const Tree *child : exp->child(1)->children()) {
          listOfDependencies.addChildAtIndexInPlace(ToPoincareExpression(child),
                                                    0, 0);
        }
        return Poincare::Dependency::Builder(
            ToPoincareExpression(exp->child(0)), listOfDependencies);
      }
      case Type::Piecewise: {
        Poincare::List arguments = Poincare::List::Builder();
        int i = 0;
        for (const Tree *child : exp->children()) {
          arguments.addChildAtIndexInPlace(ToPoincareExpression(child), i, i);
          i++;
        }
        return Poincare::PiecewiseOperator::UntypedBuilder(arguments);
      }
      default:
        // TODO: Handle missing Types
        assert(false);
    }
  }

  switch (type) {
    case Type::Addition:
    case Type::Multiplication: {
      Poincare::NAryExpression nary =
          type == Type::Addition ? static_cast<Poincare::NAryExpression>(
                                       Poincare::Addition::Builder())
                                 : Poincare::Multiplication::Builder();
      for (const Tree *child : exp->children()) {
        nary.addChildAtIndexInPlace(ToPoincareExpression(child),
                                    nary.numberOfChildren(),
                                    nary.numberOfChildren());
      }
      return nary;
    }
    case Type::List: {
      Poincare::OList list = Poincare::OList::Builder();
      for (const Tree *child : exp->children()) {
        list.addChildAtIndexInPlace(ToPoincareExpression(child),
                                    list.numberOfChildren(),
                                    list.numberOfChildren());
      }
      return list;
    }
    case Type::Matrix: {
      Poincare::OMatrix mat = Poincare::OMatrix::Builder();
      for (const Tree *child : exp->children()) {
        mat.addChildAtIndexInPlace(ToPoincareExpression(child),
                                   mat.numberOfChildren(),
                                   mat.numberOfChildren());
      }
      mat.setDimensions(Matrix::NumberOfRows(exp),
                        Matrix::NumberOfColumns(exp));
      return mat;
    }
    case Type::Subtraction:
    case Type::Power:
    case Type::PowerMatrix:
    case Type::Division: {
      Poincare::OExpression child0 = ToPoincareExpression(exp->child(0));
      Poincare::OExpression child1 = ToPoincareExpression(exp->child(1));
      Poincare::OExpression result;
      if (type == Type::Subtraction) {
        result = Poincare::Subtraction::Builder(child0, child1);
      } else if (type == Type::Division) {
        result = Poincare::Division::Builder(child0, child1);
      } else {
        result = Poincare::Power::Builder(child0, child1);
      }
      return result;
    }
    case Type::Zero:
    case Type::MinusOne:
    case Type::One:
    case Type::Two:
    case Type::IntegerShort:
    case Type::IntegerPosBig:
    case Type::IntegerNegBig:
    case Type::Half:
    case Type::RationalShort:
    case Type::RationalPosBig:
    case Type::RationalNegBig:
    case Type::SingleFloat:
    case Type::DoubleFloat:
    case Type::Decimal:
    case Type::Unit:
      return ToPoincareExpressionViaParse(exp);
    case Type::UserSymbol: {
      return Poincare::Symbol::Builder(Symbol::GetName(exp),
                                       Symbol::Length(exp));
    }
    case Type::ComplexI:
      return Poincare::Constant::ComplexIBuilder();
    case Type::Pi:
      return Poincare::Constant::PiBuilder();
    case Type::ExponentialE:
      return Poincare::Constant::ExponentialEBuilder();
    case Type::Infinity:
      return Poincare::Infinity::Builder(false);
    case Type::Factorial:
      return Poincare::Factorial::Builder(ToPoincareExpression(exp->child(0)));
    case Type::Nonreal:
      return Poincare::Nonreal::Builder();
    case Type::Opposite:
      return Poincare::Opposite::Builder(ToPoincareExpression(exp->child(0)));
    case Type::Equal:
    case Type::NotEqual:
    case Type::Superior:
    case Type::Inferior:
    case Type::SuperiorEqual:
    case Type::InferiorEqual:
      return Poincare::Comparison::Builder(ToPoincareExpression(exp->child(0)),
                                           ComparisonToOperator(type),
                                           ToPoincareExpression(exp->child(1)));
    case Type::UserFunction:
      return Poincare::Function::Builder(Symbol::GetName(exp),
                                         Symbol::Length(exp),
                                         ToPoincareExpression(exp->child(0)));
    case Type::UserSequence:
      return Poincare::Sequence::Builder(Symbol::GetName(exp),
                                         Symbol::Length(exp),
                                         ToPoincareExpression(exp->child(0)));
    case Type::Set:
    case Type::Polynomial:
    default:
      return Poincare::Undefined::Builder();
  }
}

void PushPoincareExpression(Poincare::OExpression exp) {
  using OT = Poincare::ExpressionNode::Type;
  switch (exp.otype()) {
    case OT::AbsoluteValue:
      SharedEditionPool->push(Type::Abs);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::Ceiling:
      SharedEditionPool->push(Type::Ceiling);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::Floor:
      SharedEditionPool->push(Type::Floor);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::FracPart:
      SharedEditionPool->push(Type::FracPart);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::Factorial:
      SharedEditionPool->push(Type::Factorial);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::Opposite:
      SharedEditionPool->push(Type::Opposite);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::SignFunction:
      SharedEditionPool->push(Type::Sign);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::SquareRoot:
      SharedEditionPool->push(Type::SquareRoot);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::NthRoot:
      SharedEditionPool->push(Type::NthRoot);
      PushPoincareExpression(exp.childAtIndex(0));
      return PushPoincareExpression(exp.childAtIndex(1));
    case OT::Cosine:
      SharedEditionPool->push(Type::Cosine);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::Sine:
      SharedEditionPool->push(Type::Sine);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::Tangent:
      SharedEditionPool->push(Type::Tangent);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::ArcCosine:
      SharedEditionPool->push(Type::ArcCosine);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::ArcSine:
      SharedEditionPool->push(Type::ArcSine);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::ArcTangent:
      SharedEditionPool->push(Type::ArcTangent);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::Cosecant:
      SharedEditionPool->push(Type::Cosecant);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::Secant:
      SharedEditionPool->push(Type::Secant);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::Cotangent:
      SharedEditionPool->push(Type::Cotangent);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::ArcCosecant:
      SharedEditionPool->push(Type::ArcCosecant);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::ArcSecant:
      SharedEditionPool->push(Type::ArcSecant);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::ArcCotangent:
      SharedEditionPool->push(Type::ArcCotangent);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::HyperbolicCosine:
      SharedEditionPool->push(Type::HyperbolicCosine);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::HyperbolicSine:
      SharedEditionPool->push(Type::HyperbolicSine);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::HyperbolicTangent:
      SharedEditionPool->push(Type::HyperbolicTangent);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::HyperbolicArcCosine:
      SharedEditionPool->push(Type::HyperbolicArcCosine);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::HyperbolicArcSine:
      SharedEditionPool->push(Type::HyperbolicArcSine);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::HyperbolicArcTangent:
      SharedEditionPool->push(Type::HyperbolicArcTangent);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::NaperianLogarithm:
      SharedEditionPool->push(Type::Ln);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::Dimension:
      SharedEditionPool->push(Type::Dim);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::Determinant:
      SharedEditionPool->push(Type::Det);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::MatrixIdentity:
      SharedEditionPool->push(Type::Identity);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::MatrixInverse:
      SharedEditionPool->push(Type::Inverse);
      return PushPoincareExpression(exp.childAtIndex(0));
    // case OT::: // FIXME Norm is AbsoluteValue
    // SharedEditionPool->push(Type::Norm);
    // return PushPoincareExpression(exp.childAtIndex(0));
    case OT::MatrixTrace:
      SharedEditionPool->push(Type::Trace);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::MatrixTranspose:
      SharedEditionPool->push(Type::Transpose);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::MatrixRowEchelonForm:
      SharedEditionPool->push(Type::Ref);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::MatrixReducedRowEchelonForm:
      SharedEditionPool->push(Type::Rref);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::BinomialCoefficient:
      SharedEditionPool->push(Type::Binomial);
      PushPoincareExpression(exp.childAtIndex(0));
      return PushPoincareExpression(exp.childAtIndex(1));
    case OT::PermuteCoefficient:
      SharedEditionPool->push(Type::Permute);
      PushPoincareExpression(exp.childAtIndex(0));
      return PushPoincareExpression(exp.childAtIndex(1));
    case OT::Round:
      SharedEditionPool->push(Type::Round);
      PushPoincareExpression(exp.childAtIndex(0));
      return PushPoincareExpression(exp.childAtIndex(1));
    case OT::VectorCross:
      SharedEditionPool->push(Type::Cross);
      PushPoincareExpression(exp.childAtIndex(0));
      return PushPoincareExpression(exp.childAtIndex(1));
    case OT::VectorDot:
      SharedEditionPool->push(Type::Dot);
      PushPoincareExpression(exp.childAtIndex(0));
      return PushPoincareExpression(exp.childAtIndex(1));
    case OT::VectorNorm:
      SharedEditionPool->push(Type::Norm);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::ComplexArgument:
      SharedEditionPool->push(Type::ComplexArgument);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::Conjugate:
      SharedEditionPool->push(Type::Conjugate);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::ImaginaryPart:
      SharedEditionPool->push(Type::ImaginaryPart);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::RealPart:
      SharedEditionPool->push(Type::RealPart);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::Factor:
      SharedEditionPool->push(Type::Factor);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::PercentSimple:
      SharedEditionPool->push(Type::PercentSimple);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::PercentAddition:
      SharedEditionPool->push(Type::PercentAddition);
      PushPoincareExpression(exp.childAtIndex(0));
      return PushPoincareExpression(exp.childAtIndex(1));
    case OT::DivisionQuotient:
      SharedEditionPool->push(Type::Quotient);
      PushPoincareExpression(exp.childAtIndex(0));
      return PushPoincareExpression(exp.childAtIndex(1));
    case OT::DivisionRemainder:
      SharedEditionPool->push(Type::Remainder);
      PushPoincareExpression(exp.childAtIndex(0));
      return PushPoincareExpression(exp.childAtIndex(1));
    case OT::ListElement:
      SharedEditionPool->push(Type::ListElement);
      // list is last in poincare
      PushPoincareExpression(exp.childAtIndex(1));
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::ListSlice:
      SharedEditionPool->push(Type::ListSlice);
      // list is last in poincare
      PushPoincareExpression(exp.childAtIndex(2));
      PushPoincareExpression(exp.childAtIndex(0));
      return PushPoincareExpression(exp.childAtIndex(1));
    case OT::ListSort:
      SharedEditionPool->push(Type::ListSort);
      return PushPoincareExpression(exp.childAtIndex(0));

    case OT::ListMean:
    case OT::ListVariance:
    case OT::ListStandardDeviation:
    case OT::ListSampleStandardDeviation:
    case OT::ListMedian: {
      switch (exp.otype()) {
        case OT::ListMean:
          SharedEditionPool->push(Type::Mean);
          break;
        case OT::ListVariance:
          SharedEditionPool->push(Type::Variance);
          break;
        case OT::ListSampleStandardDeviation:
          SharedEditionPool->push(Type::SampleStdDev);
          break;
        case OT::ListStandardDeviation:
          SharedEditionPool->push(Type::StdDev);
          break;
        case OT::ListMedian:
          SharedEditionPool->push(Type::Median);
          break;
        case OT::ListSum:
          SharedEditionPool->push(Type::ListSum);
          break;
        case OT::ListProduct:
          SharedEditionPool->push(Type::ListProduct);
          break;
      }
      PushPoincareExpression(exp.childAtIndex(0));
      if (exp.numberOfChildren() == 2) {
        PushPoincareExpression(exp.childAtIndex(1));
      } else {
        (1_e)->clone();
      }
      return;
    }
    case OT::ListSum:
      SharedEditionPool->push(Type::ListSum);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::ListProduct:
      SharedEditionPool->push(Type::ListProduct);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::ListMinimum:
      SharedEditionPool->push(Type::Minimum);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::ListMaximum:
      SharedEditionPool->push(Type::Maximum);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::Store:
      SharedEditionPool->push(Type::Store);
      PushPoincareExpression(exp.childAtIndex(0));
      return PushPoincareExpression(exp.childAtIndex(1));
    case OT::UnitConvert:
      SharedEditionPool->push(Type::UnitConversion);
      PushPoincareExpression(exp.childAtIndex(0));
      return PushPoincareExpression(exp.childAtIndex(1));
    case OT::OBoolean:
      SharedEditionPool->push(static_cast<Poincare::OBoolean &>(exp).value()
                                  ? Type::True
                                  : Type::False);
      return;
    case OT::LogicalOperatorNot:
      SharedEditionPool->push(Type::LogicalNot);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::BinaryLogicalOperator: {
      Poincare::BinaryLogicalOperator op =
          static_cast<Poincare::BinaryLogicalOperator &>(exp);
      Type type;
      switch (op.operatorType()) {
        case Poincare::BinaryLogicalOperatorNode::OperatorType::And:
          type = Type::LogicalAnd;
          break;
        case Poincare::BinaryLogicalOperatorNode::OperatorType::Or:
          type = Type::LogicalOr;
          break;
        case Poincare::BinaryLogicalOperatorNode::OperatorType::Xor:
          type = Type::LogicalXor;
          break;
        case Poincare::BinaryLogicalOperatorNode::OperatorType::Nand:
          type = Type::LogicalNand;
          break;
        case Poincare::BinaryLogicalOperatorNode::OperatorType::Nor:
          type = Type::LogicalNor;
          break;
        default:
          assert(false);
      }
      SharedEditionPool->push(type);
      PushPoincareExpression(exp.childAtIndex(0));
      return PushPoincareExpression(exp.childAtIndex(1));
    }
    case OT::OPoint:
      SharedEditionPool->push(Type::Point);
      PushPoincareExpression(exp.childAtIndex(0));
      return PushPoincareExpression(exp.childAtIndex(1));
    case OT::MixedFraction:
      SharedEditionPool->push(Type::MixedFraction);
      PushPoincareExpression(exp.childAtIndex(0));
      return PushPoincareExpression(exp.childAtIndex(1));
    case OT::Logarithm:
      if (exp.numberOfChildren() == 2) {
        SharedEditionPool->push(Type::Logarithm);
        PushPoincareExpression(exp.childAtIndex(0));
        PushPoincareExpression(exp.childAtIndex(1));
      } else {
        assert(exp.numberOfChildren() == 1);
        SharedEditionPool->push(Type::Log);
        PushPoincareExpression(exp.childAtIndex(0));
      }
      return;
    case OT::ListSequence:
      SharedEditionPool->push(Type::ListSequence);
      PushPoincareExpression(exp.childAtIndex(1));
      PushPoincareExpression(exp.childAtIndex(2));
      PushPoincareExpression(exp.childAtIndex(0));
      return;
    case OT::Derivative:
      if (exp.childAtIndex(3).isOne()) {
        SharedEditionPool->push(Type::Derivative);
        PushPoincareExpression(exp.childAtIndex(1));
        PushPoincareExpression(exp.childAtIndex(2));
        PushPoincareExpression(exp.childAtIndex(0));
      } else {
        SharedEditionPool->push(Type::NthDerivative);
        PushPoincareExpression(exp.childAtIndex(1));
        PushPoincareExpression(exp.childAtIndex(2));
        PushPoincareExpression(exp.childAtIndex(3));
        PushPoincareExpression(exp.childAtIndex(0));
      }
      return;
    case OT::Integral:
      SharedEditionPool->push(Type::Integral);
      PushPoincareExpression(exp.childAtIndex(1));
      PushPoincareExpression(exp.childAtIndex(2));
      PushPoincareExpression(exp.childAtIndex(3));
      PushPoincareExpression(exp.childAtIndex(0));
      return;
    case OT::Sum:
    case OT::Product:
      SharedEditionPool->push(exp.otype() == OT::Sum ? Type::Sum
                                                     : Type::Product);
      PushPoincareExpression(exp.childAtIndex(1));
      PushPoincareExpression(exp.childAtIndex(2));
      PushPoincareExpression(exp.childAtIndex(3));
      PushPoincareExpression(exp.childAtIndex(0));
      return;
    case OT::Comparison:
      if (exp.numberOfChildren() > 2) {
        // This will not work semantically but is sufficient for createLayout
        Poincare::Comparison c = static_cast<Poincare::Comparison &>(exp);
        Poincare::OExpression e = Poincare::Comparison::Builder(
            Poincare::Comparison::Builder(
                c.childAtIndex(0), c.operatorAtIndex(0), c.childAtIndex(1)),
            c.operatorAtIndex(1), c.childAtIndex(2));
        return PushPoincareExpression(e);
      }
      // TODO: Handle comparisons better
      assert(exp.numberOfChildren() == 2);
    case OT::Addition:
    case OT::PiecewiseOperator:
    case OT::Multiplication:
    case OT::OList:
    case OT::Subtraction:
    case OT::Division:
    case OT::Power:
    case OT::OMatrix:
    case OT::GreatCommonDivisor:
    case OT::LeastCommonMultiple:
      switch (exp.otype()) {
        case OT::Addition:
          SharedEditionPool->push<Type::Addition>(exp.numberOfChildren());
          break;
        case OT::Multiplication:
          SharedEditionPool->push<Type::Multiplication>(exp.numberOfChildren());
          break;
        case OT::OList:
          SharedEditionPool->push<Type::List>(exp.numberOfChildren());
          break;
        case OT::PiecewiseOperator:
          SharedEditionPool->push<Type::Piecewise>(exp.numberOfChildren());
          break;
        case OT::GreatCommonDivisor:
          SharedEditionPool->push(Type::GCD);
          SharedEditionPool->push(exp.numberOfChildren());
          break;
        case OT::LeastCommonMultiple:
          SharedEditionPool->push(Type::LCM);
          SharedEditionPool->push(exp.numberOfChildren());
          break;
        case OT::Comparison: {
          Poincare::Comparison c = static_cast<Poincare::Comparison &>(exp);
          Type type;
          switch (c.operatorAtIndex(0)) {
            case Poincare::ComparisonNode::OperatorType::Equal:
              type = Type::Equal;
              break;
            case Poincare::ComparisonNode::OperatorType::NotEqual:
              type = Type::NotEqual;
              break;
            case Poincare::ComparisonNode::OperatorType::Inferior:
              type = Type::Inferior;
              break;
            case Poincare::ComparisonNode::OperatorType::InferiorEqual:
              type = Type::InferiorEqual;
              break;
            case Poincare::ComparisonNode::OperatorType::Superior:
              type = Type::Superior;
              break;
            case Poincare::ComparisonNode::OperatorType::SuperiorEqual:
              type = Type::SuperiorEqual;
              break;
            default:
              assert(false);
          }
          SharedEditionPool->push(type);
          break;
        }
        case OT::Subtraction:
          SharedEditionPool->push(Type::Subtraction);
          break;
        case OT::Division:
          SharedEditionPool->push(Type::Division);
          break;
        case OT::Power:
          SharedEditionPool->push(Type::Power);
          break;
        case OT::OMatrix:
          SharedEditionPool->push<Type::Matrix>(
              static_cast<Poincare::OMatrix &>(exp).numberOfRows(),
              static_cast<Poincare::OMatrix &>(exp).numberOfColumns());
          break;
        default:
          assert(false);
      }
      for (int i = 0; i < exp.numberOfChildren(); i++) {
        PushPoincareExpression(exp.childAtIndex(i));
      }
      return;
    case OT::Nonreal:
      SharedEditionPool->push(Type::Nonreal);
      return;
    case OT::Rational:
    case OT::BasedInteger:
    case OT::OUnit:
    case OT::ConstantPhysics:
      return PushPoincareExpressionViaParse(exp);
    case OT::Decimal: {
      Poincare::Decimal d = static_cast<Poincare::Decimal &>(exp);
      if (d.node()->isNegative()) {
        SharedEditionPool->push(Type::Opposite);
      }
      int numberOfDigits = Poincare::Integer::NumberOfBase10DigitsWithoutSign(
          d.node()->unsignedMantissa());
      int8_t exponent = numberOfDigits - 1 - d.node()->exponent();
      SharedEditionPool->push<Type::Decimal>(exponent);
      Poincare::Integer mantissa = d.node()->unsignedMantissa();
      char buffer[100];
      mantissa.serialize(buffer, 100);
      UTF8Decoder decoder(buffer);
      IntegerHandler::Parse(decoder, OMG::Base::Decimal).pushOnEditionPool();
      return;
    }
    case OT::Sequence:
    case OT::Function:
    case OT::Symbol: {
      Poincare::Symbol s = static_cast<Poincare::Symbol &>(exp);
      Tree *t = SharedEditionPool->push<Type::UserSymbol>(
          s.name(), (exp.otype() == OT::Sequence ? 1 : strlen(s.name())) + 1);
      if (exp.otype() == OT::Function) {
        *t->block() = Type::UserFunction;
        PushPoincareExpression(exp.childAtIndex(0));
      }
      if (exp.otype() == OT::Sequence) {
        *t->block() = Type::UserSequence;
        PushPoincareExpression(exp.childAtIndex(0));
      }
      return;
    }
    case OT::ConstantMaths: {
      Poincare::Constant c = static_cast<Poincare::Constant &>(exp);
      if (c.isExponentialE()) {
        SharedEditionPool->push(Type::ExponentialE);
      } else if (c.isPi()) {
        SharedEditionPool->push(Type::Pi);
      } else if (c.isComplexI()) {
        SharedEditionPool->push(Type::ComplexI);
      } else {
        SharedEditionPool->push(Type::Undefined);
      }
      return;
    }
    case OT::Float:
      SharedEditionPool->push<Type::SingleFloat>(
          static_cast<Poincare::Float<float> &>(exp).value());
      return;
    case OT::Double:
      SharedEditionPool->push<Type::DoubleFloat>(
          static_cast<Poincare::Float<double> &>(exp).value());
      return;
    case OT::Infinity: {
      if (exp.isPositive(nullptr) == Poincare::TrinaryBoolean::False) {
        SharedEditionPool->push(Type::Opposite);
      }
      SharedEditionPool->push(Type::Infinity);
      return;
    }
    case OT::DistributionDispatcher: {
      SharedEditionPool->push(Type::Distribution);
      SharedEditionPool->push(exp.numberOfChildren());
      Poincare::DistributionDispatcher dd =
          static_cast<Poincare::DistributionDispatcher &>(exp);
      SharedEditionPool->push(static_cast<uint8_t>(dd.distributionType()));
      SharedEditionPool->push(static_cast<uint8_t>(dd.methodType()));
      for (int i = 0; i < exp.numberOfChildren(); i++) {
        PushPoincareExpression(exp.childAtIndex(i));
      }
      return;
    }
    case OT::Random:
      SharedEditionPool->push(Type::Random);
      SharedEditionPool->push(0);  // seed
      return;
    case OT::Randint:
      SharedEditionPool->push(Type::RandInt);
      SharedEditionPool->push(0);  // seed
      if (exp.numberOfChildren() == 2) {
        PushPoincareExpression(exp.childAtIndex(0));
        PushPoincareExpression(exp.childAtIndex(1));
      } else {
        (1_e)->clone();
        PushPoincareExpression(exp.childAtIndex(1));
      }
      return;
    case OT::RandintNoRepeat:
      SharedEditionPool->push(Type::RandIntNoRep);
      SharedEditionPool->push(0);  // seed
      PushPoincareExpression(exp.childAtIndex(0));
      PushPoincareExpression(exp.childAtIndex(1));
      PushPoincareExpression(exp.childAtIndex(2));
      return;
    case OT::Undefined:
      SharedEditionPool->push(Type::Undefined);
      return;
    case OT::EmptyExpression:
      SharedEditionPool->push(Type::Empty);
      return;
    case OT::Parenthesis:
      SharedEditionPool->push(Type::Parenthesis);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::JuniorExpression: {
      SharedEditionPool->clone(
          static_cast<Poincare::JuniorExpression &>(exp).tree());
      return;
    }
    default:
      assert(false);
  }
}

Tree *FromPoincareExpression(Poincare::OExpression exp) {
  Tree *node = Tree::FromBlocks(SharedEditionPool->lastBlock());
  PushPoincareExpression(exp);
  return node;
}

}  // namespace PoincareJ

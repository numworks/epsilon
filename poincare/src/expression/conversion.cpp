#include "conversion.h"

#include <poincare/old/poincare_expressions.h>
#include <poincare/src/layout/layoutter.h>
#include <poincare/src/layout/parser.h>
#include <poincare/src/layout/parsing/rack_parser.h>
#include <poincare/src/layout/rack_from_text.h>
#include <poincare/src/layout/serialize.h>
#include <poincare/src/memory/n_ary.h>
#include <poincare/src/memory/tree_ref.h>
#include <poincare/src/memory/type_block.h>

#include "builtin.h"
#include "dependency.h"
#include "float.h"
#include "integer.h"
#include "matrix.h"
#include "rational.h"
#include "simplification.h"
#include "symbol.h"

namespace Poincare::Internal {

Poincare::OExpression ToPoincareExpressionViaParse(const Tree* exp) {
  TreeRef outputLayout = Layoutter::LayoutExpression(exp->clone());
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
  TreeRef inputLayout = RackFromText(buffer);
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

Poincare::OExpression ToPoincareExpression(const Tree* exp) {
  // NOTE: Make sure new Types are handled here.
  Type type = exp->type();

  if (Builtin::IsReservedFunction(exp)) {
    Poincare::OExpression child = ToPoincareExpression(exp->child(0));
    switch (type) {
      case Type::Sqrt:
        return Poincare::SquareRoot::Builder(child);
      case Type::Root:
        return Poincare::NthRoot::Builder(child,
                                          ToPoincareExpression(exp->child(1)));
      case Type::Cos:
        return Poincare::Cosine::Builder(child);
      case Type::Sin:
        return Poincare::Sine::Builder(child);
      case Type::Tan:
        return Poincare::Tangent::Builder(child);
      case Type::ACos:
        return Poincare::ArcCosine::Builder(child);
      case Type::ASin:
        return Poincare::ArcSine::Builder(child);
      case Type::ATan:
        return Poincare::ArcTangent::Builder(child);
      case Type::Sec:
        return Poincare::Secant::Builder(child);
      case Type::Csc:
        return Poincare::Cosecant::Builder(child);
      case Type::Cot:
        return Poincare::Cotangent::Builder(child);
      case Type::ASec:
        return Poincare::ArcSecant::Builder(child);
      case Type::ACsc:
        return Poincare::ArcCosecant::Builder(child);
      case Type::ACot:
        return Poincare::ArcCotangent::Builder(child);
      case Type::CosH:
        return Poincare::HyperbolicCosine::Builder(child);
      case Type::SinH:
        return Poincare::HyperbolicSine::Builder(child);
      case Type::TanH:
        return Poincare::HyperbolicTangent::Builder(child);
      case Type::ArCosH:
        return Poincare::HyperbolicArcCosine::Builder(child);
      case Type::ArSinH:
        return Poincare::HyperbolicArcSine::Builder(child);
      case Type::ArTanH:
        return Poincare::HyperbolicArcTangent::Builder(child);
      case Type::Abs:
        return Poincare::AbsoluteValue::Builder(child);
      case Type::Ceil:
        return Poincare::Ceiling::Builder(child);
      case Type::Floor:
        return Poincare::Floor::Builder(child);
      case Type::Frac:
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
      case Type::Arg:
        return Poincare::ComplexArgument::Builder(child);
      case Type::Conj:
        return Poincare::Conjugate::Builder(child);
      case Type::Im:
        return Poincare::ImaginaryPart::Builder(child);
      case Type::Re:
        return Poincare::RealPart::Builder(child);
      case Type::PercentSimple:
        return Poincare::PercentSimple::Builder(child);
      case Type::PercentAddition:
        return Poincare::PercentAddition::Builder(
            child, ToPoincareExpression(exp->child(1)));
      case Type::Diff: {
        Poincare::OExpression symbol = child;
        if (symbol.otype() != Poincare::ExpressionNode::Type::Symbol) {
          return Poincare::Undefined::Builder();
        }
        return Poincare::Derivative::Builder(
            ToPoincareExpression(exp->child(2)),
            static_cast<Poincare::Symbol&>(symbol),
            ToPoincareExpression(exp->child(1)));
      }
      case Type::Integral: {
        Poincare::OExpression symbol = child;
        if (symbol.otype() != Poincare::ExpressionNode::Type::Symbol) {
          return Poincare::Undefined::Builder();
        }
        return Poincare::Integral::Builder(
            ToPoincareExpression(exp->child(3)),
            static_cast<Poincare::Symbol&>(symbol),
            ToPoincareExpression(exp->child(1)),
            ToPoincareExpression(exp->child(2)));
      }
      case Type::Sum: {
        Poincare::OExpression symbol = child;
        if (symbol.otype() != Poincare::ExpressionNode::Type::Symbol) {
          return Poincare::Undefined::Builder();
        }
        return Poincare::Sum::Builder(ToPoincareExpression(exp->child(3)),
                                      static_cast<Poincare::Symbol&>(symbol),
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
            static_cast<Poincare::Symbol&>(symbol),
            ToPoincareExpression(exp->child(1)),
            ToPoincareExpression(exp->child(2)));
      }
      case Type::Dependency: {
        assert(Dependency::Dependencies(exp)->isSet());
        Poincare::OList listOfDependencies = Poincare::OList::Builder();
        for (const Tree* child : Dependency::Dependencies(exp)->children()) {
          listOfDependencies.addChildAtIndexInPlace(ToPoincareExpression(child),
                                                    0, 0);
        }
        return Poincare::Dependency::Builder(
            ToPoincareExpression(Dependency::Main(exp)), listOfDependencies);
      }
      case Type::Piecewise: {
        Poincare::List arguments = Poincare::List::Builder();
        int i = 0;
        for (const Tree* child : exp->children()) {
          arguments.addChildAtIndexInPlace(ToPoincareExpression(child), i, i);
          i++;
        }
        return Poincare::PiecewiseOperator::UntypedBuilder(arguments);
      }
      default:
        // Unhandled types.
        assert(false);
    }
  }

  switch (type) {
    case Type::Add:
    case Type::Mult: {
      Poincare::NAryExpression nary =
          type == Type::Add ? static_cast<Poincare::NAryExpression>(
                                  Poincare::Addition::Builder())
                            : Poincare::Multiplication::Builder();
      for (const Tree* child : exp->children()) {
        nary.addChildAtIndexInPlace(ToPoincareExpression(child),
                                    nary.numberOfChildren(),
                                    nary.numberOfChildren());
      }
      return nary;
    }
    case Type::List: {
      Poincare::OList list = Poincare::OList::Builder();
      for (const Tree* child : exp->children()) {
        list.addChildAtIndexInPlace(ToPoincareExpression(child),
                                    list.numberOfChildren(),
                                    list.numberOfChildren());
      }
      return list;
    }
    case Type::Matrix: {
      Poincare::OMatrix mat = Poincare::OMatrix::Builder();
      for (const Tree* child : exp->children()) {
        mat.addChildAtIndexInPlace(ToPoincareExpression(child),
                                   mat.numberOfChildren(),
                                   mat.numberOfChildren());
      }
      mat.setDimensions(Matrix::NumberOfRows(exp),
                        Matrix::NumberOfColumns(exp));
      return mat;
    }
    case Type::Sub:
    case Type::Pow:
    case Type::PowMatrix:
    case Type::Div: {
      Poincare::OExpression child0 = ToPoincareExpression(exp->child(0));
      Poincare::OExpression child1 = ToPoincareExpression(exp->child(1));
      Poincare::OExpression result;
      if (type == Type::Sub) {
        result = Poincare::Subtraction::Builder(child0, child1);
      } else if (type == Type::Div) {
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
    case Type::EulerE:
      return Poincare::Constant::ExponentialEBuilder();
    case Type::Inf:
      return Poincare::Infinity::Builder(false);
    case Type::Fact:
      return Poincare::Factorial::Builder(ToPoincareExpression(exp->child(0)));
    case Type::NonReal:
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
      SharedTreeStack->push(Type::Abs);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::Ceiling:
      SharedTreeStack->push(Type::Ceil);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::Floor:
      SharedTreeStack->push(Type::Floor);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::FracPart:
      SharedTreeStack->push(Type::Frac);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::Factorial:
      SharedTreeStack->push(Type::Fact);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::Opposite:
      SharedTreeStack->push(Type::Opposite);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::SignFunction:
      SharedTreeStack->push(Type::Sign);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::SquareRoot:
      SharedTreeStack->push(Type::Sqrt);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::NthRoot:
      SharedTreeStack->push(Type::Root);
      PushPoincareExpression(exp.childAtIndex(0));
      return PushPoincareExpression(exp.childAtIndex(1));
    case OT::Cosine:
      SharedTreeStack->push(Type::Cos);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::Sine:
      SharedTreeStack->push(Type::Sin);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::Tangent:
      SharedTreeStack->push(Type::Tan);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::ArcCosine:
      SharedTreeStack->push(Type::ACos);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::ArcSine:
      SharedTreeStack->push(Type::ASin);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::ArcTangent:
      SharedTreeStack->push(Type::ATan);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::Cosecant:
      SharedTreeStack->push(Type::Csc);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::Secant:
      SharedTreeStack->push(Type::Sec);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::Cotangent:
      SharedTreeStack->push(Type::Cot);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::ArcCosecant:
      SharedTreeStack->push(Type::ACsc);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::ArcSecant:
      SharedTreeStack->push(Type::ASec);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::ArcCotangent:
      SharedTreeStack->push(Type::ACot);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::HyperbolicCosine:
      SharedTreeStack->push(Type::CosH);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::HyperbolicSine:
      SharedTreeStack->push(Type::SinH);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::HyperbolicTangent:
      SharedTreeStack->push(Type::TanH);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::HyperbolicArcCosine:
      SharedTreeStack->push(Type::ArCosH);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::HyperbolicArcSine:
      SharedTreeStack->push(Type::ArSinH);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::HyperbolicArcTangent:
      SharedTreeStack->push(Type::ArTanH);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::NaperianLogarithm:
      SharedTreeStack->push(Type::Ln);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::Dimension:
      SharedTreeStack->push(Type::Dim);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::Determinant:
      SharedTreeStack->push(Type::Det);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::MatrixIdentity:
      SharedTreeStack->push(Type::Identity);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::MatrixInverse:
      SharedTreeStack->push(Type::Inverse);
      return PushPoincareExpression(exp.childAtIndex(0));
    // case OT::: // FIXME Norm is AbsoluteValue
    // SharedTreeStack->push(Type::Norm);
    // return PushPoincareExpression(exp.childAtIndex(0));
    case OT::MatrixTrace:
      SharedTreeStack->push(Type::Trace);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::MatrixTranspose:
      SharedTreeStack->push(Type::Transpose);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::MatrixRowEchelonForm:
      SharedTreeStack->push(Type::Ref);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::MatrixReducedRowEchelonForm:
      SharedTreeStack->push(Type::Rref);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::BinomialCoefficient:
      SharedTreeStack->push(Type::Binomial);
      PushPoincareExpression(exp.childAtIndex(0));
      return PushPoincareExpression(exp.childAtIndex(1));
    case OT::PermuteCoefficient:
      SharedTreeStack->push(Type::Permute);
      PushPoincareExpression(exp.childAtIndex(0));
      return PushPoincareExpression(exp.childAtIndex(1));
    case OT::Round:
      SharedTreeStack->push(Type::Round);
      PushPoincareExpression(exp.childAtIndex(0));
      return PushPoincareExpression(exp.childAtIndex(1));
    case OT::VectorCross:
      SharedTreeStack->push(Type::Cross);
      PushPoincareExpression(exp.childAtIndex(0));
      return PushPoincareExpression(exp.childAtIndex(1));
    case OT::VectorDot:
      SharedTreeStack->push(Type::Dot);
      PushPoincareExpression(exp.childAtIndex(0));
      return PushPoincareExpression(exp.childAtIndex(1));
    case OT::VectorNorm:
      SharedTreeStack->push(Type::Norm);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::ComplexArgument:
      SharedTreeStack->push(Type::Arg);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::Conjugate:
      SharedTreeStack->push(Type::Conj);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::ImaginaryPart:
      SharedTreeStack->push(Type::Im);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::RealPart:
      SharedTreeStack->push(Type::Re);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::Factor:
      SharedTreeStack->push(Type::Factor);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::PercentSimple:
      SharedTreeStack->push(Type::PercentSimple);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::PercentAddition:
      SharedTreeStack->push(Type::PercentAddition);
      PushPoincareExpression(exp.childAtIndex(0));
      return PushPoincareExpression(exp.childAtIndex(1));
    case OT::DivisionQuotient:
      SharedTreeStack->push(Type::Quo);
      PushPoincareExpression(exp.childAtIndex(0));
      return PushPoincareExpression(exp.childAtIndex(1));
    case OT::DivisionRemainder:
      SharedTreeStack->push(Type::Rem);
      PushPoincareExpression(exp.childAtIndex(0));
      return PushPoincareExpression(exp.childAtIndex(1));
    case OT::ListElement:
      SharedTreeStack->push(Type::ListElement);
      // list is last in poincare
      PushPoincareExpression(exp.childAtIndex(1));
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::ListSlice:
      SharedTreeStack->push(Type::ListSlice);
      // list is last in poincare
      PushPoincareExpression(exp.childAtIndex(2));
      PushPoincareExpression(exp.childAtIndex(0));
      return PushPoincareExpression(exp.childAtIndex(1));
    case OT::ListSort:
      SharedTreeStack->push(Type::ListSort);
      return PushPoincareExpression(exp.childAtIndex(0));

    case OT::ListMean:
    case OT::ListVariance:
    case OT::ListStandardDeviation:
    case OT::ListSampleStandardDeviation:
    case OT::ListMedian: {
      switch (exp.otype()) {
        case OT::ListMean:
          SharedTreeStack->push(Type::Mean);
          break;
        case OT::ListVariance:
          SharedTreeStack->push(Type::Variance);
          break;
        case OT::ListSampleStandardDeviation:
          SharedTreeStack->push(Type::SampleStdDev);
          break;
        case OT::ListStandardDeviation:
          SharedTreeStack->push(Type::StdDev);
          break;
        case OT::ListMedian:
          SharedTreeStack->push(Type::Median);
          break;
        case OT::ListSum:
          SharedTreeStack->push(Type::ListSum);
          break;
        case OT::ListProduct:
          SharedTreeStack->push(Type::ListProduct);
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
      SharedTreeStack->push(Type::ListSum);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::ListProduct:
      SharedTreeStack->push(Type::ListProduct);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::ListMinimum:
      SharedTreeStack->push(Type::Min);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::ListMaximum:
      SharedTreeStack->push(Type::Max);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::Store:
      SharedTreeStack->push(Type::Store);
      PushPoincareExpression(exp.childAtIndex(0));
      return PushPoincareExpression(exp.childAtIndex(1));
    case OT::UnitConvert:
      SharedTreeStack->push(Type::UnitConversion);
      PushPoincareExpression(exp.childAtIndex(0));
      return PushPoincareExpression(exp.childAtIndex(1));
    case OT::OBoolean:
      SharedTreeStack->push(static_cast<Poincare::OBoolean&>(exp).value()
                                ? Type::True
                                : Type::False);
      return;
    case OT::LogicalOperatorNot:
      SharedTreeStack->push(Type::LogicalNot);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::BinaryLogicalOperator: {
      Poincare::BinaryLogicalOperator op =
          static_cast<Poincare::BinaryLogicalOperator&>(exp);
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
      SharedTreeStack->push(type);
      PushPoincareExpression(exp.childAtIndex(0));
      return PushPoincareExpression(exp.childAtIndex(1));
    }
    case OT::OPoint:
      SharedTreeStack->push(Type::Point);
      PushPoincareExpression(exp.childAtIndex(0));
      return PushPoincareExpression(exp.childAtIndex(1));
    case OT::MixedFraction:
      SharedTreeStack->push(Type::MixedFraction);
      PushPoincareExpression(exp.childAtIndex(0));
      return PushPoincareExpression(exp.childAtIndex(1));
    case OT::Logarithm:
      if (exp.numberOfChildren() == 2) {
        SharedTreeStack->push(Type::Logarithm);
        PushPoincareExpression(exp.childAtIndex(0));
        PushPoincareExpression(exp.childAtIndex(1));
      } else {
        assert(exp.numberOfChildren() == 1);
        SharedTreeStack->push(Type::Log);
        PushPoincareExpression(exp.childAtIndex(0));
      }
      return;
    case OT::ListSequence:
      SharedTreeStack->push(Type::ListSequence);
      PushPoincareExpression(exp.childAtIndex(1));
      PushPoincareExpression(exp.childAtIndex(2));
      PushPoincareExpression(exp.childAtIndex(0));
      return;
    case OT::Derivative:
      if (exp.childAtIndex(3).isOne()) {
        SharedTreeStack->push(Type::Diff);
        PushPoincareExpression(exp.childAtIndex(1));
        PushPoincareExpression(exp.childAtIndex(2));
        PushPoincareExpression(exp.childAtIndex(0));
      } else {
        SharedTreeStack->push(Type::NthDiff);
        PushPoincareExpression(exp.childAtIndex(1));
        PushPoincareExpression(exp.childAtIndex(2));
        PushPoincareExpression(exp.childAtIndex(3));
        PushPoincareExpression(exp.childAtIndex(0));
      }
      return;
    case OT::Integral:
      SharedTreeStack->push(Type::Integral);
      PushPoincareExpression(exp.childAtIndex(1));
      PushPoincareExpression(exp.childAtIndex(2));
      PushPoincareExpression(exp.childAtIndex(3));
      PushPoincareExpression(exp.childAtIndex(0));
      return;
    case OT::Sum:
    case OT::Product:
      SharedTreeStack->push(exp.otype() == OT::Sum ? Type::Sum : Type::Product);
      PushPoincareExpression(exp.childAtIndex(1));
      PushPoincareExpression(exp.childAtIndex(2));
      PushPoincareExpression(exp.childAtIndex(3));
      PushPoincareExpression(exp.childAtIndex(0));
      return;
    case OT::Comparison:
      if (exp.numberOfChildren() > 2) {
        // This will not work semantically but is sufficient for createLayout
        Poincare::Comparison c = static_cast<Poincare::Comparison&>(exp);
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
          SharedTreeStack->push<Type::Add>(exp.numberOfChildren());
          break;
        case OT::Multiplication:
          SharedTreeStack->push<Type::Mult>(exp.numberOfChildren());
          break;
        case OT::OList:
          SharedTreeStack->push<Type::List>(exp.numberOfChildren());
          break;
        case OT::PiecewiseOperator:
          SharedTreeStack->push<Type::Piecewise>(exp.numberOfChildren());
          break;
        case OT::GreatCommonDivisor:
          SharedTreeStack->push(Type::GCD);
          SharedTreeStack->push(exp.numberOfChildren());
          break;
        case OT::LeastCommonMultiple:
          SharedTreeStack->push(Type::LCM);
          SharedTreeStack->push(exp.numberOfChildren());
          break;
        case OT::Comparison: {
          Poincare::Comparison c = static_cast<Poincare::Comparison&>(exp);
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
          SharedTreeStack->push(type);
          break;
        }
        case OT::Subtraction:
          SharedTreeStack->push(Type::Sub);
          break;
        case OT::Division:
          SharedTreeStack->push(Type::Div);
          break;
        case OT::Power:
          SharedTreeStack->push(Type::Pow);
          break;
        case OT::OMatrix:
          SharedTreeStack->push<Type::Matrix>(
              static_cast<Poincare::OMatrix&>(exp).numberOfRows(),
              static_cast<Poincare::OMatrix&>(exp).numberOfColumns());
          break;
        default:
          assert(false);
      }
      for (int i = 0; i < exp.numberOfChildren(); i++) {
        PushPoincareExpression(exp.childAtIndex(i));
      }
      return;
    case OT::Nonreal:
      SharedTreeStack->push(Type::NonReal);
      return;
    case OT::Rational:
    case OT::BasedInteger:
    case OT::OUnit:
    case OT::ConstantPhysics:
      return PushPoincareExpressionViaParse(exp);
    case OT::Decimal: {
      Poincare::Decimal d = static_cast<Poincare::Decimal&>(exp);
      if (d.node()->isNegative()) {
        SharedTreeStack->push(Type::Opposite);
      }
      int numberOfDigits = Poincare::Integer::NumberOfBase10DigitsWithoutSign(
          d.node()->unsignedMantissa());
      int8_t exponent = numberOfDigits - 1 - d.node()->exponent();
      SharedTreeStack->push<Type::Decimal>(exponent);
      Poincare::Integer mantissa = d.node()->unsignedMantissa();
      char buffer[100];
      mantissa.serialize(buffer, 100);
      UTF8Decoder decoder(buffer);
      IntegerHandler::Parse(decoder, OMG::Base::Decimal).pushOnTreeStack();
      return;
    }
    case OT::Sequence:
    case OT::Function:
    case OT::Symbol: {
      Poincare::Symbol s = static_cast<Poincare::Symbol&>(exp);
      Tree* t = SharedTreeStack->push<Type::UserSymbol>(
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
      Poincare::Constant c = static_cast<Poincare::Constant&>(exp);
      if (c.isExponentialE()) {
        SharedTreeStack->push(Type::EulerE);
      } else if (c.isPi()) {
        SharedTreeStack->push(Type::Pi);
      } else if (c.isComplexI()) {
        SharedTreeStack->push(Type::ComplexI);
      } else {
        KUndefUnhandled->clone();
      }
      return;
    }
    case OT::Float:
      SharedTreeStack->push<Type::SingleFloat>(
          static_cast<Poincare::Float<float>&>(exp).value());
      return;
    case OT::Double:
      SharedTreeStack->push<Type::DoubleFloat>(
          static_cast<Poincare::Float<double>&>(exp).value());
      return;
    case OT::Infinity: {
      if (exp.isPositive(nullptr) == Poincare::TrinaryBoolean::False) {
        SharedTreeStack->push(Type::Opposite);
      }
      SharedTreeStack->push(Type::Inf);
      return;
    }
    case OT::DistributionDispatcher: {
      SharedTreeStack->push(Type::Distribution);
      SharedTreeStack->push(exp.numberOfChildren());
      Poincare::DistributionDispatcher dd =
          static_cast<Poincare::DistributionDispatcher&>(exp);
      SharedTreeStack->push(static_cast<uint8_t>(dd.distributionType()));
      SharedTreeStack->push(static_cast<uint8_t>(dd.methodType()));
      for (int i = 0; i < exp.numberOfChildren(); i++) {
        PushPoincareExpression(exp.childAtIndex(i));
      }
      return;
    }
    case OT::Random:
      SharedTreeStack->push(Type::Random);
      SharedTreeStack->push(0);  // seed
      return;
    case OT::Randint:
      SharedTreeStack->push(Type::RandInt);
      SharedTreeStack->push(0);  // seed
      if (exp.numberOfChildren() == 2) {
        PushPoincareExpression(exp.childAtIndex(0));
        PushPoincareExpression(exp.childAtIndex(1));
      } else {
        (1_e)->clone();
        PushPoincareExpression(exp.childAtIndex(1));
      }
      return;
    case OT::RandintNoRepeat:
      SharedTreeStack->push(Type::RandIntNoRep);
      SharedTreeStack->push(0);  // seed
      PushPoincareExpression(exp.childAtIndex(0));
      PushPoincareExpression(exp.childAtIndex(1));
      PushPoincareExpression(exp.childAtIndex(2));
      return;
    case OT::Undefined:
      KUndef->clone();
      return;
    case OT::EmptyExpression:
      SharedTreeStack->push(Type::Empty);
      return;
    case OT::Parenthesis:
      SharedTreeStack->push(Type::Parenthesis);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::JuniorExpression: {
      SharedTreeStack->clone(
          static_cast<Poincare::JuniorExpression&>(exp).tree());
      return;
    }
    default:
      assert(false);
  }
}

Tree* FromPoincareExpression(Poincare::OExpression exp) {
  Tree* node = Tree::FromBlocks(SharedTreeStack->lastBlock());
  PushPoincareExpression(exp);
  return node;
}

}  // namespace Poincare::Internal

#include <poincare_expressions.h>
#include <poincare_junior/include/expression.h>
#include <poincare_junior/src/expression/approximation.h>
#include <poincare_junior/src/expression/builtin.h>
#include <poincare_junior/src/expression/constant.h>
#include <poincare_junior/src/expression/integer.h>
#include <poincare_junior/src/expression/rational.h>
#include <poincare_junior/src/expression/simplification.h>
#include <poincare_junior/src/expression/symbol.h>
#include <poincare_junior/src/layout/parser.h>
#include <poincare_junior/src/memory/cache_pool.h>
#include <poincare_junior/src/memory/edition_reference.h>
#include <poincare_junior/src/n_ary.h>

namespace PoincareJ {

void Expression::ConvertBuiltinToLayout(EditionReference layoutParent,
                                        Node *expression) {
  assert(Builtin::IsBuiltin(expression->type()));
  EditionPool *editionPool = EditionPool::sharedEditionPool();
  UTF8Decoder decoder(Builtin::Name(expression->type()).mainAlias());
  CodePoint codePoint = decoder.nextCodePoint();
  while (codePoint != UCodePointNull) {
    NAry::AddChild(
        layoutParent,
        editionPool->push<BlockType::CodePointLayout, CodePoint>(codePoint));
    codePoint = decoder.nextCodePoint();
  }
  EditionReference parenthesis =
      editionPool->push<BlockType::ParenthesisLayout>();
  EditionReference newParent = editionPool->push<BlockType::RackLayout>(0);
  NAry::AddChild(layoutParent, parenthesis);
  for (int j = 0; j < expression->numberOfChildren(); j++) {
    if (j != 0) {
      NAry::AddChild(
          newParent,
          editionPool->push<BlockType::CodePointLayout, CodePoint>(','));
    }
    // No parentheses within builtin parameters
    ConvertExpressionToLayout(newParent, expression->nextNode(), false);
  }
}

void Expression::ConvertIntegerHandlerToLayout(EditionReference layoutParent,
                                               IntegerHandler handler) {
  EditionPool *editionPool = EditionPool::sharedEditionPool();
  if (handler.strictSign() == StrictSign::Negative) {
    NAry::AddChild(
        layoutParent,
        editionPool->push<BlockType::CodePointLayout, CodePoint>('-'));
  }
  uint64_t value = 0;
  int numberOfDigits = handler.numberOfDigits();
  if (numberOfDigits > 8) {
    assert(false);
    NAry::AddChild(
        layoutParent,
        editionPool->push<BlockType::CodePointLayout, CodePoint>('?'));
    return;
  }
  for (int i = numberOfDigits - 1; i >= 0; i--) {
    value = value * (UINT8_MAX + 1) + handler.digits()[i];
  }
  int firstInsertedIndex = layoutParent.numberOfChildren();
  do {
    uint8_t digit = value % 10;
    value /= 10;
    NAry::AddChildAtIndex(
        layoutParent,
        editionPool->push<BlockType::CodePointLayout, CodePoint>('0' + digit),
        firstInsertedIndex);
  } while (value > 0);
}

void Expression::ConvertInfixOperatorToLayout(EditionReference layoutParent,
                                              Node *expression) {
  BlockType type = expression->type();
  assert(type == BlockType::Addition || type == BlockType::Multiplication ||
         type == BlockType::Subtraction);
  CodePoint codepoint = (type == BlockType::Addition)         ? '+'
                        : (type == BlockType::Multiplication) ? '*'
                                                              : '-';
  int childNumber = expression->numberOfChildren();
  for (int i = 0; i < childNumber; i++) {
    if (i > 0) {
      NAry::AddChild(
          layoutParent,
          EditionPool::sharedEditionPool()
              ->push<BlockType::CodePointLayout, CodePoint>(codepoint));
    }
    // 2*(x+y) or x-(y+z)
    bool allowParentheses = (type == BlockType::Multiplication) ||
                            ((type == BlockType::Subtraction) && (i == 1));
    ConvertExpressionToLayout(layoutParent, expression->nextNode(),
                              allowParentheses);
  }
}

void Expression::ConvertPowerOrDivisionToLayout(EditionReference layoutParent,
                                                Node *expression) {
  EditionPool *editionPool = EditionPool::sharedEditionPool();
  BlockType type = expression->type();
  /* Once first child has been converted, this will point to second child. */
  expression = expression->nextNode();
  EditionReference createdLayout;
  // No parentheses in Fraction roots and Power index.
  if (type == BlockType::Division) {
    createdLayout = editionPool->push<BlockType::FractionLayout>();
    ConvertExpressionToLayout(editionPool->push<BlockType::RackLayout>(0),
                              expression, false);
  } else {
    assert(type == BlockType::Power);
    ConvertExpressionToLayout(layoutParent, expression);
    createdLayout = editionPool->push<BlockType::VerticalOffsetLayout>();
  }
  ConvertExpressionToLayout(editionPool->push<BlockType::RackLayout>(0),
                            expression, false);
  NAry::AddChild(layoutParent, createdLayout);
}

Poincare::Expression Expression::ToPoincareExpression(const Node *exp) {
  BlockType type = exp->type();

  if (Builtin::IsBuiltin(type)) {
    Poincare::Expression child = ToPoincareExpression(exp->childAtIndex(0));
    switch (type) {
      case BlockType::Cosine:
        return Poincare::Cosine::Builder(child);
      case BlockType::Sine:
        return Poincare::Sine::Builder(child);
      case BlockType::Tangent:
        return Poincare::Tangent::Builder(child);
      case BlockType::ArcCosine:
        return Poincare::ArcCosine::Builder(child);
      case BlockType::ArcSine:
        return Poincare::ArcSine::Builder(child);
      case BlockType::ArcTangent:
        return Poincare::ArcTangent::Builder(child);
      case BlockType::Abs:
        return Poincare::AbsoluteValue::Builder(child);
      case BlockType::Log:
        return Poincare::Logarithm::Builder(child);
      case BlockType::Logarithm:
        return Poincare::Logarithm::Builder(
            child, ToPoincareExpression(exp->childAtIndex(1)));
    }
  }

  switch (type) {
    case BlockType::Addition: {
      case BlockType::Multiplication:
        Poincare::NAryExpression nary =
            type == BlockType::Addition ? static_cast<Poincare::NAryExpression>(
                                              Poincare::Addition::Builder())
                                        : Poincare::Multiplication::Builder();
        for (const Node *child : exp->children()) {
          nary.addChildAtIndexInPlace(ToPoincareExpression(child),
                                      nary.numberOfChildren(),
                                      nary.numberOfChildren());
        }
        return nary;
    }
    case BlockType::Subtraction:
    case BlockType::Power:
    case BlockType::Division: {
      Poincare::Expression child0 = ToPoincareExpression(exp->childAtIndex(0));
      Poincare::Expression child1 = ToPoincareExpression(exp->childAtIndex(1));
      Poincare::Expression result;
      if (type == BlockType::Subtraction) {
        result = Poincare::Subtraction::Builder(child0, child1);
      } else if (type == BlockType::Division) {
        result = Poincare::Division::Builder(child0, child1);
      } else {
        result = Poincare::Power::Builder(child0, child1);
      }
      return result;
    }
    case BlockType::Zero:
    case BlockType::MinusOne:
    case BlockType::One:
    case BlockType::Two:
    case BlockType::IntegerShort:
    case BlockType::IntegerPosBig:
    case BlockType::IntegerNegBig:
      return Poincare::Rational::Builder(Integer::Handler(exp).to<double>());
    case BlockType::Half:
    case BlockType::RationalShort:
    case BlockType::RationalPosBig:
    case BlockType::RationalNegBig:
      return Poincare::Rational::Builder(
          Rational::Numerator(exp).to<double>(),
          Rational::Denominator(exp).to<double>());
    case BlockType::Float:
      return Poincare::Float<double>::Builder(Approximation::To<double>(exp));
    case BlockType::Ln:
      return Poincare::NaperianLogarithm::Builder(
          ToPoincareExpression(exp->childAtIndex(0)));
    case BlockType::UserSymbol: {
      char buffer[20];
      Symbol::GetName(exp, buffer, std::size(buffer));
      return Poincare::Symbol::Builder(buffer, Symbol::Length(exp));
    }
    case BlockType::Constant: {
      if (Constant::Type(exp) == Constant::Type::Pi) {
        return Poincare::Constant::PiBuilder();
      }
      if (Constant::Type(exp) == Constant::Type::E) {
        return Poincare::Constant::ExponentialEBuilder();
      }
      return Poincare::Undefined::Builder();
    }
    case BlockType::Factorial:
      return Poincare::Factorial::Builder(
          ToPoincareExpression(exp->childAtIndex(0)));
    case BlockType::UserFunction:
    case BlockType::UserSequence:
    case BlockType::Set:
    case BlockType::List:
    case BlockType::Polynomial:
    default:
      return Poincare::Undefined::Builder();
  }
}

void Expression::PushPoincareExpression(Poincare::Expression exp) {
  using OT = Poincare::ExpressionNode::Type;
  EditionPool *pool = EditionPool::sharedEditionPool();
  switch (exp.type()) {
    case OT::Parenthesis:
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::AbsoluteValue:
      pool->pushBlock(BlockType::Abs);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::Opposite:
      pool->push<BlockType::Multiplication>(2);
      pool->pushBlock(BlockType::MinusOne);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::Cosine:
      pool->pushBlock(BlockType::Cosine);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::Sine:
      pool->pushBlock(BlockType::Sine);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::Tangent:
      pool->pushBlock(BlockType::Tangent);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::ArcCosine:
      pool->pushBlock(BlockType::ArcCosine);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::ArcSine:
      pool->pushBlock(BlockType::ArcSine);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::ArcTangent:
      pool->pushBlock(BlockType::ArcTangent);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::Logarithm:
      if (exp.numberOfChildren() == 2) {
        pool->pushBlock(BlockType::Logarithm);
        PushPoincareExpression(exp.childAtIndex(0));
        PushPoincareExpression(exp.childAtIndex(1));
      } else {
        assert(exp.numberOfChildren() == 1);
        pool->pushBlock(BlockType::Log);
        PushPoincareExpression(exp.childAtIndex(0));
      }
      return;
    case OT::NaperianLogarithm:
      pool->pushBlock(BlockType::Ln);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::Addition:
    case OT::Multiplication:
    case OT::Subtraction:
    case OT::Division:
    case OT::Power:
      switch (exp.type()) {
        case OT::Addition:
          pool->push<BlockType::Addition>(exp.numberOfChildren());
          break;
        case OT::Multiplication:
          pool->push<BlockType::Multiplication>(exp.numberOfChildren());
          break;
        case OT::Subtraction:
          pool->pushBlock(BlockType::Subtraction);
          break;
        case OT::Division:
          pool->pushBlock(BlockType::Division);
          break;
        case OT::Power:
          pool->pushBlock(BlockType::Power);
          break;
      }
      for (int i = 0; i < exp.numberOfChildren(); i++) {
        PushPoincareExpression(exp.childAtIndex(i));
      }
      return;
    case OT::Rational: {
      Poincare::Rational rat = static_cast<Poincare::Rational &>(exp);
      int num = rat.signedIntegerNumerator().approximate<double>();
      int den = rat.integerDenominator().approximate<double>();
      Rational::Push(IntegerHandler(num), IntegerHandler(den));
      return;
    }
    case OT::BasedInteger: {
      Poincare::BasedInteger i = static_cast<Poincare::BasedInteger &>(exp);
      int num = i.doubleApproximation();
      Rational::Push(IntegerHandler(num), IntegerHandler(1));
      return;
    }
    case OT::Float: {
      Poincare::Float<float> f = static_cast<Poincare::Float<float> &>(exp);
      pool->push<BlockType::Float>(f.value());
      return;
    }
    case OT::Symbol: {
      Poincare::Symbol s = static_cast<Poincare::Symbol &>(exp);
      pool->push<BlockType::UserSymbol>(s.name(), strlen(s.name()));
      return;
    }
    case OT::ConstantMaths: {
      Poincare::Constant c = static_cast<Poincare::Constant &>(exp);
      if (c.isExponentialE()) {
        pool->push<BlockType::Constant>(u'e');
      } else {
        pool->pushBlock(BlockType::Undefined);
      }
      return;
    }
    default:
      pool->pushBlock(BlockType::Undefined);
  }
}

Node *Expression::FromPoincareExpression(Poincare::Expression exp) {
  Node *node = Node::FromBlocks(EditionPool::sharedEditionPool()->lastBlock());
  PushPoincareExpression(exp);
  return node;
}

// Remove expression while converting it to a layout in layoutParent
void Expression::ConvertExpressionToLayout(EditionReference layoutParent,
                                           Node *expression,
                                           bool allowParentheses) {
  /* TODO: ConvertExpressionToLayout is a very temporary implementation and must
   *      be improved in the future. */
  assert(Layout::IsHorizontal(layoutParent));
  BlockType type = expression->type();
  EditionPool *editionPool = EditionPool::sharedEditionPool();

  switch (type) {
    case BlockType::Addition:
    case BlockType::Subtraction:
      // Add Parentheses if allowed and needed.
      assert(expression->numberOfChildren() > 1);
      if (allowParentheses) {
        EditionReference parenthesis =
            editionPool->push<BlockType::ParenthesisLayout>();
        EditionReference newParent =
            editionPool->push<BlockType::RackLayout>(0);
        NAry::AddChild(layoutParent, parenthesis);
        layoutParent = newParent;
      }
      // continue
    case BlockType::Multiplication:
      ConvertInfixOperatorToLayout(layoutParent, expression);
      break;
    case BlockType::Power:
    case BlockType::Division:
      ConvertPowerOrDivisionToLayout(layoutParent, expression);
      break;
    case BlockType::Zero:
    case BlockType::MinusOne:
    case BlockType::One:
    case BlockType::Two:
    case BlockType::IntegerShort:
    case BlockType::IntegerPosBig:
    case BlockType::IntegerNegBig:
      ConvertIntegerHandlerToLayout(layoutParent, Integer::Handler(expression));
      break;
    case BlockType::Half:
    case BlockType::RationalShort:
    case BlockType::RationalPosBig:
    case BlockType::RationalNegBig: {
      EditionReference createdLayout =
          editionPool->push<BlockType::FractionLayout>();
      ConvertIntegerHandlerToLayout(editionPool->push<BlockType::RackLayout>(0),
                                    Rational::Numerator(expression));
      ConvertIntegerHandlerToLayout(editionPool->push<BlockType::RackLayout>(0),
                                    Rational::Denominator(expression));
      NAry::AddChild(layoutParent, createdLayout);
      break;
    }
    case BlockType::Factorial:
      ConvertExpressionToLayout(layoutParent, expression->nextNode());
      NAry::AddChild(
          layoutParent,
          editionPool->push<BlockType::CodePointLayout, CodePoint>('!'));
      break;
    case BlockType::Constant:
      NAry::AddChild(layoutParent,
                     editionPool->push<BlockType::CodePointLayout, CodePoint>(
                         Constant::ToCodePoint(Constant::Type(expression))));
      break;
    case BlockType::UserSymbol:
      assert(*reinterpret_cast<const uint8_t *>(expression->block() + 1) == 1);
      NAry::AddChild(
          layoutParent,
          editionPool->push<BlockType::CodePointLayout, CodePoint>(
              *reinterpret_cast<const char *>(expression->block() + 2)));
      break;
    case BlockType::UserFunction:
    case BlockType::UserSequence:
    case BlockType::Float:
    case BlockType::Set:
    case BlockType::List:
    case BlockType::Polynomial:
    default:
      if (Builtin::IsBuiltin(type)) {
        ConvertBuiltinToLayout(layoutParent, expression);
      } else {
        assert(false);
        NAry::AddChild(
            layoutParent,
            editionPool->push<BlockType::CodePointLayout, CodePoint>('?'));
        break;
      }
  }
  // Children have already been removed.
  expression->removeNode();
}

EditionReference Expression::EditionPoolExpressionToLayout(Node *expression) {
  assert(expression->block()->isExpression());
  /* expression lives before layoutParent in the EditionPool and will be
   * destroyed in the process. An EditionReference is necessary to keep track of
   * layoutParent's root. */
  EditionReference layoutParent =
      EditionPool::sharedEditionPool()->push<BlockType::RackLayout>(0);
  // No parentheses on root layout.
  ConvertExpressionToLayout(layoutParent, expression, false);
  return layoutParent;
}

Expression Expression::Parse(const char *textInput) {
  return Expression(
      [](const char *text) {
        EditionReference layout = Layout::EditionPoolTextToLayout(text);
        Parser::Parse(layout);
        layout.removeTree();
      },
      textInput);
}

Expression Expression::Parse(const Layout *layout) {
  return Expression(
      [](Node *node) {
        Parser::Parse(node);
        node->removeTree();
      },
      layout);
}

Expression Expression::CreateSimplifyReduction(void *expressionAddress) {
  return Expression(
      [](Node *tree) {
        EditionReference reference(tree);
        Simplification::Simplify(&reference);
      },
      Node::FromBlocks(static_cast<const TypeBlock *>(expressionAddress)));
}

Layout Expression::toLayout() const {
  return Layout([](Node *node) { EditionPoolExpressionToLayout(node); }, this);
}

float Expression::approximate() const {
  float res;
  send(
      [](const Node *tree, void *res) {
        float *result = static_cast<float *>(res);
        *result = Approximation::To<float>(tree);
      },
      &res);
  return res;
}

}  // namespace PoincareJ

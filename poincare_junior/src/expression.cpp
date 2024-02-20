#include <poincare_junior/include/expression.h>
#include <poincare_junior/include/layout.h>
#include <poincare_junior/src/expression/approximation.h>
#include <poincare_junior/src/expression/projection.h>
#include <poincare_junior/src/expression/simplification.h>
#include <poincare_junior/src/layout/parser.h>
#include <poincare_junior/src/memory/cache_pool.h>

namespace PoincareJ {

Expression Expression::Parse(const char *textInput) {
  return Expression(
      [](const char *text) {
        EditionReference layout = Layout::EditionPoolTextToLayout(text);
        Parser::Parse(layout);
        layout->removeTree();
      },
      textInput);
}

Expression Expression::Parse(const Layout *layout) {
  return Expression(
      [](Tree *node) {
        Parser::Parse(node);
        node->removeTree();
      },
      layout);
}

Expression Expression::Simplify(const Expression *input) {
  return Expression(
      [](Tree *input) {
        Simplification::Simplify(input, Projection::ContextFromSettings());
      },
      input);
}

Expression Expression::CreateSimplifyReduction(void *expressionAddress) {
  return Expression(
      [](Tree *tree) { Simplification::Simplify(tree); },
      Tree::FromBlocks(static_cast<const TypeBlock *>(expressionAddress)));
}

Expression Expression::Approximate(const Expression *input) {
  return Expression(
      [](Tree *input) {
        Approximation::RootTreeToTree<float>(
            input, Projection::ContextFromSettings().m_angleUnit,
            Projection::ContextFromSettings().m_complexFormat);
        input->removeTree();
      },
      input);
}

template <typename T>
T Expression::approximate() const {
  return Approximation::RootTreeTo<T>(getTree());
}

Expression Expression::FromPoincareExpression(const Poincare::Expression *exp) {
  return Expression(
      [](const void *data) {
        const Poincare::Expression *expression =
            static_cast<const Poincare::Expression *>(data);
        PoincareJ::Expression::FromPoincareExpression(*expression);
      },
      exp, sizeof(Poincare::Expression));
}

template float Expression::approximate<float>() const;
template double Expression::approximate<double>() const;

}  // namespace PoincareJ

#include <poincare_junior/include/expression.h>
#include <poincare_junior/include/layout.h>
#include <poincare_junior/src/expression/approximation.h>
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
  return Expression([](Tree *input) { Simplification::Simplify(input); },
                    input);
}

Expression Expression::CreateSimplifyReduction(void *expressionAddress) {
  return Expression(
      [](Tree *tree) { Simplification::Simplify(tree); },
      Tree::FromBlocks(static_cast<const TypeBlock *>(expressionAddress)));
}

template <typename T>
T Expression::approximate() const {
  T res;
  send(
      [](const Tree *tree, void *res) {
        T *result = static_cast<T *>(res);
        *result = Approximation::RootTreeTo<T>(tree);
      },
      &res);
  return res;
}

Poincare::Expression Expression::toPoincareExpression() const {
  Poincare::Expression res;
  send(
      [](const Tree *tree, void *res) {
        Poincare::Expression *result = static_cast<Poincare::Expression *>(res);
        *result = ToPoincareExpression(tree);
      },
      &res);
  return res;
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

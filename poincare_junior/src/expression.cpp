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

Expression Expression::CreateSimplifyReduction(void *expressionAddress) {
  return Expression(
      [](Tree *tree) {
        EditionReference reference(tree);
        Simplification::Simplify(reference);
      },
      Tree::FromBlocks(static_cast<const TypeBlock *>(expressionAddress)));
}

template <typename T>
T Expression::approximate() const {
  T res;
  send(
      [](const Tree *tree, void *res) {
        T *result = static_cast<T *>(res);
        *result = Approximation::To<T>(tree);
      },
      &res);
  return res;
}

template float Expression::approximate<float>() const;
template double Expression::approximate<double>() const;

}  // namespace PoincareJ

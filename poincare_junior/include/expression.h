#ifndef POINCARE_EXPRESSION_H
#define POINCARE_EXPRESSION_H

#include <poincare_junior/include/layout.h>
#include <poincare_junior/src/expression/integer.h>
#include <poincare_junior/src/memory/edition_reference.h>
#include <poincare_junior/src/memory/reference.h>

namespace PoincareJ {

class Expression final : public Reference {
 public:
  Expression(const Node* tree) : Reference(tree) {
    assert(tree->block()->isExpression());
  }
  using Reference::Reference;
  // TODO : Delete this method and adapt tests ?
  static Expression Parse(const char* text);
  static Expression Parse(const Layout* layout);
  static Expression CreateSimplifyReduction(void* treeAddress);
  Layout toLayout() const;
  float approximate() const;

  static EditionReference EditionPoolExpressionToLayout(Node* node);

 private:
  static void ConvertBuiltinToLayout(EditionReference layoutParent,
                                     EditionReference expressionReference);
  static void ConvertIntegerHandlerToLayout(EditionReference layoutParent,
                                            IntegerHandler handler);
  static void ConvertInfixOperatorToLayout(
      EditionReference layoutParent, EditionReference expressionReference);
  static void ConvertPowerOrDivisionToLayout(
      EditionReference layoutParent, EditionReference expressionReference);
  static void ConvertExpressionToLayout(EditionReference layoutParent,
                                        EditionReference expressionReference);
};

static_assert(sizeof(Expression) == sizeof(Reference));

}  // namespace PoincareJ

#endif

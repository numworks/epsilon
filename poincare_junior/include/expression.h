#ifndef POINCARE_EXPRESSION_H
#define POINCARE_EXPRESSION_H

#include <poincare/expression.h>
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

  static EditionReference EditionPoolExpressionToLayout(Node* expression);
  static Poincare::Expression ToPoincareExpression(const Node* exp);
  static Node* FromPoincareExpression(Poincare::Expression exp);

 private:
  static void ConvertBuiltinToLayout(EditionReference layoutParent,
                                     Node* expression);
  static void ConvertIntegerHandlerToLayout(EditionReference layoutParent,
                                            IntegerHandler handler);
  static void ConvertInfixOperatorToLayout(EditionReference layoutParent,
                                           Node* expression);
  static void ConvertPowerOrDivisionToLayout(EditionReference layoutParent,
                                             Node* expression);
  static void ConvertExpressionToLayout(EditionReference layoutParent,
                                        Node* expression,
                                        bool allowParentheses = true);
  static void PushPoincareExpression(Poincare::Expression exp);
};

static_assert(sizeof(Expression) == sizeof(Reference));

}  // namespace PoincareJ

#endif

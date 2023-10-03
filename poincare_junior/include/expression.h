#ifndef POINCARE_EXPRESSION_H
#define POINCARE_EXPRESSION_H

#include <poincare/expression.h>
#include <poincare_junior/src/memory/reference.h>

namespace PoincareJ {

class Layout;

class Expression final : public Reference {
 public:
  Expression(const Tree* tree) : Reference(tree) {
    assert(tree->type().isExpression());
  }
  using Reference::Reference;
  // TODO : Delete this method and adapt tests ?
  static Expression Parse(const char* text);
  static Expression Parse(const Layout* layout);
  static Expression CreateSimplifyReduction(void* treeAddress);
  float approximate() const;

  static Poincare::Expression ToPoincareExpression(const Tree* exp);
  static Tree* FromPoincareExpression(Poincare::Expression exp);

 private:
  static void PushPoincareExpression(Poincare::Expression exp);
};

static_assert(sizeof(Expression) == sizeof(Reference));

}  // namespace PoincareJ

#endif

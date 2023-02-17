#ifndef POINCARE_EXPRESSION_H
#define POINCARE_EXPRESSION_H

#include <poincare_junior/src/memory/reference.h>
#include <poincare_junior/src/memory/edition_reference.h>
#include <poincare_junior/include/layout.h>

namespace PoincareJ {

class Expression final : public Reference {
public:
  Expression(const Node tree) : Reference(tree) { assert(tree.block()->isExpression()); }
  using Reference::Reference;
  // TODO : Delete this method and adapt tests ?
  static Expression Parse(const char * text);
  static Expression Parse(const Layout * layout);
  static Expression CreateBasicReduction(void * treeAddress);
  Layout toLayout() const;
  float approximate() const;
private:
  static EditionReference EditionPoolExpressionToLayout(Node node);
  static EditionReference EditionPoolLayoutToExpression(Node node);
};

static_assert(sizeof(Expression) == sizeof(Reference));

}

#endif


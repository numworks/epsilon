#include "print.h"
#include <poincare_junior/include/expression.h>
#include <poincare_junior/include/layout.h>
#include <poincare_junior/src/expression/constructor.h>
#include <poincare_junior/src/layout/constructor.h>
#include <poincare_junior/src/layout/render.h>
#include <kandinsky/ion_context.h>

using namespace PoincareJ;

QUIZ_CASE(pcj_layout_creation) {
  // text -> Layout
  Layout l1 = Layout::Parse("-1+2*3");
  // text -> Expression -> Layout
  Expression e1 = Expression::Parse("-1+2*3");
  Layout l2 = e1.toLayout();
  assert(l2.treeIsIdenticalTo(l1));
  // expression node -> Expression -> Layout
  Expression e2 = Expression(Add(-1_e, Mult(2_e, 3_e)));
  Layout l3 = e2.toLayout();
  // layout Node -> Layout
  assert(l3.treeIsIdenticalTo(l1));
  // constexpr tree -> Layout
  Layout l4 = Layout("-1+2*3"_l);
  assert(l4.treeIsIdenticalTo(l1));
}

QUIZ_CASE(pcj_layout_render) {
  KDContext * ctx = KDIonContext::SharedContext;
  Layout l = Layout(
    RackL(
      "1+"_l,
      ParenthesisL(
        RackL(
          "2*"_l,
          ParenthesisL(
            RackL(
              "1+"_l,
              FracL("1"_l, "2"_l)
            )
          )
        )
      ),
      VertOffL("2"_l),
      "-2"_l
    )
  );
  l.draw(ctx, KDPoint(10,100), KDFont::Size::Large);
}

#include <poincare/src/expression/k_tree.h>
#include <poincare/src/expression/list.h>

#include "helper.h"

using namespace Poincare::Internal;

QUIZ_CASE(pcj_simplification_list_bubble_up) {
  // Bubble-up with no reduction to test the bubble-up itself
  Tree* l1 = KPow(2_e, KList(3_e, 4_e))->cloneTree();
  List::BubbleUp(l1, [](Tree*) { return false; });
  assert_trees_are_equal(l1, KList(KPow(2_e, 3_e), KPow(2_e, 4_e)));
  l1->removeTree();

  Tree* l2 =
      KMult(2_e, KAdd(3_e, KList(5_e, 6_e)), KList(7_e, 8_e))->cloneTree();
  List::BubbleUp(l2, [](Tree*) { return false; });
  assert_trees_are_equal(l2, KList(KMult(2_e, KAdd(3_e, 5_e), 7_e),
                                   KMult(2_e, KAdd(3_e, 6_e), 8_e)));
  l2->removeTree();
}

QUIZ_CASE(pcj_simplification_list) {
  ProjectionContext replaceSymbolCtx = {
      .m_symbolic = Poincare::SymbolicComputation::ReplaceAllSymbols};
  simplifies_to("{1,2}+3", "{4,5}");
  simplifies_to("{1,2}*{3,4}", "{3,8}");
  simplifies_to("{0,1}=0", "{True,False}");
  simplifies_to("{}=1", "{}");
  simplifies_to("sequence(k, k, 2)<0", "{False,False}");
  simplifies_to("sequence(k<0, k, 2)", "{False,False}");
  simplifies_to("sequence(2*k, k, 3)+1", "{3,5,7}");
  simplifies_to("sum(sequence(2*k*t, k, 3)+1, t, 1, 3)", "{15,27,39}");
  simplifies_to("sum(sequence(1, k, 255))", "255");
  simplifies_to("sequence(1, k, 256)", "undef");
  simplifies_to("sequence(k,k,0)", "{}");
  simplifies_to("mean({1,3*x,2})", "x+1");
  simplifies_to("sum({1,3*x,2})", "3×(x+1)");
  simplifies_to("min({1,-4/7,2,-2})", "-2");
  simplifies_to("var(sequence(k,k,6))", "35/12");
  simplifies_to("sort({2+1,1,2})", "{1,2,3}");
  simplifies_to("med(π*{1,2,3},{4,1,1})", "π");
  simplifies_to("med(π*{4,2,1,3})", "(5×π)/2");
  simplifies_to("dim(1+sequence(k,k,3))", "3");
  simplifies_to("mean({1,2}, {2,1})", "4/3");
  simplifies_to("dim({{1,2}})", "undef");
  simplifies_to("{2*[[1]]}", "undef");
  simplifies_to("{(1)/2, 1}", "{1/2, 1}");
  simplifies_to("prod({1,1,1})", "1");
  simplifies_to("sort({})", "{}");
  simplifies_to("prod({})", "1");
  simplifies_to("{dim({2,4})}", "{2}");
  simplifies_to("min({})", "undef");
  simplifies_to("sequence(k,k,{1,2})", "undef");
  simplifies_to("{3,4}(2)", "4");
  simplifies_to("{3,4}(1,2)", "{3,4}");
  simplifies_to("{3,4}(0,5)", "{3,4}");
  simplifies_to("0*{3,4}", "{0,0}");
  simplifies_to("0*{3,inf}", "{0,undef}");
  simplifies_to("0*inf*{3,4}", "{undef,undef}");
  simplifies_to("0^({3,4}*2)", "{0,0}");
  simplifies_to("{1,2}/{1,0}", "{1,undef}");
  simplifies_to("med(π*{undef,nonreal,x,3})", "undef");
  simplifies_to("sort(randintnorep(1,4,4))", "sort(randintnorep(1,4,4))");
  simplifies_to("med(sequence(random(),k,10))", "med(sequence(random(),k,10))");
  simplifies_to("sort({π,3,e})", "{e,3,π}");
  simplifies_to("sort({i,2})", "{undef,undef}");
  simplifies_to("sort(sort({3, 1, 2, 4}))", "{1, 2, 3, 4}");
  simplifies_to("max({π,e})", "π");
  simplifies_to("max({π,i})", "undef");
  simplifies_to("min({π,e})", "e");
  simplifies_to("med({π,3,e})", "3");
  // Ensure that [GetElement] returning [nullptr] doesn't crash
  simplifies_to("mean(sort({x,0}))", "undef", replaceSymbolCtx);
  simplifies_to("mean(sort({x,0}))", "mean(sort({x,0}))", keepAllSymbolsCtx);
  simplifies_to("max(sort({x,0}))", "max(sort({x,0}))", keepAllSymbolsCtx);
  simplifies_to("samplestddev({1,2},sort({x,0}))",
                "samplestddev({1,2},sort({x,0}))", keepAllSymbolsCtx);

  simplifies_to("{3,4}(0)", "undef");
  simplifies_to("{3,4}(4)", "undef");
  simplifies_to("{3,4}(-1)", "undef");
  simplifies_to("{3,4}(1/3)", "undef");
  simplifies_to("{3,4}([[2]])", "undef");
  simplifies_to("{3,4}(true)", "undef");
  simplifies_to("{3,4}(-1,0)", "undef");
  simplifies_to("{3,4}(5,6)", "{}");
  simplifies_to("{3,4}(2,1)", "{}");
  simplifies_to("{3,4}(2,2/3)", "undef");
  simplifies_to("{3,4}(2,true)", "undef");
  simplifies_to("{3,4}(2,[[2]])", "undef");
  simplifies_to("{{0}}", "undef");
  simplifies_to("{0,{}}", "undef");
  simplifies_to("{0,{1}}", "undef");
  simplifies_to("{0}=x", "{undef}", replaceSymbolCtx);
  simplifies_to("{0,x}=1", "{False,undef}", replaceSymbolCtx);
  simplifies_to("{0,1,2,3,4,5,6,7,8,{9}}", "undef");
  simplifies_to("sequence(k*_Gs, k, 2)", "undef");
}
